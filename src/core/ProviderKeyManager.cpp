#include "core/ProviderKeyManager.h"

#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>
#include <QUuid>

#include "core/SettingsManager.h"

namespace mcw {

static QString storageFilePath() {
    const QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dir);
    return QDir(dir).filePath("provider_keys.json");
}

ProviderKeyManager::ProviderKeyManager() {
    load();

    // Migrate existing single keys from SettingsManager if not in provider_keys.json
    auto &sm = SettingsManager::instance();
    for (const auto &p : SettingsManager::availableProviders()) {
        if (!m_keys.contains(p.id) || m_keys[p.id].isEmpty()) {
            const QString oldKey = sm.apiKey(p.id);
            if (!oldKey.isEmpty()) {
                addKey(p.id, oldKey, "Default Key");
            }
        }
    }
}

ProviderKeyManager &ProviderKeyManager::instance() {
    static ProviderKeyManager s_inst;
    return s_inst;
}

QList<ApiKeyEntry> ProviderKeyManager::keysForProvider(const QString &providerId) const {
    return m_keys.value(providerId);
}

QString ProviderKeyManager::activeKey(const QString &providerId) const {
    const auto list = m_keys.value(providerId);
    for (const auto &k : list) {
        if (k.isActive) return k.key;
    }
    if (!list.isEmpty()) return list.first().key;
    return QString();
}

void ProviderKeyManager::addKey(const QString &providerId, const QString &key, const QString &label) {
    if (key.trimmed().isEmpty()) return;

    ApiKeyEntry entry;
    entry.id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    entry.key = key.trimmed();
    entry.label = label.isEmpty() ? QString("Key %1").arg(m_keys.value(providerId).size() + 1) : label;
    entry.createdAt = QDateTime::currentSecsSinceEpoch();

    auto &list = m_keys[providerId];
    // If first key, set active
    if (list.isEmpty()) {
        entry.isActive = true;
    }
    list.append(entry);

    // Sync active key to SettingsManager
    if (entry.isActive) {
        SettingsManager::instance().setApiKey(entry.key, providerId);
    }

    save();
    emit keysChanged(providerId);
}

void ProviderKeyManager::removeKey(const QString &providerId, const QString &keyId) {
    if (!m_keys.contains(providerId)) return;
    auto &list = m_keys[providerId];

    bool wasActive = false;
    for (int i = 0; i < list.size(); ++i) {
        if (list[i].id == keyId) {
            wasActive = list[i].isActive;
            list.removeAt(i);
            break;
        }
    }

    if (wasActive && !list.isEmpty()) {
        list[0].isActive = true;
        SettingsManager::instance().setApiKey(list[0].key, providerId);
    } else if (list.isEmpty()) {
        SettingsManager::instance().setApiKey(QString(), providerId);
    }

    save();
    emit keysChanged(providerId);
}

void ProviderKeyManager::setActiveKey(const QString &providerId, const QString &keyId) {
    if (!m_keys.contains(providerId)) return;
    auto &list = m_keys[providerId];

    QString activeStr;
    for (auto &k : list) {
        k.isActive = (k.id == keyId);
        if (k.isActive) activeStr = k.key;
    }

    SettingsManager::instance().setApiKey(activeStr, providerId);
    save();
    emit keysChanged(providerId);
}

QString ProviderKeyManager::maskKey(const QString &key) {
    const QString k = key.trimmed();
    if (k.length() <= 8) return "••••••••";
    const QString prefix = k.left(4);
    const QString suffix = k.right(4);
    return prefix + "••••••••" + suffix;
}

void ProviderKeyManager::load() {
    m_keys.clear();
    QFile file(storageFilePath());
    if (!file.open(QIODevice::ReadOnly)) return;

    const auto doc = QJsonDocument::fromJson(file.readAll());
    const auto root = doc.object();

    for (auto it = root.begin(); it != root.end(); ++it) {
        const QString pId = it.key();
        const auto arr = it.value().toArray();
        QList<ApiKeyEntry> list;
        for (const auto &v : arr) {
            const auto obj = v.toObject();
            ApiKeyEntry e;
            e.id = obj.value("id").toString();
            e.key = obj.value("key").toString();
            e.label = obj.value("label").toString();
            e.isActive = obj.value("isActive").toBool(false);
            e.createdAt = obj.value("createdAt").toVariant().toLongLong();
            if (!e.key.isEmpty()) list.append(e);
        }
        m_keys.insert(pId, list);
    }
}

void ProviderKeyManager::save() {
    QJsonObject root;
    for (auto it = m_keys.begin(); it != m_keys.end(); ++it) {
        QJsonArray arr;
        for (const auto &k : it.value()) {
            QJsonObject obj;
            obj["id"] = k.id;
            obj["key"] = k.key;
            obj["label"] = k.label;
            obj["isActive"] = k.isActive;
            obj["createdAt"] = k.createdAt;
            arr.append(obj);
        }
        root[it.key()] = arr;
    }

    QFile file(storageFilePath());
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(root).toJson(QJsonDocument::Compact));
    }
}

}  // namespace mcw
