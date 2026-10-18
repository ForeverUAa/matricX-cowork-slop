#pragma once

#include <QDialog>
#include <functional>

#include "ai/AiClient.h"

class QComboBox;
class QLineEdit;
class QLabel;
class QPushButton;

namespace mcw {

class SettingsDialog : public QDialog {
    Q_OBJECT
public:
    explicit SettingsDialog(QWidget *parent = nullptr);

    std::function<void()> onSettingsSaved;

protected:
    void paintEvent(QPaintEvent *) override;

private:
    void loadProvider(const QString &providerId);
    void save();
    void autodetectModels();
    void startGoogleOAuth();
    void updateOAuthStatus();

    QComboBox *m_providerCombo = nullptr;
    QLineEdit *m_apiKeyEdit = nullptr;
    QComboBox *m_modelCombo = nullptr;
    QLineEdit *m_baseUrlEdit = nullptr;
    QLabel *m_keyHelpLabel = nullptr;
    QLabel *m_statusLabel = nullptr;

    // OAuth controls
    QPushButton *m_oauthBtn = nullptr;
    QLabel *m_oauthStatusLabel = nullptr;

    // Autodetect control
    QPushButton *m_detectModelsBtn = nullptr;

    AiClient m_ai;
};

}  // namespace mcw

