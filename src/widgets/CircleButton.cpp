#include "widgets/CircleButton.h"

#include <QPainter>
#include <QPolygonF>

namespace mcw {

CircleButton::CircleButton(Kind kind, const QColor &color, QWidget *parent)
    : QAbstractButton(parent), m_kind(kind), m_color(color) {
    setFixedSize(26, 26);
    setCursor(Qt::PointingHandCursor);
}

void CircleButton::setGroupHover(bool on) {
    m_group = on;
    update();
}

void CircleButton::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);
    p.setBrush(isDown() ? m_color.darker(115) : m_color);
    p.drawEllipse(rect().adjusted(1, 1, -1, -1));
    if (!m_group) return;

    const QColor ink = m_color.darker(320);
    p.translate(width() / 2.0, height() / 2.0);
    p.setPen(QPen(ink, 1.9, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.setBrush(Qt::NoBrush);
    switch (m_kind) {
    case Close:
        p.drawLine(QPointF(-4, -4), QPointF(4, 4));
        p.drawLine(QPointF(-4, 4), QPointF(4, -4));
        break;
    case Min:
        p.drawLine(QPointF(-5, 0), QPointF(5, 0));
        break;
    case Max:
        p.setPen(Qt::NoPen);
        p.setBrush(ink);
        p.drawPolygon(QPolygonF() << QPointF(4.5, -4.5) << QPointF(4.5, 0.5) << QPointF(-0.5, -4.5));
        p.drawPolygon(QPolygonF() << QPointF(-4.5, 4.5) << QPointF(-4.5, -0.5) << QPointF(0.5, 4.5));
        break;
    }
}

}  // namespace mcw
