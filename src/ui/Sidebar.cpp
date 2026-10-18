#include "ui/Sidebar.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>

#include "core/Icons.h"
#include "core/SessionManager.h"
#include "core/Theme.h"
#include "core/ThemeManager.h"
#include "widgets/OutlinedLabel.h"

namespace mcw {

Sidebar::Sidebar(QWidget *parent) : QWidget(parent) {
    setObjectName("sidebar");
    setAttribute(Qt::WA_StyledBackground, true);
    setFixedWidth(theme::kSidebarWidth);

    auto *v = new QVBoxLayout(this);
    v->setContentsMargins(12, 16, 12, 16);
    v->setSpacing(10);
    v->addSpacing(16);

    // Header row: Chats label + New Chat button
    auto *headerRow = new QHBoxLayout;
    headerRow->addWidget(new OutlinedLabel("Chats", 28), 0, Qt::AlignLeft);
    headerRow->addStretch();

    m_newChatBtn = new QPushButton;
    m_newChatBtn->setIcon(icons::icon(icons::IconType::Plus, ThemeManager::instance().colors().accent));
    m_newChatBtn->setFixedSize(30, 30);
    m_newChatBtn->setCursor(Qt::PointingHandCursor);
    m_newChatBtn->setToolTip("New Session");
    m_newChatBtn->setStyleSheet(
        "QPushButton { background: rgba(16, 185, 129, 0.12); border: 1px solid rgba(16, 185, 129, 0.3); border-radius: 6px; }"
        "QPushButton:hover { background: rgba(16, 185, 129, 0.25); }");
    connect(m_newChatBtn, &QPushButton::clicked, this, [] {
        SessionManager::instance().createSession("New Session");
    });
    headerRow->addWidget(m_newChatBtn);
    v->addLayout(headerRow);

    v->addSpacing(10);

    // Scrollable active sessions list
    auto *scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setStyleSheet("QScrollArea { background: transparent; border: none; }");

    auto *content = new QWidget;
    content->setStyleSheet("background: transparent;");
    m_sessionListLayout = new QVBoxLayout(content);
    m_sessionListLayout->setContentsMargins(0, 0, 0, 0);
    m_sessionListLayout->setSpacing(6);
    m_sessionListLayout->addStretch();

    scroll->setWidget(content);
    v->addWidget(scroll, 1);

    // Settings trigger button at bottom
    m_settingsBtn = new QPushButton(" Settings");
    m_settingsBtn->setIcon(icons::icon(icons::IconType::Gear, QColor(200, 200, 200)));
    m_settingsBtn->setFixedHeight(38);
    m_settingsBtn->setCursor(Qt::PointingHandCursor);
    m_settingsBtn->setStyleSheet(
        "QPushButton {"
        "  background: #242429;"
        "  color: #e5e7eb;"
        "  border: 1px solid #333338;"
        "  border-radius: 6px;"
        "  text-align: left;"
        "  padding-left: 12px;"
        "  font-size: 13px;"
        "  font-weight: 500;"
        "}"
        "QPushButton:hover {"
        "  background: #2e2e36;"
        "  color: #ffffff;"
        "}");
    v->addWidget(m_settingsBtn);

    setCornerRadius(theme::kWindowRadius);

    connect(&SessionManager::instance(), &SessionManager::sessionsChanged, this, &Sidebar::refreshSessions);
    connect(&SessionManager::instance(), &SessionManager::currentSessionChanged, this, &Sidebar::refreshSessions);

    refreshSessions();
}

void Sidebar::setCornerRadius(int radius) {
    m_radius = radius;
    setStyleSheet(QString("#sidebar {"
                          "  background: #141417;"
                          "  border-right: 1px solid #242428;"
                          "  border-top-left-radius: %1px;"
                          "  border-bottom-left-radius: %1px;"
                          "}").arg(m_radius));
}

void Sidebar::refreshSessions() {
    // Clear existing buttons except the final stretch
    while (m_sessionListLayout->count() > 1) {
        QLayoutItem *item = m_sessionListLayout->takeAt(0);
        if (item->widget()) delete item->widget();
        delete item;
    }

    const auto sessions = SessionManager::instance().activeSessions();
    const QString currentId = SessionManager::instance().currentSessionId();

    for (const auto &s : sessions) {
        const bool isCurrent = (s.id == currentId);

        auto *itemFrame = new QFrame;
        itemFrame->setObjectName("sessionCard");
        itemFrame->setFixedHeight(36);
        itemFrame->setStyleSheet(QString(
            "QFrame#sessionCard {"
            "  background: %1;"
            "  border: 1px solid %2;"
            "  border-radius: 6px;"
            "}"
            "QFrame#sessionCard:hover {"
            "  background: %3;"
            "}"
        ).arg(isCurrent ? "rgba(16, 185, 129, 0.14)" : "#1c1c20",
              isCurrent ? "rgba(16, 185, 129, 0.4)" : "transparent",
              isCurrent ? "rgba(16, 185, 129, 0.22)" : "#26262c"));

        auto *row = new QHBoxLayout(itemFrame);
        row->setContentsMargins(10, 0, 4, 0);
        row->setSpacing(6);

        auto *titleBtn = new QPushButton(s.title);
        titleBtn->setCursor(Qt::PointingHandCursor);
        titleBtn->setStyleSheet(QString(
            "QPushButton { background: transparent; border: none; text-align: left; color: %1; font-size: 13px; font-weight: %2; }"
        ).arg(isCurrent ? "#10b981" : "#d1d5db", isCurrent ? "600" : "400"));
        connect(titleBtn, &QPushButton::clicked, this, [s] {
            SessionManager::instance().setCurrentSession(s.id);
        });
        row->addWidget(titleBtn, 1);

        // Overflow action menu button (...)
        auto *menuBtn = new QPushButton;
        menuBtn->setIcon(icons::icon(icons::IconType::MoreVertical, QColor(140, 140, 140)));
        menuBtn->setFixedSize(22, 22);
        menuBtn->setCursor(Qt::PointingHandCursor);
        menuBtn->setStyleSheet("QPushButton { background: transparent; border: none; border-radius: 4px; } QPushButton:hover { background: #374151; }");
        connect(menuBtn, &QPushButton::clicked, this, [this, s, menuBtn] {
            showSessionMenu(s.id, menuBtn->mapToGlobal(QPoint(0, menuBtn->height())));
        });
        row->addWidget(menuBtn);

        m_sessionListLayout->insertWidget(m_sessionListLayout->count() - 1, itemFrame);
    }
}

void Sidebar::showSessionMenu(const QString &sessionId, const QPoint &pos) {
    QMenu menu(this);
    menu.setStyleSheet(
        "QMenu { background: #222228; border: 1px solid #333338; border-radius: 6px; padding: 4px; }"
        "QMenu::item { color: #f3f4f6; padding: 6px 16px; border-radius: 4px; font-size: 12px; }"
        "QMenu::item:selected { background: #323238; color: #ffffff; }");

    auto *archiveAct = menu.addAction(icons::icon(icons::IconType::Archive, QColor(200, 200, 200)), "Archive Session");
    auto *deleteAct = menu.addAction(icons::icon(icons::IconType::Trash, QColor(239, 68, 68)), "Delete Permanently");

    connect(archiveAct, &QAction::triggered, this, [sessionId] {
        SessionManager::instance().archiveSession(sessionId);
    });

    connect(deleteAct, &QAction::triggered, this, [this, sessionId] {
        // Double confirmation for permanent deletion
        auto res1 = QMessageBox::question(this, "Delete Session",
            "Are you sure you want to delete this session? This action is permanent.",
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (res1 != QMessageBox::Yes) return;

        auto res2 = QMessageBox::warning(this, "Final Confirmation",
            "All messages in this session will be permanently erased. Proceed?",
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (res2 == QMessageBox::Yes) {
            SessionManager::instance().deleteSessionPermanently(sessionId);
        }
    });

    menu.exec(pos);
}

}  // namespace mcw
