#include "ui/SettingsDialog.h"

#include <QComboBox>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPainter>
#include <QPainterPath>
#include <QPushButton>
#include <QVBoxLayout>

#include "ai/AntigravityAuth.h"
#include "core/SettingsManager.h"
#include "core/Theme.h"

namespace mcw {

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("AI Provider Settings");
    setModal(true);
    setFixedSize(560, 560);
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(14);

    // Header bar (Title + Close button)
    auto *headerLayout = new QHBoxLayout;
    auto *title = new QLabel("AI Provider Settings");
    title->setStyleSheet("color: #ffffff; font-size: 20px; font-weight: bold;");
    headerLayout->addWidget(title);
    headerLayout->addStretch();

    auto *closeBtn = new QPushButton("×");
    closeBtn->setFixedSize(28, 28);
    closeBtn->setCursor(Qt::PointingHandCursor);
    closeBtn->setStyleSheet(
        "QPushButton { background: transparent; color: #aaaaaa; border: none; font-size: 20px; font-weight: bold; border-radius: 14px; }"
        "QPushButton:hover { background: #444444; color: #ffffff; }");
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::reject);
    headerLayout->addWidget(closeBtn);
    mainLayout->addLayout(headerLayout);

    // Form fields styling
    const QString labelStyle = "color: #cccccc; font-size: 13px; font-weight: 500;";
    const QString inputStyle = 
        "QLineEdit, QComboBox {"
        "  background: #383838;"
        "  border: 1px solid #505050;"
        "  border-radius: 8px;"
        "  color: #eeeeee;"
        "  padding: 8px 12px;"
        "  font-size: 14px;"
        "}"
        "QLineEdit:focus, QComboBox:focus {"
        "  border: 1px solid #10aa00;"
        "}"
        "QComboBox::drop-down {"
        "  border: none;"
        "  padding-right: 10px;"
        "}"
        "QComboBox QAbstractItemView {"
        "  background: #2b2b2b;"
        "  color: #eeeeee;"
        "  selection-background-color: #1d7a12;"
        "  selection-color: #ffffff;"
        "  border: 1px solid #444444;"
        "}";

    // Provider Selector
    auto *providerLayout = new QVBoxLayout;
    providerLayout->setSpacing(6);
    auto *providerLabel = new QLabel("Provider");
    providerLabel->setStyleSheet(labelStyle);
    providerLayout->addWidget(providerLabel);

    m_providerCombo = new QComboBox;
    m_providerCombo->setStyleSheet(inputStyle);
    for (const auto &p : SettingsManager::availableProviders()) {
        m_providerCombo->addItem(p.name, p.id);
    }
    providerLayout->addWidget(m_providerCombo);
    mainLayout->addLayout(providerLayout);

    // Google Antigravity OAuth Box
    auto *oauthBox = new QFrame;
    oauthBox->setObjectName("oauthBox");
    oauthBox->setStyleSheet("QFrame#oauthBox { background: #323232; border: 1px solid #444; border-radius: 8px; padding: 10px; }");
    auto *oauthLayout = new QHBoxLayout(oauthBox);
    oauthLayout->setContentsMargins(12, 8, 12, 8);
    oauthLayout->setSpacing(12);

    auto *oauthLeft = new QVBoxLayout;
    oauthLeft->setSpacing(2);
    auto *oauthTitle = new QLabel("Google Antigravity OAuth");
    oauthTitle->setStyleSheet("color: #ffffff; font-size: 13px; font-weight: bold;");
    m_oauthStatusLabel = new QLabel("Not logged in");
    m_oauthStatusLabel->setStyleSheet("color: #888888; font-size: 12px;");
    oauthLeft->addWidget(oauthTitle);
    oauthLeft->addWidget(m_oauthStatusLabel);
    oauthLayout->addLayout(oauthLeft, 1);

    m_oauthBtn = new QPushButton("Login with Google");
    m_oauthBtn->setCursor(Qt::PointingHandCursor);
    m_oauthBtn->setFixedHeight(32);
    m_oauthBtn->setStyleSheet(
        "QPushButton { background: #2563eb; color: #ffffff; border: none; border-radius: 6px; font-size: 13px; font-weight: 600; padding: 0 14px; }"
        "QPushButton:hover { background: #1d4ed8; }");
    connect(m_oauthBtn, &QPushButton::clicked, this, &SettingsDialog::startGoogleOAuth);
    oauthLayout->addWidget(m_oauthBtn);
    mainLayout->addWidget(oauthBox);

    // API Key Input
    auto *keyLayout = new QVBoxLayout;
    keyLayout->setSpacing(6);
    auto *keyLabel = new QLabel("API Key");
    keyLabel->setStyleSheet(labelStyle);
    keyLayout->addWidget(keyLabel);

    m_apiKeyEdit = new QLineEdit;
    m_apiKeyEdit->setEchoMode(QLineEdit::Password);
    m_apiKeyEdit->setPlaceholderText("Enter API key...");
    m_apiKeyEdit->setStyleSheet(inputStyle);
    keyLayout->addWidget(m_apiKeyEdit);

    m_keyHelpLabel = new QLabel;
    m_keyHelpLabel->setOpenExternalLinks(true);
    m_keyHelpLabel->setStyleSheet("color: #888888; font-size: 12px;");
    keyLayout->addWidget(m_keyHelpLabel);
    mainLayout->addLayout(keyLayout);

    // Model Selector + Autodetect Button
    auto *modelLayout = new QVBoxLayout;
    modelLayout->setSpacing(6);
    auto *modelHeader = new QHBoxLayout;
    auto *modelLabel = new QLabel("Model");
    modelLabel->setStyleSheet(labelStyle);
    modelHeader->addWidget(modelLabel);
    modelHeader->addStretch();

    m_detectModelsBtn = new QPushButton("⚡ Autodetect Models");
    m_detectModelsBtn->setCursor(Qt::PointingHandCursor);
    m_detectModelsBtn->setFixedHeight(24);
    m_detectModelsBtn->setStyleSheet(
        "QPushButton { background: #3b82f6; color: #ffffff; border: none; border-radius: 4px; font-size: 11px; font-weight: 600; padding: 0 8px; }"
        "QPushButton:hover { background: #2563eb; }"
        "QPushButton:disabled { background: #555555; color: #888888; }");
    connect(m_detectModelsBtn, &QPushButton::clicked, this, &SettingsDialog::autodetectModels);
    modelHeader->addWidget(m_detectModelsBtn);
    modelLayout->addLayout(modelHeader);

    m_modelCombo = new QComboBox;
    m_modelCombo->setEditable(true);
    m_modelCombo->setStyleSheet(inputStyle);
    modelLayout->addWidget(m_modelCombo);
    mainLayout->addLayout(modelLayout);

    // Custom Base URL
    auto *urlLayout = new QVBoxLayout;
    urlLayout->setSpacing(6);
    auto *urlLabel = new QLabel("Base URL (optional override)");
    urlLabel->setStyleSheet(labelStyle);
    urlLayout->addWidget(urlLabel);

    m_baseUrlEdit = new QLineEdit;
    m_baseUrlEdit->setPlaceholderText("https://...");
    m_baseUrlEdit->setStyleSheet(inputStyle);
    urlLayout->addWidget(m_baseUrlEdit);
    mainLayout->addLayout(urlLayout);

    // Status message
    m_statusLabel = new QLabel;
    m_statusLabel->setStyleSheet("color: #10aa00; font-size: 12px; font-weight: bold;");
    m_statusLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(m_statusLabel);

    mainLayout->addStretch();

    // Bottom action buttons
    auto *btnLayout = new QHBoxLayout;
    btnLayout->setSpacing(12);

    auto *cancelBtn = new QPushButton("Cancel");
    cancelBtn->setFixedHeight(36);
    cancelBtn->setCursor(Qt::PointingHandCursor);
    cancelBtn->setStyleSheet(
        "QPushButton { background: #444444; color: #ffffff; border: none; border-radius: 8px; font-size: 14px; font-weight: 500; padding: 0 16px; }"
        "QPushButton:hover { background: #555555; }");
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    btnLayout->addWidget(cancelBtn);

    auto *saveBtn = new QPushButton("Save Settings");
    saveBtn->setFixedHeight(36);
    saveBtn->setCursor(Qt::PointingHandCursor);
    saveBtn->setStyleSheet(
        "QPushButton { background: #188610; color: #ffffff; border: none; border-radius: 8px; font-size: 14px; font-weight: bold; padding: 0 20px; }"
        "QPushButton:hover { background: #1fb314; }");
    connect(saveBtn, &QPushButton::clicked, this, &SettingsDialog::save);
    btnLayout->addWidget(saveBtn);

    mainLayout->addLayout(btnLayout);

    // Set initial values from saved settings
    const QString currentProvider = SettingsManager::instance().provider();
    int idx = m_providerCombo->findData(currentProvider);
    if (idx >= 0) m_providerCombo->setCurrentIndex(idx);
    loadProvider(currentProvider);

    connect(m_providerCombo, &QComboBox::currentIndexChanged, this, [this](int index) {
        if (index >= 0) {
            loadProvider(m_providerCombo->itemData(index).toString());
        }
    });

    updateOAuthStatus();
}

