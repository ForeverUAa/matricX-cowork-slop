#pragma once

#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QWidget>

namespace mcw {

// Keyboard accessible model selector with search and direct in-chat switching
class ModelSelector : public QWidget {
    Q_OBJECT
public:
    explicit ModelSelector(QWidget *parent = nullptr);

    void refreshModels();
    QString currentModel() const;
    QString currentProvider() const;
    void setSelectedModel(const QString &providerId, const QString &model);

signals:
    void modelChanged(const QString &providerId, const QString &model);

private:
    void onSelectionChanged(int index);

    QLabel *m_iconLabel = nullptr;
    QComboBox *m_combo = nullptr;
    bool m_updating = false;
};

}  // namespace mcw
