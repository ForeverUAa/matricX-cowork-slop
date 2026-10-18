#pragma once

#include <QList>
#include <QString>
#include <QTimer>
#include <QWidget>

#include "ai/AiClient.h"
#include "core/Theme.h"
#include "tools/CoworkTool.h"

class QLineEdit;
class QScrollArea;
class QVBoxLayout;
class QLabel;
class QFrame;

namespace mcw {

class WindowControls;

// Right panel: modern smooth dark theme with subtle glows, message bubbles, input bar
// and the window controls in the top-right corner.
class ChatArea : public QWidget {
    Q_OBJECT
public:
    explicit ChatArea(QWidget *parent = nullptr);

    WindowControls *windowControls() const { return m_controls; }
    void focusInput();
    void setCornerRadius(int radius);

protected:
    void paintEvent(QPaintEvent *) override;

private:
    void addMessage(const QString &text, bool fromUser);
    void updateStatusIndicator(const QString &text, bool active);
    void send();
    void runAgentLoop();
    void executeNextToolCall(const QList<ToolCallRequest> &calls, int index);

    WindowControls *m_controls = nullptr;
    QScrollArea *m_scroll = nullptr;
    QVBoxLayout *m_msgLayout = nullptr;
    QLineEdit *m_input = nullptr;
    QFrame *m_inputBar = nullptr;
    QLabel *m_statusPill = nullptr;
    bool m_hasMessages = false;
    CoworkTool m_tool;
    AiClient m_ai;
    QList<ChatMessage> m_history;
    bool m_busy = false;
    int m_radius = theme::kWindowRadius;

    // Subtle pulsing animation for background glow & active agent
    float m_pulsePhase = 0.0f;
    QTimer m_animTimer;
};

}  // namespace mcw
