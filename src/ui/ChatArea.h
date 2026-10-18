#pragma once

#include <QList>
#include <QString>
#include <QWidget>

#include "ai/AiClient.h"
#include "core/Theme.h"
#include "tools/CoworkTool.h"

class QLineEdit;
class QScrollArea;
class QVBoxLayout;

namespace mcw {

class WindowControls;

// Right panel: black background with green gradient, big title, messages, input bar
// and the window controls in the top-right corner.
class ChatArea : public QWidget {
public:
    explicit ChatArea(QWidget *parent = nullptr);

    WindowControls *windowControls() const { return m_controls; }
    void focusInput();
    void setCornerRadius(int radius);

protected:
    void paintEvent(QPaintEvent *) override;

private:
    void addMessage(const QString &text, bool fromUser);
    void send();
    void runAgentLoop();
    void executeNextToolCall(const QList<ToolCallRequest> &calls, int index);

    WindowControls *m_controls = nullptr;
    QScrollArea *m_scroll = nullptr;
    QVBoxLayout *m_msgLayout = nullptr;
    QLineEdit *m_input = nullptr;
    bool m_hasMessages = false;
    CoworkTool m_tool;
    AiClient m_ai;
    QList<ChatMessage> m_history;
    bool m_busy = false;
    int m_radius = theme::kWindowRadius;
};

}  // namespace mcw
