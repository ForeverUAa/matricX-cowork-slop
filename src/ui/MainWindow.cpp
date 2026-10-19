#include "ui/MainWindow.h"

#include <QAbstractButton>
#include <QEvent>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPalette>
#include <QScreen>

#include "core/Theme.h"
#include "ui/ChatArea.h"
#include "ui/SettingsDialog.h"
#include "ui/Sidebar.h"
#include "widgets/IconButton.h"
#include "widgets/WindowControls.h"

#ifdef Q_OS_WIN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <windowsx.h>
#endif

namespace mcw {

MainWindow::MainWindow() {
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowMinMaxButtonsHint |
                   Qt::WindowSystemMenuHint);
    // Opaque window: translucent layered windows break native maximize/snap/resize.
    // Rounded corners come from DWM (Windows 11) instead.
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(14, 14, 16));
    setPalette(pal);
    setWindowTitle("M-Cowork");
    setMinimumSize(theme::kMinWidth, theme::kMinHeight);
    resize(theme::kDefaultWidth, theme::kDefaultHeight);

    auto *h = new QHBoxLayout(this);
    h->setContentsMargins(0, 0, 0, 0);
    h->setSpacing(0);

    m_sidebar = new Sidebar;
    m_chat = new ChatArea;
    h->addWidget(m_sidebar);
    h->addWidget(m_chat, 1);

    // Wire sidebar collapsing between sidebar and chat header button
    connect(m_sidebar, &Sidebar::collapsedChanged, this, [this](bool collapsed) {
        m_chat->setSidebarCollapsed(collapsed);
    });
    connect(m_chat->uncollapseButton(), &QPushButton::clicked, this, [this] {
        m_sidebar->setCollapsed(false);
    });

    WindowControls *controls = m_chat->windowControls();
    connect(controls->maxBtn, &QAbstractButton::clicked, this, [this] {
        toggleMaximize();
    });
    connect(controls->minBtn, &QAbstractButton::clicked, this, [this] { showMinimized(); });
    connect(controls->closeBtn, &QAbstractButton::clicked, this, [this] { close(); });

    connect(m_sidebar->settingsButton(), &QAbstractButton::clicked, this, [this] {
        SettingsDialog dlg(this);
        dlg.exec();
    });

    updateCorners();
}

void MainWindow::focusInput() { m_chat->focusInput(); }

void MainWindow::updateCorners() {
    const int r = 0;  // opaque window; DWM rounds the corners
    m_sidebar->setCornerRadius(r);
    m_chat->setCornerRadius(r);
}

void MainWindow::toggleMaximize() {
    isMaximized() ? showNormal() : showMaximized();
}

void MainWindow::changeEvent(QEvent *e) {
    QWidget::changeEvent(e);
    if (e->type() == QEvent::WindowStateChange && m_chat && m_sidebar) updateCorners();
}