void SettingsDialog::updateOAuthStatus() {
    if (AntigravityAuth::hasToken()) {
        const QString em = AntigravityAuth::email();
        m_oauthStatusLabel->setText(em.isEmpty() ? "Logged in via Google OAuth" : QString("Logged in as %1").arg(em));
        m_oauthStatusLabel->setStyleSheet("color: #10b981; font-size: 12px; font-weight: 500;");
        m_oauthBtn->setText("Disconnect");
        m_oauthBtn->setStyleSheet(
            "QPushButton { background: #dc2626; color: #ffffff; border: none; border-radius: 6px; font-size: 13px; font-weight: 600; padding: 0 14px; }"
            "QPushButton:hover { background: #b91c1c; }");
    } else {
        m_oauthStatusLabel->setText("Not logged in");
        m_oauthStatusLabel->setStyleSheet("color: #888888; font-size: 12px;");
        m_oauthBtn->setText("Login with Google");
        m_oauthBtn->setStyleSheet(
            "QPushButton { background: #2563eb; color: #ffffff; border: none; border-radius: 6px; font-size: 13px; font-weight: 600; padding: 0 14px; }"
            "QPushButton:hover { background: #1d4ed8; }");
    }
}

void SettingsDialog::startGoogleOAuth() {
    if (AntigravityAuth::hasToken()) {
        AntigravityAuth::clearToken();
        updateOAuthStatus();
        m_statusLabel->setText("Google Antigravity account disconnected.");
        m_statusLabel->setStyleSheet("color: #eab308; font-size: 12px;");
        return;
    }

    m_oauthBtn->setEnabled(false);
    m_oauthBtn->setText("Waiting in browser...");
    m_statusLabel->setText("Browser opened. Sign in with Google to authorize...");
    m_statusLabel->setStyleSheet("color: #38bdf8; font-size: 12px;");

    AntigravityAuth::instance().startAuthFlow([this](const GoogleTokenInfo &info) {
        m_oauthBtn->setEnabled(true);
        if (info.valid) {
            updateOAuthStatus();
            m_statusLabel->setText("Google Antigravity authorization successful!");
            m_statusLabel->setStyleSheet("color: #10b981; font-size: 12px; font-weight: bold;");
        } else {
            updateOAuthStatus();
            m_statusLabel->setText(QString("OAuth failed: %1").arg(info.error));
            m_statusLabel->setStyleSheet("color: #ef4444; font-size: 12px; font-weight: bold;");
        }
    });
}

