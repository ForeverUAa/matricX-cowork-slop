#include "widgets/IconButton.h"

#include <QPainter>

namespace mcw {

IconButton::IconButton(const QString &iconFile, const QString &hoverIconFile, const QSize &buttonSize,
                       const QSize &iconSize, const QColor &background, QWidget *parent)
    : QAbstractButton(parent),
      m_icon(iconFile),
      m_hoverIcon(hoverIconFile),
      m_iconSize(iconSize),
      m_bg(background) {
    setFixedSize(buttonSize);
    setCursor(Qt::PointingHandCursor);
}

void IconButton::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    if (m_bg.alpha() > 0) {
        p.setPen(Qt::NoPen);
        p.setBrush(m_hover ? m_bg.lighter(115) : m_bg);
        p.drawRoundedRect(rect(), 6, 6);
    }
    QSvgRenderer &r = (m_hover && m_hoverIcon.isValid()) ? m_hoverIcon : m_icon;
    const QRectF target((width() - m_iconSize.width()) / 2.0, (height() - m_iconSize.height()) / 2.0,
                        m_iconSize.width(), m_iconSize.height());
    r.render(&p, target);
}

void IconButton::enterEvent(QEnterEvent *) {
    m_hover = true;
    update();
}

void IconButton::leaveEvent(QEvent *) {
    m_hover = false;
    update();
}

}  // namespace mcw
