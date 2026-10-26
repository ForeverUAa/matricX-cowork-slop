#pragma once

#include <QAbstractButton>
#include <QColor>
#include <QPropertyAnimation>

namespace mcw {

// Round window-control button (sleeker, smaller macOS style with smooth transitions).
class CircleButton : public QAbstractButton {
    Q_OBJECT
    Q_PROPERTY(float hoverProgress READ hoverProgress WRITE setHoverProgress)
public:
    enum Kind { Min, Max, Close };
    enum Shape { Circle, RoundedSquare, Square };

    CircleButton(Kind kind, const QColor &color, QWidget *parent = nullptr);
    void setGroupHover(bool on);

    void setShape(Shape shape);
    Shape shape() const { return m_shape; }
    void setButtonSize(int px);

    float hoverProgress() const { return m_hoverProgress; }
    void setHoverProgress(float val);

protected:
    void paintEvent(QPaintEvent *) override;

private:
    Kind m_kind;
    QColor m_color;
    Shape m_shape = Circle;
    bool m_group = false;
    float m_hoverProgress = 0.0f;
    QPropertyAnimation *m_anim = nullptr;
};

}  // namespace mcw
