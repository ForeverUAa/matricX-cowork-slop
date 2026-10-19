#pragma once

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>

namespace mcw {

// OpenAI Codex-style sidebar: collapsible, search filter, clean session list, user/settings footer.
class Sidebar : public QWidget {
    Q_OBJECT
public:
    explicit Sidebar(QWidget *parent = nullptr);

    QPushButton *settingsButton() const { return m_settingsBtn; }
    QPushButton *toggleButton() const { return m_toggleBtn; }
    void setCornerRadius(int radius);

    bool isCollapsed() const { return m_collapsed; }
    void setCollapsed(bool collapsed);

public slots:
    void refreshSessions();

signals:
    void collapsedChanged(bool collapsed);

private:
    void showSessionMenu(const QString &sessionId, const QPoint &pos);
    void updateTheme();

    bool m_collapsed = false;
    int m_expandedWidth = 240;

    QPushButton *m_toggleBtn = nullptr;
    QPushButton *m_newThreadBtn = nullptr;
    QLineEdit *m_searchEdit = nullptr;

    QScrollArea *m_scrollArea = nullptr;
    QWidget *m_scrollContent = nullptr;
    QVBoxLayout *m_sessionListLayout = nullptr;

    QPushButton *m_settingsBtn = nullptr;
    QLabel *m_providerBadge = nullptr;
    int m_radius = 16;
};

}  // namespace mcw
