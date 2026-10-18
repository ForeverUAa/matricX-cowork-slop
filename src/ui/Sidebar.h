#pragma once

#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>

namespace mcw {

class IconButton;

// Left panel: "Chats" heading, session creation, active session items with archive/delete menus, and settings button.
class Sidebar : public QWidget {
    Q_OBJECT
public:
    explicit Sidebar(QWidget *parent = nullptr);

    QPushButton *settingsButton() const { return m_settingsBtn; }
    void setCornerRadius(int radius);

public slots:
    void refreshSessions();

private:
    void showSessionMenu(const QString &sessionId, const QPoint &pos);

    QVBoxLayout *m_sessionListLayout = nullptr;
    QPushButton *m_settingsBtn = nullptr;
    QPushButton *m_newChatBtn = nullptr;
    int m_radius = 16;
};

}  // namespace mcw
