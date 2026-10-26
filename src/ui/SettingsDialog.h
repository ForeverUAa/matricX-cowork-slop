#pragma once

#include <QDialog>
#include <functional>

#include "ai/AiClient.h"

class QComboBox;
class QCheckBox;
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
    // Fired live when the window-chrome preferences change, so the frame can re-layout.
    std::function<void()> onWindowChromeChanged;

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
    QComboBox *m_controlsPlaceCombo = nullptr;
    QComboBox *m_controlsPositionCombo = nullptr;
    QComboBox *m_controlsShapeCombo = nullptr;
    QComboBox *m_controlsSizeCombo = nullptr;
    QCheckBox *m_showReasoningCheck = nullptr;

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
