#include "widgets/CircleButton.h"

#include <QEasingCurve>
#include <QPainter>
#include <QPolygonF>

namespace mcw {

CircleButton::CircleButton(Kind kind, const QColor &color, QWidget *parent)
    : QAbstractButton(parent), m_kind(kind), m_color(color) {
    setFixedSize(20, 20);  // bigger click target
    setCursor(Qt::PointingHandCursor);

    m_anim = new QPropertyAnimation(this, "hoverProgress", this);
    m_anim->setDuration(160);
    m_anim->setEasingCurve(QEasingCurve::OutCubic);
}

void CircleButton::setHoverProgress(float val) {
    m_hoverProgress = val;
    update();
}

void CircleButton::setGroupHover(bool on) {
    if (m_group == on) return;
    m_group = on;
    m_anim->stop();
    m_anim->setStartValue(m_hoverProgress);
    m_anim->setEndValue(on ? 1.0f : 0.0f);
    m_anim->start();
}

void CircleButton::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // Subtle hairline border and vibrant fill
    QColor fill = isDown() ? m_color.darker(120) : m_color;
    p.setPen(QPen(fill.darker(130), 0.75));
    p.setBrush(fill);
    p.drawEllipse(rect().adjusted(1, 1, -1, -1));

    if (m_hoverProgress <= 0.01f) return;

    // Smoothly fade-in inner symbol
    QColor ink = m_color.darker(340);
    ink.setAlpha(int(240 * m_hoverProgress));

    p.translate(width() / 2.0, height() / 2.0);
    p.scale(width() / 14.0, height() / 14.0);  // symbols were drawn for a 14px button
    p.setPen(QPen(ink, 1.3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.setBrush(Qt::NoBrush);

    switch (m_kind) {
    case Close:
        p.drawLine(QPointF(-2.5, -2.5), QPointF(2.5, 2.5));
        p.drawLine(QPointF(-2.5, 2.5), QPointF(2.5, -2.5));
        break;
    case Min:
        p.drawLine(QPointF(-3.2, 0), QPointF(3.2, 0));
        break;
    case Max:
        p.setPen(Qt::NoPen);
        p.setBrush(ink);
        p.drawPolygon(QPolygonF() << QPointF(2.8, -2.8) << QPointF(2.8, 0.4) << QPointF(-0.4, -2.8));
        p.drawPolygon(QPolygonF() << QPointF(-2.8, 2.8) << QPointF(-2.8, -0.4) << QPointF(0.4, 2.8));
        break;
    }
}

}  // namespace mcw
