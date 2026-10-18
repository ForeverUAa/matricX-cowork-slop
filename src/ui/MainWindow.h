#pragma once

#include <QByteArray>
#include <QPoint>
#include <QWidget>

namespace mcw {

class ChatArea;
class Sidebar;

// Frameless, rounded top-level window.
// On Windows: behaves like a native window via WM_NCHITTEST / WM_NCCALCSIZE.
// On Linux / cross-platform: supports mouse drag and edge resize.
class MainWindow : public QWidget {
public:
    MainWindow();
    void focusInput();

protected:
    void changeEvent(QEvent *e) override;
    void showEvent(QShowEvent *e) override;
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void keyPressEvent(QKeyEvent *e) override;

private:
    void updateCorners();

    Sidebar *m_sidebar = nullptr;
    ChatArea *m_chat = nullptr;
    bool m_framed = false;
    bool m_dragging = false;
    QPoint m_dragPosition;
};

}  // namespace mcw
