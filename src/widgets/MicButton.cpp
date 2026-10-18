#include "widgets/MicButton.h"

#include <QPainter>
#include <cmath>

#include "core/Icons.h"
#include "core/ThemeManager.h"

namespace mcw {

MicButton::MicButton(QWidget *parent) : QAbstractButton(parent) {
    setFixedSize(46, 46);
    setCursor(Qt::PointingHandCursor);
    updateTooltip();

    connect(&m_pulseTimer, &QTimer::timeout, this, [this] {
        m_pulseProgress += 0.08f;
        if (m_pulseProgress > 6.28318f) m_pulseProgress -= 6.28318f;
        update();
    });
}

void MicButton::setPulseProgress(float v) {
    m_pulseProgress = v;
    update();
}

void MicButton::setState(State s) {
    if (m_state == s) return;
    m_state = s;
    if (m_state == Recording) {
        m_pulseTimer.start(30);
    } else {
        m_pulseTimer.stop();
        m_pulseProgress = 0.0f;
    }
    updateTooltip();
    update();
}

void MicButton::updateTooltip() {
    switch (m_state) {
    case Idle:
        setToolTip("Voice Input (Click to start recording)");
        setAccessibleName("Microphone voice input idle");
        break;
    case Recording:
        setToolTip("Recording audio... (Click to stop)");
        setAccessibleName("Microphone recording active");
        break;
    case Muted:
        setToolTip("Microphone muted (Click to unmute)");
        setAccessibleName("Microphone muted");
        break;
    }
}

void MicButton::enterEvent(QEnterEvent *) {
    m_hover = true;
    update();
}

void MicButton::leaveEvent(QEvent *) {
    m_hover = false;
    update();
}

void MicButton::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    const auto &colors = ThemeManager::instance().colors();
    const bool dark = ThemeManager::instance().isDark();

    // Prototype emerald accent
    QColor accent = colors.accent;       // #10b981
    QColor bg;
    QColor iconColor;

    if (m_state == Recording) {
        // Red recording ring + pulsing halo
        const float wave = 0.5f + 0.5f * std::sin(m_pulseProgress);
        QColor halo(239, 68, 68, int(80 * wave));
        p.setPen(Qt::NoPen);
        p.setBrush(halo);
        p.drawEllipse(rect().adjusted(1, 1, -1, -1));

        bg = QColor(220, 38, 38);
        iconColor = Qt::white;
    } else if (m_state == Muted) {
        bg = dark ? QColor(40, 40, 45) : QColor(226, 232, 240);
        iconColor = colors.textMuted;
    } else {
        // Idle state: vibrant prototype emerald accent button
        if (m_hover) {
            bg = colors.accentHover;
            iconColor = dark ? QColor(10, 30, 20) : Qt::white;
        } else {
            bg = accent;
            iconColor = dark ? QColor(6, 40, 25) : Qt::white;
        }
    }

    if (isDown()) {
        bg = bg.darker(115);
    }

    // Outer circle container
    p.setPen(Qt::NoPen);
    p.setBrush(bg);
    p.drawEllipse(rect().adjusted(4, 4, -4, -4));

    // Lucide mic or mic-off icon
    const icons::IconType icType = (m_state == Muted) ? icons::IconType::MicOff : icons::IconType::Mic;
    const QPixmap pm = icons::pixmap(icType, QSize(20, 20), iconColor, 2.2f);
    p.drawPixmap((width() - pm.width()) / 2, (height() - pm.height()) / 2, pm);
}

}  // namespace mcw
