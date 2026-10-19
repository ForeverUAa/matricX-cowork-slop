#include "widgets/WindowControls.h"

#include <QHBoxLayout>

#include "core/Theme.h"

namespace mcw {

WindowControls::WindowControls(QWidget *parent) : QWidget(parent) {
    auto *l = new QHBoxLayout(this);
    l->setContentsMargins(6, 6, 6, 6);
    l->setSpacing(10);

    closeBtn = new CircleButton(CircleButton::Close, theme::kClose);
    minBtn = new CircleButton(CircleButton::Min, theme::kMinimize);
    maxBtn = new CircleButton(CircleButton::Max, theme::kMaximize);

    // Windows order: minimize, maximize, close (close sits at the far right corner)
    l->addWidget(minBtn);
    l->addWidget(maxBtn);
    l->addWidget(closeBtn);
}

void WindowControls::enterEvent(QEnterEvent *) { setHover(true); }

void WindowControls::leaveEvent(QEvent *) { setHover(false); }

void WindowControls::setHover(bool on) {
    minBtn->setGroupHover(on);
    maxBtn->setGroupHover(on);
    closeBtn->setGroupHover(on);
}

}  // namespace mcw
