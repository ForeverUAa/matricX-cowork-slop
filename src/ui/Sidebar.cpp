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
    v->setContentsMargins(12, 16, 12, 16);
    v->setSpacing(10);
    v->addSpacing(24);
    v->addWidget(new OutlinedLabel("Chats", 32), 0, Qt::AlignLeft);
    v->addSpacing(16);

    auto *current = new QPushButton("💬  Current Session");
    current->setFixedHeight(38);
    current->setCursor(Qt::PointingHandCursor);
    current->setStyleSheet(
        "QPushButton {"
        "  background: rgba(16, 185, 129, 0.15);"
        "  color: #10b981;"
        "  border: 1px solid rgba(16, 185, 129, 0.35);"
        "  border-radius: 8px;"
        "  text-align: left;"
        "  padding-left: 14px;"
        "  font-size: 14px;"
        "  font-weight: 600;"
        "}"
        "QPushButton:hover {"
        "  background: rgba(16, 185, 129, 0.25);"
        "}");
    v->addWidget(current);
    v->addStretch();

    m_settings = new IconButton(assets::path("icons/gear.svg"), QString(), QSize(44, 44), QSize(26, 26),
                                QColor(48, 48, 52));
    v->addWidget(m_settings, 0, Qt::AlignLeft);

    setCornerRadius(theme::kWindowRadius);
}

void Sidebar::setCornerRadius(int radius) {
    setStyleSheet(QString("#sidebar {"
                          "  background: #18181b;"
                          "  border-right: 1px solid #27272a;"
                          "  border-top-left-radius: %1px;"
                          "  border-bottom-left-radius: %1px;"
                          "}").arg(radius));
}

}  // namespace mcw
