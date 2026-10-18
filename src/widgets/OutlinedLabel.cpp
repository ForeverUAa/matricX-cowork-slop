#include "widgets/OutlinedLabel.h"

#include <QFontMetrics>
#include <QPainter>
#include <QPainterPath>

namespace mcw {

OutlinedLabel::OutlinedLabel(const QString &text, int pixelSize, QWidget *parent)
    : QWidget(parent), m_text(text) {
    m_font = QFont("Arial Black");
    m_font.setBold(true);
    m_font.setPixelSize(pixelSize);
    const QFontMetrics fm(m_font);
    setFixedSize(kPadX + fm.horizontalAdvance(text) + 10, fm.height() + 8);
}

void OutlinedLabel::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    const QFontMetrics fm(m_font);
    QPainterPath path;
    path.addText(kPadX, 4 + fm.ascent(), m_font, m_text);
    p.strokePath(path, QPen(Qt::black, 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.fillPath(path, Qt::white);
}

}  // namespace mcw
