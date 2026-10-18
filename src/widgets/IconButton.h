#pragma once

#include <QAbstractButton>
#include <QColor>
#include <QEnterEvent>
#include <QSize>
#include <QString>
#include <QSvgRenderer>

namespace mcw {

// Button that draws an SVG icon from the assets folder (optionally a different one on hover)
// on top of an optional rounded background.
class IconButton : public QAbstractButton {
public:
    IconButton(const QString &iconFile, const QString &hoverIconFile, const QSize &buttonSize,
               const QSize &iconSize, const QColor &background, QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *) override;
    void enterEvent(QEnterEvent *) override;
    void leaveEvent(QEvent *) override;

private:
    QSvgRenderer m_icon;
    QSvgRenderer m_hoverIcon;
    QSize m_iconSize;
    QColor m_bg;
    bool m_hover = false;
};

}  // namespace mcw
