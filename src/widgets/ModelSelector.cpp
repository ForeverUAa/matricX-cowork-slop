#include "widgets/ModelSelector.h"

#include <QCompleter>

#include "core/Icons.h"
#include "core/SettingsManager.h"
#include "core/ThemeManager.h"

namespace mcw {

ModelSelector::ModelSelector(QWidget *parent) : QWidget(parent) {
    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(4, 2, 4, 2);
    layout->setSpacing(6);

    m_iconLabel = new QLabel(this);
    m_iconLabel->setFixedSize(18, 18);
    layout->addWidget(m_iconLabel);

    m_combo = new QComboBox(this);
    m_combo->setEditable(true);
    m_combo->setInsertPolicy(QComboBox::NoInsert);
    m_combo->setMinimumWidth(160);
    m_combo->setMaximumWidth(230);
    m_combo->setFixedHeight(28);

    // Make lineEdit search-friendly
    if (m_combo->lineEdit()) {
        m_combo->lineEdit()->setPlaceholderText("Search model...");
        m_combo->completer()->setFilterMode(Qt::MatchContains);
        m_combo->completer()->setCaseSensitivity(Qt::CaseInsensitive);
    }

    layout->addWidget(m_combo);

    connect(m_combo, &QComboBox::currentIndexChanged, this, &ModelSelector::onSelectionChanged);
    connect(&ThemeManager::instance(), &ThemeManager::themeChanged, this, &ModelSelector::refreshModels);

    refreshModels();
}

void ModelSelector::refreshModels() {
    m_updating = true;
    const bool dark = ThemeManager::instance().isDark();
    const auto &c = ThemeManager::instance().colors();

    // Use Bot icon for AI model representation
    m_iconLabel->setPixmap(icons::pixmap(icons::IconType::Bot, QSize(16, 16), c.accent));

    m_combo->setStyleSheet(QString(
        "QComboBox {"
        "  background: %1;"
        "  border: 1px solid %2;"
        "  border-radius: 6px;"
        "  color: %3;"
        "  padding: 2px 8px;"
        "  font-size: 12px;"
        "  font-weight: 500;"
        "}"
        "QComboBox:focus {"
        "  border: 1px solid %4;"
        "}"
        "QComboBox::drop-down {"
        "  border: none;"
        "  width: 14px;"
        "}"
        "QComboBox QAbstractItemView {"
        "  background: %1;"
        "  border: 1px solid %2;"
        "  color: %3;"
        "  selection-background-color: %4;"
        "  selection-color: #ffffff;"
        "}"
    ).arg(dark ? "#26262b" : "#f1f5f9",
          dark ? "#3f3f46" : "#cbd5e1",
          dark ? "#f3f4f6" : "#0f172a",
          c.accent.name()));

    const QString currentP = SettingsManager::instance().provider();
    const QString currentM = SettingsManager::instance().model(currentP);

    m_combo->clear();

    // Populate all available models grouped by provider
    for (const auto &p : SettingsManager::availableProviders()) {
        for (const auto &m : p.models) {
            const QString display = QString("%1: %2").arg(p.name, m);
            m_combo->addItem(display, QVariantMap{{"provider", p.id}, {"model", m}});
        }
    }

    setSelectedModel(currentP, currentM);
    m_updating = false;
}

QString ModelSelector::currentModel() const {
    const auto data = m_combo->currentData().toMap();
    return data.value("model").toString();
}

QString ModelSelector::currentProvider() const {
    const auto data = m_combo->currentData().toMap();
    return data.value("provider").toString();
}

void ModelSelector::setSelectedModel(const QString &providerId, const QString &model) {
    for (int i = 0; i < m_combo->count(); ++i) {
        const auto data = m_combo->itemData(i).toMap();
        if (data.value("provider").toString() == providerId && data.value("model").toString() == model) {
            m_updating = true;
            m_combo->setCurrentIndex(i);
            m_updating = false;
            return;
        }
    }
}

void ModelSelector::onSelectionChanged(int index) {
    if (m_updating || index < 0) return;
    const auto data = m_combo->itemData(index).toMap();
    const QString pId = data.value("provider").toString();
    const QString model = data.value("model").toString();
    if (!pId.isEmpty() && !model.isEmpty()) {
        SettingsManager::instance().setProvider(pId);
        SettingsManager::instance().setModel(model, pId);
        emit modelChanged(pId, model);
    }
}

}  // namespace mcw
