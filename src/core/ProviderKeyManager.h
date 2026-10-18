#pragma once

#include <QDateTime>
#include <QList>
#include <QObject>
#include <QString>

namespace mcw {

struct ApiKeyEntry {
    QString id;
    QString key;
    QString label;
    bool isActive = false;
    qint64 createdAt = 0;
};

// Manages multiple API keys nested under each provider card
class ProviderKeyManager : public QObject {
    Q_OBJECT
public:
    static ProviderKeyManager &instance();

    QList<ApiKeyEntry> keysForProvider(const QString &providerId) const;
    QString activeKey(const QString &providerId) const;

    void addKey(const QString &providerId, const QString &key, const QString &label = QString());
    void removeKey(const QString &providerId, const QString &keyId);
    void setActiveKey(const QString &providerId, const QString &keyId);

    // Helper to mask key: "sk-ant-...4a9b"
    static QString maskKey(const QString &key);

signals:
    void keysChanged(const QString &providerId);

private:
    ProviderKeyManager();
    void load();
    void save();

    // Map: providerId -> list of keys
    QMap<QString, QList<ApiKeyEntry>> m_keys;
};

}  // namespace mcw
