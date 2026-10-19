#pragma once

#include <QColor>
#include <QFrame>
#include <QImage>
#include <QLabel>
#include <QRect>

class QPainter;

namespace mcw {

// Implemented by an ancestor widget that can render whatever lies behind glass panes.
class BackdropSource {
public:
    virtual ~BackdropSource() = default;
    // `rect` is in the source widget's logical coordinates. The returned image covers it at
    // devicePixelRatio() resolution and carries that ratio.
    // 'requester' is the glass widget asking, so the source can skip rendering the asker's own subtree.
    virtual QImage backdrop(const QRect &rect, const QWidget *requester) const = 0;
    virtual QWidget *backdropWidget() = 0;
};

struct GlassStyle {
    qreal radius = 14;       // corner radius (logical px)
    qreal bevel = 16;        // width of the refracting edge band
    qreal strength = 16;     // max displacement at the very edge
    qreal dispersion = 0.04; // RGB split at the rim (chromatic aberration)
    QColor tint{255, 255, 255, 20};
    QColor rim{255, 255, 255, 190};
    qreal rimWidth = 1.3;
};

namespace glass {

// Core renderer: refracted backdrop + tint + specular rim, with an anti-aliased rounded mask.
// `backdrop` covers the pane plus `margin` logical px on every side.
QImage render(const QImage &backdrop, int margin, const QSize &size, qreal dpr, const GlassStyle &s);

// Draws a glass pane covering the whole of `w`. Falls back to a flat tint with no backdrop source.
void paint(QWidget *w, QPainter &p, const GlassStyle &s);

// Theme-aware presets.
GlassStyle panel(bool dark, qreal radius = 14);
GlassStyle bubble(bool dark, bool fromUser);

}  // namespace glass

// QLabel drawn on a glass pane (set only colour/padding/font in its stylesheet, no background).
class GlassLabel : public QLabel {
public:
    GlassLabel(const QString &text, bool fromUser, QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *e) override;

private:
    bool m_fromUser;
};

// QFrame drawn on a glass pane; the rim lights up in the accent colour while a child has focus.
class GlassFrame : public QFrame {
public:
    explicit GlassFrame(qreal radius = 14, QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *e) override;

private:
    qreal m_radius;
    bool m_focused = false;
};

}  // namespace mcw