void SettingsDialog::autodetectModels() {
    const QString pId = m_providerCombo->currentData().toString();
    auto &sm = SettingsManager::instance();
    // Temporarily save entered key/url so detection uses what is in inputs
    sm.setApiKey(m_apiKeyEdit->text().trimmed(), pId);
    sm.setCustomBaseUrl(m_baseUrlEdit->text().trimmed(), pId);

    m_detectModelsBtn->setEnabled(false);
    m_detectModelsBtn->setText("Detecting...");
    m_statusLabel->setText("Querying available models from provider...");
    m_statusLabel->setStyleSheet("color: #38bdf8; font-size: 12px;");

    m_ai.fetchAvailableModels(pId, [this, pId](bool success, const QStringList &models, const QString &err) {
        m_detectModelsBtn->setEnabled(true);
        m_detectModelsBtn->setText("⚡ Autodetect Models");

        if (!success || models.isEmpty()) {
            m_statusLabel->setText(QString("Autodetect failed: %1").arg(err.isEmpty() ? "No models returned" : err));
            m_statusLabel->setStyleSheet("color: #ef4444; font-size: 12px;");
            return;
        }

        const QString currentSel = m_modelCombo->currentText();
        m_modelCombo->clear();
        for (const QString &m : models) {
            m_modelCombo->addItem(m);
        }
        int found = m_modelCombo->findText(currentSel);
        if (found >= 0) {
            m_modelCombo->setCurrentIndex(found);
        } else if (!models.isEmpty()) {
            m_modelCombo->setCurrentIndex(0);
        }

        m_statusLabel->setText(QString("✓ Detected %1 available models!").arg(models.size()));
        m_statusLabel->setStyleSheet("color: #10b981; font-size: 12px; font-weight: bold;");
    });
}

