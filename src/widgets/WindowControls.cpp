#include "widgets/WindowControls.h"

#include <QHBoxLayout>

#include "core/SettingsManager.h"
#include "core/Theme.h"

namespace mcw {

WindowControls::WindowControls(QWidget *parent) : QWidget(parent) {
    m_layout = new QHBoxLayout(this);
    m_layout->setContentsMargins(2, 2, 2, 2);
    m_layout->setSpacing(7);

    closeBtn = new CircleButton(CircleButton::Close, theme::kClose);
    minBtn = new CircleButton(CircleButton::Min, theme::kMinimize);
    maxBtn = new CircleButton(CircleButton::Max, theme::kMaximize);

    m_layout->addWidget(closeBtn);
    m_layout->addWidget(minBtn);
    m_layout->addWidget(maxBtn);

    applyPreferences();
}

void WindowControls::applyPreferences() {
    auto &sm = SettingsManager::instance();

    const QString shape = sm.windowControlsShape();
    CircleButton::Shape s = CircleButton::Circle;
    if (shape == "rounded") s = CircleButton::RoundedSquare;
    else if (shape == "square") s = CircleButton::Square;

    const QString size = sm.windowControlsSize();
    int px = 20;
    int spacing = 7;
    if (size == "small") { px = 16; spacing = 5; }
    else if (size == "large") { px = 26; spacing = 9; }

    for (CircleButton *b : {closeBtn, minBtn, maxBtn}) {
        b->setShape(s);
        b->setButtonSize(px);
    }
    m_layout->setSpacing(spacing);

    adjustSize();
    updateGeometry();
}

void WindowControls::enterEvent(QEnterEvent *) { setHover(true); }

void WindowControls::leaveEvent(QEvent *) { setHover(false); }

void WindowControls::setHover(bool on) {
    minBtn->setGroupHover(on);
    maxBtn->setGroupHover(on);
    closeBtn->setGroupHover(on);
}

}  // namespace mcw
