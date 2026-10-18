#pragma once

#include <QAbstractButton>
#include <QColor>
#include <QPropertyAnimation>
#include <QTimer>

namespace mcw {

// Microphone button with prototype emerald accent, smooth hover, recording, and muted states.
class MicButton : public QAbstractButton {
    Q_OBJECT
    Q_PROPERTY(float pulseProgress READ pulseProgress WRITE setPulseProgress)
public:
    enum State {
        Idle,
        Recording,
        Muted
    };

    explicit MicButton(QWidget *parent = nullptr);

    State state() const { return m_state; }
    void setState(State s);

    float pulseProgress() const { return m_pulseProgress; }
    void setPulseProgress(float v);

protected:
    void paintEvent(QPaintEvent *) override;
    void enterEvent(QEnterEvent *) override;
    void leaveEvent(QEvent *) override;

private:
    void updateTooltip();

    State m_state = Idle;
    bool m_hover = false;
    float m_pulseProgress = 0.0f;
    QTimer m_pulseTimer;
};

}  // namespace mcw