void SettingsDialog::loadProvider(const QString &providerId) {
    const auto cfg = SettingsManager::providerConfig(providerId);
    auto &sm = SettingsManager::instance();

    m_apiKeyEdit->setText(sm.apiKey(providerId));
    m_baseUrlEdit->setText(sm.rawCustomBaseUrl(providerId));
    m_baseUrlEdit->setPlaceholderText(cfg.defaultBaseUrl);

    m_modelCombo->clear();
    for (const auto &m : cfg.models) {
        m_modelCombo->addItem(m);
    }
    const QString savedModel = sm.model(providerId);
    int mIdx = m_modelCombo->findText(savedModel);
    if (mIdx >= 0) {
        m_modelCombo->setCurrentIndex(mIdx);
    } else {
        m_modelCombo->setCurrentText(savedModel);
    }

    if (!cfg.docsUrl.isEmpty()) {
        m_keyHelpLabel->setText(QString("Get key: <a style='color:#10aa00;text-decoration:none;' href='%1'>%1</a>")
                                    .arg(cfg.docsUrl));
    } else {
        m_keyHelpLabel->clear();
    }

    updateOAuthStatus();
}

void SettingsDialog::save() {
    auto &sm = SettingsManager::instance();
    const QString pId = m_providerCombo->currentData().toString();
    sm.setProvider(pId);
    sm.setApiKey(m_apiKeyEdit->text().trimmed(), pId);
    sm.setModel(m_modelCombo->currentText().trimmed(), pId);
    sm.setCustomBaseUrl(m_baseUrlEdit->text().trimmed(), pId);

    if (onSettingsSaved) onSettingsSaved();
    accept();
}

void SettingsDialog::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QPainterPath path;
    path.addRoundedRect(rect(), 14, 14);
    p.fillPath(path, QColor(36, 36, 36));

    QPen pen(QColor(55, 55, 55), 1.5);
    p.setPen(pen);
    p.drawPath(path);
}

}  // namespace mcw

