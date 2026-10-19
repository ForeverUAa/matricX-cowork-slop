#pragma once

#include <QList>
#include <QPushButton>
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
class ModelSelector;
class MicButton;
class DotMatrixWordmark;
class ActionStepCard;

// Right panel: OpenAI Codex-style canvas with centered stream, action cards, and floating input capsule.
class ChatArea : public QWidget {
    Q_OBJECT
public:
    explicit ChatArea(QWidget *parent = nullptr);

    WindowControls *windowControls() const { return m_controls; }
    QPushButton *uncollapseButton() const { return m_uncollapseBtn; }
    void focusInput();
    void setCornerRadius(int radius);

public slots:
    void loadCurrentSession();
    void setSidebarCollapsed(bool collapsed);

protected:
    void paintEvent(QPaintEvent *) override;

private:
    void setupEmptyState(QWidget *container);
    void addMessage(const QString &text, bool fromUser);
    void addActionCard(const QString &toolName, const QJsonObject &args);
    void updateStatusIndicator(const QString &text, bool active);
    void updateSendButtonState();
    void send();
    void runAgentLoop();
    void executeNextToolCall(const QList<ToolCallRequest> &calls, int index);

    QPushButton *m_uncollapseBtn = nullptr;
    QLabel *m_threadTitleLabel = nullptr;
    WindowControls *m_controls = nullptr;

    QScrollArea *m_scroll = nullptr;
    QWidget *m_scrollContent = nullptr;
    QVBoxLayout *m_msgLayout = nullptr;
    QWidget *m_emptyStateWidget = nullptr;
    DotMatrixWordmark *m_wordmark = nullptr;

    // Floating input capsule
    QFrame *m_capsule = nullptr;
    QLineEdit *m_input = nullptr;
    ModelSelector *m_modelSelector = nullptr;
    MicButton *m_micBtn = nullptr;
    QPushButton *m_sendBtn = nullptr;
    QLabel *m_statusPill = nullptr;

    bool m_hasMessages = false;
    CoworkTool m_tool;
    AiClient m_ai;
    QList<ChatMessage> m_history;
    bool m_busy = false;
    int m_radius = theme::kWindowRadius;

    ActionStepCard *m_currentActionCard = nullptr;
    float m_pulsePhase = 0.0f;
    QTimer m_animTimer;
};

}  // namespace mcw
