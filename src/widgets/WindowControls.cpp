#include "widgets/WindowControls.h"

#include <QHBoxLayout>

#include "core/Theme.h"

namespace mcw {

WindowControls::WindowControls(QWidget *parent) : QWidget(parent) {
    auto *l = new QHBoxLayout(this);
    l->setContentsMargins(2, 2, 2, 2);
    l->setSpacing(7);

    closeBtn = new CircleButton(CircleButton::Close, theme::kClose);
    minBtn = new CircleButton(CircleButton::Min, theme::kMinimize);
    maxBtn = new CircleButton(CircleButton::Max, theme::kMaximize);

    l->addWidget(closeBtn);
    l->addWidget(minBtn);
    l->addWidget(maxBtn);
}

void WindowControls::enterEvent(QEnterEvent *) { setHover(true); }

void WindowControls::leaveEvent(QEvent *) { setHover(false); }

void WindowControls::setHover(bool on) {
    minBtn->setGroupHover(on);
    maxBtn->setGroupHover(on);
    closeBtn->setGroupHover(on);
}

}  // namespace mcw
