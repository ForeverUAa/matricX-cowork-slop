#include "widgets/LiquidGlass.h"

#include <QApplication>
#include <QPainter>
#include <QPainterPath>
#include <algorithm>
#include <cmath>
#include <memory>
#include <vector>
#include <QHash>

#include "core/ThemeManager.h"

namespace mcw {
namespace glass {

namespace {

// Per-pixel geometry of a pane (coverage, lens displacement, rim shape). It depends only on the
// pane's size and shape, never on the backdrop, so it is computed once and cached.
struct Geo {
    int W = 0, H = 0;
    std::vector<uint8_t> cov;     // anti-aliased mask, 0..255
    std::vector<uint8_t> mag;     // displacement magnitude in 1/4 px, 0 = none (flat interior)
    std::vector<int8_t> nx, ny;   // outward normal * 127
    std::vector<uint8_t> rim;     // rim shape * 255
    std::vector<uint8_t> inner;   // soft inner glow * 255
};

std::shared_ptr<Geo> geometry(int W, int H, qreal dpr, const GlassStyle &s) {
    static QHash<QString, std::shared_ptr<Geo>> cache;
    const QString key = QStringLiteral("%1x%2/%3/%4/%5/%6/%7/%8")
                            .arg(W).arg(H).arg(dpr).arg(s.radius).arg(s.bevel).arg(s.strength).arg(s.rimWidth).arg(0);
    if (auto it = cache.constFind(key); it != cache.constEnd()) return it.value();
    if (cache.size() > 40) cache.clear();

    auto g = std::make_shared<Geo>();
    g->W = W; g->H = H;
    const size_t N = size_t(W) * size_t(H);
    g->cov.assign(N, 0); g->mag.assign(N, 0); g->nx.assign(N, 0); g->ny.assign(N, 0);
    g->rim.assign(N, 0); g->inner.assign(N, 0);

    const float hw = W * 0.5f, hh = H * 0.5f;
    const float r = float(std::min<qreal>(s.radius * dpr, std::min(hw, hh)));
    const float bevel = float(std::max<qreal>(1.0, s.bevel * dpr));
    const float str = float(s.strength * dpr);
    const float rimW = float(std::max<qreal>(0.5, s.rimWidth * dpr));
    const float lx = -0.7071f, ly = -0.7071f;

    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            const size_t i = size_t(y) * W + x;
            const float px = x + 0.5f - hw, py = y + 0.5f - hh;
            const float qx = std::fabs(px) - (hw - r), qy = std::fabs(py) - (hh - r);
            const float ox = std::max(qx, 0.0f), oy = std::max(qy, 0.0f);
            const float len = std::sqrt(ox * ox + oy * oy);
            const float sdf = len + std::min(std::max(qx, qy), 0.0f) - r;
            const float cov = std::clamp(0.5f - sdf, 0.0f, 1.0f);
            if (cov <= 0.0f) continue;
            g->cov[i] = uint8_t(cov * 255.0f + 0.5f);

            float nx, ny;
            if (len > 1e-4f) {
                nx = ox / len * (px < 0 ? -1.f : 1.f);
                ny = oy / len * (py < 0 ? -1.f : 1.f);
            } else if (qx > qy) {
                nx = px < 0 ? -1.f : 1.f; ny = 0.f;
            } else {
                nx = 0.f; ny = py < 0 ? -1.f : 1.f;
            }
            const float d = std::max(0.0f, -sdf);
            const float t = std::clamp(1.0f - d / bevel, 0.0f, 1.0f);
            const float m = str * (1.0f - std::sqrt(std::max(0.0f, 1.0f - t * t)));
            g->mag[i] = uint8_t(std::clamp(m * 4.0f, 0.0f, 255.0f));
            g->nx[i] = int8_t(std::lround(nx * 127.0f));
            g->ny[i] = int8_t(std::lround(ny * 127.0f));

            const float facing = nx * lx + ny * ly;
            const float spec = std::max(0.0f, facing) + 0.35f * std::max(0.0f, -facing);
            g->rim[i] = uint8_t(std::clamp(std::exp(-d / rimW) * (0.18f + 0.82f * std::min(1.0f, spec)), 0.0f, 1.0f) * 255.0f);
            g->inner[i] = uint8_t(std::clamp(t * t, 0.0f, 1.0f) * 255.0f);
        }
    }
    cache.insert(key, g);
    return g;
}

}  // namespace

