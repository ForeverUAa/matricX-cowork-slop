#pragma once

#include <QDialog>
#include <functional>

class QComboBox;
class QLineEdit;
class QLabel;

namespace mcw {

class SettingsDialog : public QDialog {
public:
    explicit SettingsDialog(QWidget *parent = nullptr);

    std::function<void()> onSettingsSaved;

protected:
    void paintEvent(QPaintEvent *) override;

private:
    void loadProvider(const QString &providerId);
    void save();

    QComboBox *m_providerCombo = nullptr;
    QLineEdit *m_apiKeyEdit = nullptr;
    QComboBox *m_modelCombo = nullptr;
    QLineEdit *m_baseUrlEdit = nullptr;
    QLabel *m_keyHelpLabel = nullptr;
    QLabel *m_statusLabel = nullptr;
};

}  // namespace mcw

