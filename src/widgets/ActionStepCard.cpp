#include "widgets/ActionStepCard.h"

#include <QDesktopServices>
#include <QHBoxLayout>
#include <QJsonDocument>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <QUrl>
#include <QVBoxLayout>

#include "core/Icons.h"
#include "core/ThemeManager.h"

namespace mcw {

ActionStepCard::ActionStepCard(const QString &toolName, const QJsonObject &args, QWidget *parent)
    : QFrame(parent), m_toolName(toolName), m_args(args) {
    setObjectName("actionStepCard");

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(12, 10, 12, 10);
    mainLayout->setSpacing(6);

    // Header row
    auto *header = new QHBoxLayout;
    header->setSpacing(8);

    m_iconLabel = new QLabel(this);
    const icons::IconType iconType = (toolName == "screenshot") ? icons::IconType::Image : icons::IconType::Terminal;
    m_iconLabel->setPixmap(icons::pixmap(iconType, QSize(16, 16), QColor(16, 185, 129)));
    header->addWidget(m_iconLabel);

    // Human-friendly title
    QString argSummary;
    if (toolName == "mouse_move" || toolName == "mouse_click") {
        if (args.contains("x") && args.contains("y")) {
            argSummary = QString(" (x: %1, y: %2)").arg(args.value("x").toInt()).arg(args.value("y").toInt());
        }
    } else if (toolName == "keyboard_type") {
        QString text = args.value("text").toString();
        if (text.length() > 24) text = text.left(21) + "...";
        argSummary = QString(" \"%1\"").arg(text);
    } else if (toolName == "keyboard_key") {
        argSummary = QString(" [%1]").arg(args.value("keys").toString());
    }

    m_titleLabel = new QLabel(QString("Action: %1%2").arg(toolName, argSummary), this);
    m_titleLabel->setStyleSheet("font-size: 13px; font-weight: 600; color: #f4f4f5; font-family: monospace;");
    header->addWidget(m_titleLabel, 1);

    m_statusBadge = new QLabel("Running...", this);
    m_statusBadge->setStyleSheet(
        "background: rgba(234, 179, 8, 0.15); border: 1px solid rgba(234, 179, 8, 0.4);"
        "color: #fbbf24; font-size: 11px; font-weight: 600; padding: 2px 8px; border-radius: 4px;");
    header->addWidget(m_statusBadge);

    m_chevronBtn = new QPushButton(this);
    m_chevronBtn->setIcon(icons::icon(icons::IconType::ChevronRight, QColor(161, 161, 170)));
    m_chevronBtn->setFixedSize(20, 20);
    m_chevronBtn->setStyleSheet("background: transparent; border: none;");
    header->addWidget(m_chevronBtn);

    mainLayout->addLayout(header);

    // Expandable details section
    m_detailsWidget = new QWidget(this);
    auto *detailsLayout = new QVBoxLayout(m_detailsWidget);
    detailsLayout->setContentsMargins(24, 4, 4, 4);
    detailsLayout->setSpacing(6);

    const QString argsJson = QString::fromUtf8(QJsonDocument(args).toJson(QJsonDocument::Indented)).trimmed();
    m_argsLabel = new QLabel(QString("Input:\n%1").arg(argsJson), m_detailsWidget);
    m_argsLabel->setStyleSheet("font-size: 11px; color: #a1a1aa; font-family: monospace;");
    m_argsLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    detailsLayout->addWidget(m_argsLabel);

    m_resultLabel = new QLabel(m_detailsWidget);
    m_resultLabel->setStyleSheet("font-size: 11px; color: #10b981; font-family: monospace;");
    m_resultLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    m_resultLabel->hide();
    detailsLayout->addWidget(m_resultLabel);

    m_imagePreview = new QLabel(m_detailsWidget);
    m_imagePreview->setCursor(Qt::PointingHandCursor);
    m_imagePreview->hide();
    detailsLayout->addWidget(m_imagePreview);

    m_detailsWidget->hide();
    mainLayout->addWidget(m_detailsWidget);

    connect(m_chevronBtn, &QPushButton::clicked, this, [this] {
        const bool willShow = !m_detailsWidget->isVisible();
        m_detailsWidget->setVisible(willShow);
        m_chevronBtn->setIcon(icons::icon(willShow ? icons::IconType::ChevronDown : icons::IconType::ChevronRight, QColor(161, 161, 170)));
    });

    updateTheme();
}

void ActionStepCard::setStatus(Status s, const QString &resultText, const QString &imagePath) {
    m_status = s;
    if (s == Status::Success) {
        m_statusBadge->setText("Completed");
        m_statusBadge->setStyleSheet(
            "background: rgba(16, 185, 129, 0.15); border: 1px solid rgba(16, 185, 129, 0.4);"
            "color: #34d399; font-size: 11px; font-weight: 600; padding: 2px 8px; border-radius: 4px;");
    } else if (s == Status::Failed) {
        m_statusBadge->setText("Failed");
        m_statusBadge->setStyleSheet(
            "background: rgba(239, 68, 68, 0.15); border: 1px solid rgba(239, 68, 68, 0.4);"
            "color: #f87171; font-size: 11px; font-weight: 600; padding: 2px 8px; border-radius: 4px;");
    }

    if (!resultText.isEmpty()) {
        m_resultLabel->setText(QString("Result:\n%1").arg(resultText));
        m_resultLabel->show();
    }

    if (!imagePath.isEmpty()) {
        QPixmap pm(imagePath);
        if (!pm.isNull()) {
            QPixmap scaled = pm.scaled(QSize(280, 160), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            m_imagePreview->setPixmap(scaled);
            m_imagePreview->setToolTip("Click to view full screenshot in system viewer");
            m_imagePreview->show();
            m_detailsWidget->show(); // auto-reveal screenshot card
            m_chevronBtn->setIcon(icons::icon(icons::IconType::ChevronDown, QColor(161, 161, 170)));

            m_imagePreview->disconnect();
            auto *clickBtn = new QPushButton("View Full Image", m_detailsWidget);
            clickBtn->setCursor(Qt::PointingHandCursor);
            clickBtn->setFixedHeight(26);
            clickBtn->setStyleSheet("background: #27272a; color: #38bdf8; border: 1px solid #3f3f46; border-radius: 4px; font-size: 11px; font-weight: 500; padding: 0 10px;");
            connect(clickBtn, &QPushButton::clicked, this, [imagePath] {
                QDesktopServices::openUrl(QUrl::fromLocalFile(imagePath));
            });
            m_detailsWidget->layout()->addWidget(clickBtn);
        }
    }
}

void ActionStepCard::updateTheme() {
    const bool dark = ThemeManager::instance().isDark();
    setStyleSheet(QString(
        "QFrame#actionStepCard {"
        "  background: %1;"
        "  border: 1px solid %2;"
        "  border-radius: 8px;"
        "}"
    ).arg(dark ? "#202024" : "#f4f4f5",
          dark ? "#333338" : "#e4e4e7"));
}

}  // namespace mcw