QImage render(const QImage &bd, int margin, const QSize &size, qreal dpr, const GlassStyle &s) {
    const int W = std::max(1, qRound(size.width() * dpr));
    const int H = std::max(1, qRound(size.height() * dpr));
    QImage out(W, H, QImage::Format_ARGB32_Premultiplied);
    out.setDevicePixelRatio(dpr);

    const auto geo = geometry(W, H, dpr, s);
    const int mpx = qRound(margin * dpr);
    const float kR = 1.0f + float(s.dispersion), kB = 1.0f - float(s.dispersion);
    const bool haveBd = !bd.isNull() && bd.format() == QImage::Format_ARGB32_Premultiplied;
    const int bw = bd.width(), bh = bd.height();

    const float tintA = s.tint.alphaF();
    const float tr = s.tint.red(), tg = s.tint.green(), tb = s.tint.blue();
    const float rimA = s.rim.alphaF() / 255.0f;
    const float rr = s.rim.red(), rg = s.rim.green(), rb = s.rim.blue();

    for (int y = 0; y < H; ++y) {
        QRgb *dst = reinterpret_cast<QRgb *>(out.scanLine(y));
        const size_t row = size_t(y) * W;
        for (int x = 0; x < W; ++x) {
            const size_t i = row + x;
            const int cv = geo->cov[i];
            if (cv == 0) { dst[x] = 0; continue; }

            float cr = 18, cg = 18, cb = 22;
            if (haveBd) {
                auto fetch = [&](float ox, float oy, int shift) -> int {
                    const int sx = std::clamp(int(x + mpx + ox), 0, bw - 1);
                    const int sy = std::clamp(int(y + mpx + oy), 0, bh - 1);
                    return (reinterpret_cast<const QRgb *>(bd.constScanLine(sy))[sx] >> shift) & 0xff;
                };
                const int mg = geo->mag[i];
                if (mg == 0) {
                    const QRgb c = reinterpret_cast<const QRgb *>(bd.constScanLine(std::clamp(y + mpx, 0, bh - 1)))[std::clamp(x + mpx, 0, bw - 1)];
                    cr = float(qRed(c)); cg = float(qGreen(c)); cb = float(qBlue(c));
                } else {
                    const float m = mg * 0.25f;
                    const float ex = geo->nx[i] * (1.0f / 127.0f) * m, ey = geo->ny[i] * (1.0f / 127.0f) * m;
                    cr = float(fetch(ex * kR, ey * kR, 16));
                    cg = float(fetch(ex, ey, 8));
                    cb = float(fetch(ex * kB, ey * kB, 0));
                }
            }

            cr += (tr - cr) * tintA; cg += (tg - cg) * tintA; cb += (tb - cb) * tintA;
            const float inner = geo->inner[i] * (14.0f / 255.0f);
            cr += inner; cg += inner; cb += inner;
            const float rimI = geo->rim[i] * rimA;
            cr += (rr - cr) * rimI; cg += (rg - cg) * rimI; cb += (rb - cb) * rimI;

            const float cov = cv * (1.0f / 255.0f);
            dst[x] = qRgba(std::clamp(int(cr * cov), 0, 255), std::clamp(int(cg * cov), 0, 255),
                           std::clamp(int(cb * cov), 0, 255), cv);
        }
    }
    return out;
}
void paint(QWidget *w, QPainter &p, const GlassStyle &s) {
    BackdropSource *src = nullptr;
    for (QWidget *a = w->parentWidget(); a && !src; a = a->parentWidget())
        src = dynamic_cast<BackdropSource *>(a);

    const qreal dpr = w->devicePixelRatioF();
    if (!src) {
        p.setRenderHint(QPainter::Antialiasing);
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(30, 30, 34, 230));
        p.drawRoundedRect(QRectF(w->rect()), s.radius, s.radius);
        return;
    }

    const int margin = int(std::ceil(s.bevel + s.strength)) + 2;
    const QPoint origin = w->mapTo(src->backdropWidget(), QPoint(0, 0));
    const QRect area = QRect(origin, w->size()).adjusted(-margin, -margin, margin, margin);
    const QImage bd = src->backdrop(area, w);
    const QImage img = render(bd, margin, w->size(), dpr, s);
    p.drawImage(QPointF(0, 0), img);
}

GlassStyle panel(bool dark, qreal radius) {
    GlassStyle s;
    s.radius = radius;
    if (dark) {
        s.tint = QColor(255, 255, 255, 16);
        s.rim = QColor(255, 255, 255, 170);
    } else {
        s.tint = QColor(255, 255, 255, 150);
        s.rim = QColor(255, 255, 255, 255);
        s.strength = 12;
    }
    return s;
}

GlassStyle bubble(bool dark, bool fromUser) {
    GlassStyle s = panel(dark, 14);
    s.bevel = 12;
    s.strength = 10;
    if (fromUser) {
        s.tint = dark ? QColor(16, 185, 129, 70) : QColor(16, 185, 129, 80);
        s.rim = dark ? QColor(110, 231, 183, 200) : QColor(255, 255, 255, 255);
    } else {
        s.tint = dark ? QColor(148, 163, 184, 30) : QColor(255, 255, 255, 170);
    }
    return s;
}

}  // namespace glass

// ---------------------------------------------------------------------------------------------

GlassLabel::GlassLabel(const QString &text, bool fromUser, QWidget *parent)
    : QLabel(text, parent), m_fromUser(fromUser) {}

void GlassLabel::paintEvent(QPaintEvent *e) {
    {
        QPainter p(this);
        glass::paint(this, p, glass::bubble(ThemeManager::instance().isDark(), m_fromUser));
    }
    QLabel::paintEvent(e);
}

GlassFrame::GlassFrame(qreal radius, QWidget *parent) : QFrame(parent), m_radius(radius) {
    connect(qApp, &QApplication::focusChanged, this, [this](QWidget *, QWidget *now) {
        const bool f = now && (now == this || isAncestorOf(now));
        if (f != m_focused) {
            m_focused = f;
            update();
        }
    });
}

void GlassFrame::paintEvent(QPaintEvent *e) {
    {
        QPainter p(this);
        GlassStyle s = glass::panel(ThemeManager::instance().isDark(), m_radius);
        if (m_focused) {
            s.rim = ThemeManager::instance().colors().accent;
            s.rim.setAlpha(255);
            s.tint = QColor(16, 185, 129, 26);
        }
        glass::paint(this, p, s);
    }
    QFrame::paintEvent(e);
}

}  // namespace mcw
