#pragma once

#include <QByteArray>
#include <QWidget>

namespace mcw {

class ChatArea;
class Sidebar;

// Frameless, rounded top-level window that still behaves like a normal Windows window
// (drag, Aero Snap, resize edges, maximize, taskbar minimize) via native hit-testing.
class MainWindow : public QWidget {
public:
    MainWindow();
    void focusInput();

protected:
    void changeEvent(QEvent *e) override;
    void showEvent(QShowEvent *e) override;
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;

private:
    void updateCorners();

    Sidebar *m_sidebar = nullptr;
    ChatArea *m_chat = nullptr;
    bool m_framed = false;
};

}  // namespace mcw
