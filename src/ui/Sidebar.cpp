#include "ui/Sidebar.h"

#include <QPushButton>
#include <QVBoxLayout>

#include "core/Assets.h"
#include "core/Theme.h"
#include "widgets/IconButton.h"
#include "widgets/OutlinedLabel.h"

namespace mcw {

Sidebar::Sidebar(QWidget *parent) : QWidget(parent) {
    setObjectName("sidebar");
    setAttribute(Qt::WA_StyledBackground, true);
    setFixedWidth(theme::kSidebarWidth);

    auto *v = new QVBoxLayout(this);
    v->setContentsMargins(4, 0, 4, 6);
    v->setSpacing(0);
    v->addSpacing(44);
    v->addWidget(new OutlinedLabel("Chats", 36), 0, Qt::AlignLeft);
    v->addSpacing(32);

    auto *current = new QPushButton("Current");
    current->setFixedHeight(33);
    current->setCursor(Qt::PointingHandCursor);
    current->setStyleSheet(
        QString("QPushButton{background:%1;color:%2;border:none;border-radius:6px;"
                "text-align:left;padding-left:16px;font-size:18px;}"
                "QPushButton:hover{background:%3;}")
            .arg(theme::kChatItemBg.name(), theme::kChatItemText.name(), theme::kChatItemHoverBg.name()));
    v->addWidget(current);
    v->addStretch();

    m_settings = new IconButton(assets::path("icons/gear.svg"), QString(), QSize(52, 52), QSize(52, 52),
                                theme::kSettingsBg);
    v->addWidget(m_settings, 0, Qt::AlignLeft);

    setCornerRadius(theme::kWindowRadius);
}

void Sidebar::setCornerRadius(int radius) {
    setStyleSheet(QString("#sidebar{background:%1;border-top-left-radius:%2px;"
                          "border-bottom-left-radius:%2px;}")
                      .arg(theme::kSidebarBg.name())
                      .arg(radius));
}

}  // namespace mcw