void MainWindow::showEvent(QShowEvent *e) {
    QWidget::showEvent(e);
#ifdef Q_OS_WIN
    if (!m_framed) {
        m_framed = true;
        HWND h = reinterpret_cast<HWND>(winId());
        const LONG_PTR st = GetWindowLongPtr(h, GWL_STYLE);
        SetWindowLongPtr(h, GWL_STYLE, st | WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
        SetWindowPos(h, nullptr, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
        // Windows 11: let DWM round the corners (DWMWA_WINDOW_CORNER_PREFERENCE = 33, DWMWCP_ROUND = 2)
        if (HMODULE dwm = LoadLibraryW(L"dwmapi.dll")) {
            using DwmSetFn = HRESULT(WINAPI *)(HWND, DWORD, LPCVOID, DWORD);
            if (auto fn = reinterpret_cast<DwmSetFn>(GetProcAddress(dwm, "DwmSetWindowAttribute"))) {
                const int pref = 2;
                fn(h, 33, &pref, sizeof(pref));
            }
        }
    }
#endif
}

bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, qintptr *result) {
#ifdef Q_OS_WIN
    if (eventType == "windows_generic_MSG") {
        MSG *msg = static_cast<MSG *>(message);


        if (msg->message == WM_NCCALCSIZE && msg->wParam) {
            // no native frame/caption is drawn; when maximized, trim the invisible resize border
            if (IsZoomed(msg->hwnd)) {
                auto *p = reinterpret_cast<NCCALCSIZE_PARAMS *>(msg->lParam);
                const UINT dpi = GetDpiForWindow(msg->hwnd);
                const int pad = GetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi);
                const int fx = GetSystemMetricsForDpi(SM_CXSIZEFRAME, dpi) + pad;
                const int fy = GetSystemMetricsForDpi(SM_CYSIZEFRAME, dpi) + pad;
                p->rgrc[0].left += fx;
                p->rgrc[0].right -= fx;
                p->rgrc[0].top += fy;
                p->rgrc[0].bottom -= fy;
            }
            *result = 0;
            return true;
        }

        if (msg->message == WM_NCHITTEST) {
            RECT rc;
            GetWindowRect(msg->hwnd, &rc);
            const int x = GET_X_LPARAM(msg->lParam);
            const int y = GET_Y_LPARAM(msg->lParam);
            const double dpr = devicePixelRatioF();

            if (!isMaximized() && !isFullScreen()) {
                const int b = int(theme::kResizeBorder * dpr);
                const bool l = x < rc.left + b, r = x >= rc.right - b;
                const bool t = y < rc.top + b, bt = y >= rc.bottom - b;
                if (t && l) { *result = HTTOPLEFT; return true; }
                if (t && r) { *result = HTTOPRIGHT; return true; }
                if (bt && l) { *result = HTBOTTOMLEFT; return true; }
                if (bt && r) { *result = HTBOTTOMRIGHT; return true; }
                if (l) { *result = HTLEFT; return true; }
                if (r) { *result = HTRIGHT; return true; }
                if (t) { *result = HTTOP; return true; }
                if (bt) { *result = HTBOTTOM; return true; }
            }

            // top strip acts as the title bar (drag, double-click maximize, Aero Snap),
            // except over the window buttons
            const QPoint local = mapFromGlobal(QPoint(int(x / dpr), int(y / dpr)));
            if (local.y() >= 0 && local.y() < theme::kCaptionHeight) {
                QWidget *w = childAt(local);
                if (!w || (!dynamic_cast<QAbstractButton *>(w) && !dynamic_cast<WindowControls *>(w))) {
                    *result = HTCAPTION;
                    return true;
                }
            }
            *result = HTCLIENT;
            return true;
        }
    }
#else
    Q_UNUSED(eventType)
    Q_UNUSED(message)
    Q_UNUSED(result)
#endif
    return QWidget::nativeEvent(eventType, message, result);
}

void MainWindow::keyPressEvent(QKeyEvent *e) {
    if (e->key() == Qt::Key_F11) {
        isFullScreen() ? showNormal() : showFullScreen();
        e->accept();
        return;
    }
    QWidget::keyPressEvent(e);
}

void MainWindow::mousePressEvent(QMouseEvent *e) {
#ifndef Q_OS_WIN
    if (e->button() == Qt::LeftButton && e->position().y() < theme::kCaptionHeight) {
        QWidget *w = childAt(e->position().toPoint());
        if (!w || (!dynamic_cast<QAbstractButton *>(w) && !dynamic_cast<WindowControls *>(w))) {
            m_dragging = true;
            m_dragPosition = e->globalPosition().toPoint() - frameGeometry().topLeft();
            e->accept();
            return;
        }
    }
#endif
    QWidget::mousePressEvent(e);
}

void MainWindow::mouseMoveEvent(QMouseEvent *e) {
#ifndef Q_OS_WIN
    if (m_dragging && (e->buttons() & Qt::LeftButton)) {
        move(e->globalPosition().toPoint() - m_dragPosition);
        e->accept();
        return;
    }
#endif
    QWidget::mouseMoveEvent(e);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *e) {
#ifndef Q_OS_WIN
    if (m_dragging && e->button() == Qt::LeftButton) {
        m_dragging = false;
        e->accept();
        return;
    }
#endif
    QWidget::mouseReleaseEvent(e);
}

}  // namespace mcw
