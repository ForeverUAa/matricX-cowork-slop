#pragma once

#include <QDialog>
#include <functional>

#include "ai/AiClient.h"

class QComboBox;
class QLineEdit;
class QLabel;
class QPushButton;
class QTabWidget;
class QVBoxLayout;
class QScrollArea;

namespace mcw {

class SettingsDialog : public QDialog {
    Q_OBJECT
public:
    explicit SettingsDialog(QWidget *parent = nullptr);

    std::function<void()> onSettingsSaved;

protected:
    void paintEvent(QPaintEvent *) override;

private:
    void setupAppearanceTab(QWidget *tab);
    void setupProvidersTab(QWidget *tab);
    void setupArchivedTab(QWidget *tab);

    void refreshProviderCards();
    void refreshArchivedSessions();
    void autodetectModels(const QString &providerId);
    void startGoogleOAuth();
    void updateOAuthStatus();

    QTabWidget *m_tabs = nullptr;

    // Appearance tab
    QComboBox *m_themeCombo = nullptr;

    // Providers tab
    QVBoxLayout *m_providerCardsLayout = nullptr;
    QLabel *m_statusLabel = nullptr;

    // Archived sessions tab
    QVBoxLayout *m_archivedListLayout = nullptr;
    QLabel *m_emptyArchivedLabel = nullptr;

    // OAuth controls
    QPushButton *m_oauthBtn = nullptr;
    QLabel *m_oauthStatusLabel = nullptr;

    AiClient m_ai;
};

}  // namespace mcw
