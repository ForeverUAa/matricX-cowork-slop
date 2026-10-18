#pragma once

#include <QHash>
#include <QPointer>
#include <QPushButton>
#include <QRect>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>

class QPropertyAnimation;

namespace mcw {

class IconButton;

// Left panel: "Chats" heading, session creation, active session items with archive/delete menus, and settings button.
class Sidebar : public QWidget {
    Q_OBJECT
    Q_PROPERTY(QRect pillGeometry READ pillGeometry WRITE setPillGeometry)

public:
    explicit Sidebar(QWidget *parent = nullptr);

    QPushButton *settingsButton() const { return m_settingsBtn; }
    void setCornerRadius(int radius);
    QRect pillGeometry() const { return m_pillGeometry; }
    void setPillGeometry(const QRect &geometry);

public slots:
    void refreshSessions();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    void showSessionMenu(const QString &sessionId, const QPoint &pos);
    void movePillTo(QWidget *target, bool immediate = false);
    void registerNavTarget(QWidget *target);

    QVBoxLayout *m_sessionListLayout = nullptr;
    QPushButton *m_settingsBtn = nullptr;
    QPushButton *m_newChatBtn = nullptr;
    QPropertyAnimation *m_pillAnimation = nullptr;
    QPointer<QWidget> m_selectedTarget;
    QHash<QObject *, QPointer<QWidget>> m_navTargets;
    QRect m_pillGeometry;
    bool m_pillVisible = false;
    int m_radius = 16;
};

}  // namespace mcw
