#pragma once

#include <QAbstractButton>
#include <QColor>

namespace mcw {

// Round window-control button (macOS style: its symbol shows while the whole group is hovered).
class CircleButton : public QAbstractButton {
public:
    enum Kind { Min, Max, Close };

    CircleButton(Kind kind, const QColor &color, QWidget *parent = nullptr);
    void setGroupHover(bool on);

protected:
    void paintEvent(QPaintEvent *) override;

private:
    Kind m_kind;
    QColor m_color;
    bool m_group = false;
};

}  // namespace mcw
