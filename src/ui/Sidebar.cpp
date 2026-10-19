#include "ui/Sidebar.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>

#include "core/Icons.h"
#include "core/SessionManager.h"
#include "core/SettingsManager.h"
#include "core/Theme.h"
#include "core/ThemeManager.h"
#include "widgets/WindowControls.h"

namespace mcw {

Sidebar::Sidebar(QWidget *parent) : QWidget(parent) {
    setObjectName("sidebar");
    setAttribute(Qt::WA_StyledBackground, true);
    setFixedWidth(m_expandedWidth);

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(12, 14, 12, 12);
    root->setSpacing(10);

    // Header bar: macOS Traffic Lights on the left, Toggle & New Chat on the right
    auto *topBar = new QHBoxLayout;
    topBar->setContentsMargins(0, 0, 0, 0);
    topBar->setSpacing(6);

    m_controls = new WindowControls(this);
    topBar->addWidget(m_controls, 0, Qt::AlignVCenter);

    topBar->addStretch();

    m_toggleBtn = new QPushButton(this);
    m_toggleBtn->setIcon(icons::icon(icons::IconType::PanelLeft, QColor(161, 161, 170)));
    m_toggleBtn->setFixedSize(28, 28);
    m_toggleBtn->setCursor(Qt::PointingHandCursor);
    m_toggleBtn->setToolTip("Toggle sidebar");
    m_toggleBtn->setStyleSheet(
        "QPushButton { background: transparent; border: none; border-radius: 6px; }"
        "QPushButton:hover { background: rgba(255, 255, 255, 0.08); }");
    connect(m_toggleBtn, &QPushButton::clicked, this, [this] {
        setCollapsed(!m_collapsed);
    });
    topBar->addWidget(m_toggleBtn);

    m_newThreadBtn = new QPushButton(this);
    m_newThreadBtn->setIcon(icons::icon(icons::IconType::Plus, QColor(244, 244, 245)));
    m_newThreadBtn->setFixedSize(28, 28);
    m_newThreadBtn->setCursor(Qt::PointingHandCursor);
    m_newThreadBtn->setToolTip("New Chat");
    m_newThreadBtn->setStyleSheet(
        "QPushButton { background: transparent; border: none; border-radius: 6px; }"
        "QPushButton:hover { background: rgba(255, 255, 255, 0.08); }");
    connect(m_newThreadBtn, &QPushButton::clicked, this, [] {
        SessionManager::instance().createSession("New Chat");
    });
    topBar->addWidget(m_newThreadBtn);

    root->addLayout(topBar);

    // Search bar
    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setPlaceholderText("Search chats...");
    m_searchEdit->setFixedHeight(32);
    m_searchEdit->setStyleSheet(
        "QLineEdit {"
        "  background: rgba(255, 255, 255, 0.04);"
        "  border: 1px solid rgba(255, 255, 255, 0.08);"
        "  border-radius: 6px;"
        "  color: #f4f4f5;"
        "  font-size: 12px;"
        "  padding: 0 10px;"
        "}"
        "QLineEdit:focus {"
        "  border: 1px solid #10b981;"
        "  background: rgba(255, 255, 255, 0.07);"
        "}");
    connect(m_searchEdit, &QLineEdit::textChanged, this, [this] { refreshSessions(); });
    root->addWidget(m_searchEdit);

    // Recent chats section label
    auto *sectionLabel = new QLabel("Recent", this);
    sectionLabel->setStyleSheet("font-size: 11px; font-weight: 600; color: #71717a; padding: 4px 4px 0 4px;");
    root->addWidget(sectionLabel);

    // Sessions Scroll Area
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setFrameShape(QFrame::NoFrame);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setStyleSheet("QScrollArea { background: transparent; border: none; }");

    m_scrollContent = new QWidget;
    m_scrollContent->setStyleSheet("background: transparent;");
    m_sessionListLayout = new QVBoxLayout(m_scrollContent);
    m_sessionListLayout->setContentsMargins(0, 0, 0, 0);
    m_sessionListLayout->setSpacing(2);
    m_sessionListLayout->addStretch();

    m_scrollArea->setWidget(m_scrollContent);
    root->addWidget(m_scrollArea, 1);

    // Bottom user / settings tile
    auto *footerFrame = new QFrame(this);
    footerFrame->setObjectName("footerFrame");
    footerFrame->setFixedHeight(48);
    footerFrame->setStyleSheet(
        "QFrame#footerFrame {"
        "  background: rgba(255, 255, 255, 0.03);"
        "  border: 1px solid rgba(255, 255, 255, 0.06);"
        "  border-radius: 8px;"
        "}"
        "QFrame#footerFrame:hover {"
        "  background: rgba(255, 255, 255, 0.06);"
        "}");
    auto *footerLayout = new QHBoxLayout(footerFrame);
    footerLayout->setContentsMargins(10, 0, 8, 0);
    footerLayout->setSpacing(8);

    auto *avatarIcon = new QLabel(this);
    avatarIcon->setPixmap(icons::pixmap(icons::IconType::Bot, QSize(18, 18), QColor(16, 185, 129)));
    footerLayout->addWidget(avatarIcon);

    auto *infoCol = new QVBoxLayout;
    infoCol->setContentsMargins(0, 6, 0, 6);
    infoCol->setSpacing(1);

    m_providerBadge = new QLabel("M-Cowork", this);
    m_providerBadge->setStyleSheet("color: #f4f4f5; font-size: 12px; font-weight: 600;");
    infoCol->addWidget(m_providerBadge);

    auto *modelTag = new QLabel("Ready", this);
    modelTag->setStyleSheet("color: #71717a; font-size: 11px;");
    infoCol->addWidget(modelTag);

    footerLayout->addLayout(infoCol, 1);

    m_settingsBtn = new QPushButton(this);
    m_settingsBtn->setIcon(icons::icon(icons::IconType::Gear, QColor(161, 161, 170)));
    m_settingsBtn->setFixedSize(28, 28);
    m_settingsBtn->setCursor(Qt::PointingHandCursor);
    m_settingsBtn->setToolTip("Open Settings");
    m_settingsBtn->setStyleSheet("QPushButton { background: transparent; border: none; border-radius: 4px; } QPushButton:hover { background: rgba(255,255,255,0.08); }");
    footerLayout->addWidget(m_settingsBtn);

    root->addWidget(footerFrame);

    setCornerRadius(theme::kWindowRadius);

    connect(&SessionManager::instance(), &SessionManager::sessionsChanged, this, &Sidebar::refreshSessions);
    connect(&SessionManager::instance(), &SessionManager::currentSessionChanged, this, &Sidebar::refreshSessions);
    connect(&ThemeManager::instance(), &ThemeManager::themeChanged, this, &Sidebar::updateTheme);

    updateTheme();
    refreshSessions();
}

void Sidebar::setCollapsed(bool collapsed) {
    if (m_collapsed == collapsed) return;
    m_collapsed = collapsed;
    setFixedWidth(m_collapsed ? 54 : m_expandedWidth);

    m_searchEdit->setVisible(!m_collapsed);
    m_scrollArea->setVisible(!m_collapsed);
    m_providerBadge->setVisible(!m_collapsed);

    emit collapsedChanged(m_collapsed);
}

void Sidebar::setCornerRadius(int radius) {
    m_radius = radius;
    updateTheme();
}

void Sidebar::updateTheme() {
    const bool dark = ThemeManager::instance().isDark();
    setStyleSheet(QString(
        "#sidebar {"
        "  background: %1;"
        "  border-right: 1px solid %2;"
        "  border-top-left-radius: %3px;"
        "  border-bottom-left-radius: %3px;"
        "}"
    ).arg(dark ? "#121214" : "#f0f0f3",
          dark ? "#27272a" : "#e4e4e7",
          QString::number(m_radius)));
}

void Sidebar::refreshSessions() {
    while (m_sessionListLayout->count() > 1) {
        QLayoutItem *item = m_sessionListLayout->takeAt(0);
        if (item->widget()) delete item->widget();
        delete item;
    }

    const auto sessions = SessionManager::instance().activeSessions();
    const QString currentId = SessionManager::instance().currentSessionId();
    const QString filter = m_searchEdit ? m_searchEdit->text().trimmed().toLower() : QString();

    for (const auto &s : sessions) {
        if (!filter.isEmpty() && !s.title.toLower().contains(filter)) {
            continue;
        }

        const bool isCurrent = (s.id == currentId);

        auto *itemFrame = new QFrame;
        itemFrame->setObjectName("threadItem");
        itemFrame->setFixedHeight(34);
        itemFrame->setStyleSheet(QString(
            "QFrame#threadItem {"
            "  background: %1;"
            "  border: none;"
            "  border-radius: 6px;"
            "}"
            "QFrame#threadItem:hover {"
            "  background: %2;"
            "}"
        ).arg(isCurrent ? "rgba(255, 255, 255, 0.08)" : "transparent",
              isCurrent ? "rgba(255, 255, 255, 0.11)" : "rgba(255, 255, 255, 0.04)"));

        auto *row = new QHBoxLayout(itemFrame);
        row->setContentsMargins(10, 0, 4, 0);
        row->setSpacing(6);

        auto *titleBtn = new QPushButton(s.title);
        titleBtn->setCursor(Qt::PointingHandCursor);
        titleBtn->setStyleSheet(QString(
            "QPushButton {"
            "  background: transparent;"
            "  border: none;"
            "  text-align: left;"
            "  color: %1;"
            "  font-size: 13px;"
            "  font-weight: %2;"
            "}"
        ).arg(isCurrent ? "#f4f4f5" : "#a1a1aa", isCurrent ? "600" : "400"));
        connect(titleBtn, &QPushButton::clicked, this, [s] {
            SessionManager::instance().setCurrentSession(s.id);
        });
        row->addWidget(titleBtn, 1);

        auto *menuBtn = new QPushButton;
        menuBtn->setIcon(icons::icon(icons::IconType::MoreHorizontal, QColor(161, 161, 170)));
        menuBtn->setFixedSize(20, 20);
        menuBtn->setCursor(Qt::PointingHandCursor);
        menuBtn->setStyleSheet("QPushButton { background: transparent; border: none; border-radius: 3px; } QPushButton:hover { background: rgba(255,255,255,0.12); }");
        connect(menuBtn, &QPushButton::clicked, this, [this, s, menuBtn] {
            showSessionMenu(s.id, menuBtn->mapToGlobal(QPoint(0, menuBtn->height())));
        });
        row->addWidget(menuBtn);

        m_sessionListLayout->insertWidget(m_sessionListLayout->count() - 1, itemFrame);
    }

    const QString p = SettingsManager::instance().provider();
    const auto cfg = SettingsManager::providerConfig(p);
    m_providerBadge->setText(cfg.name.split(' ').first());
}

void Sidebar::showSessionMenu(const QString &sessionId, const QPoint &pos) {
    QMenu menu(this);
    menu.setStyleSheet(
        "QMenu { background: #27272a; border: 1px solid #3f3f46; border-radius: 6px; padding: 4px; }"
        "QMenu::item { color: #f4f4f5; padding: 6px 14px; border-radius: 4px; font-size: 12px; }"
        "QMenu::item:selected { background: #3f3f46; color: #ffffff; }");

    auto *archiveAct = menu.addAction(icons::icon(icons::IconType::Archive, QColor(200, 200, 200)), "Archive Chat");
    auto *deleteAct = menu.addAction(icons::icon(icons::IconType::Trash, QColor(239, 68, 68)), "Delete Permanently");

    connect(archiveAct, &QAction::triggered, this, [sessionId] {
        SessionManager::instance().archiveSession(sessionId);
    });

    connect(deleteAct, &QAction::triggered, this, [this, sessionId] {
        auto res1 = QMessageBox::question(this, "Delete Chat",
            "Delete this conversation permanently? This cannot be undone.",
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (res1 != QMessageBox::Yes) return;

        auto res2 = QMessageBox::warning(this, "Confirm Permanent Deletion",
            "Are you completely sure? All messages and attachments will be deleted.",
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (res2 == QMessageBox::Yes) {
            SessionManager::instance().deleteSessionPermanently(sessionId);
        }
    });

    menu.exec(pos);
}

}  // namespace mcw
