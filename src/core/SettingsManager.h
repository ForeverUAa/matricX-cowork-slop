#pragma once

#include <QString>
#include <QStringList>

namespace mcw {

struct ProviderConfig {
    QString id;
    QString name;
    QString defaultBaseUrl;
    QStringList models;
    QString defaultModel;
    QString docsUrl;
};

class SettingsManager {
public:
    static SettingsManager &instance();

    static const QList<ProviderConfig> &availableProviders();
    static ProviderConfig providerConfig(const QString &providerId);

    QString provider() const;
    void setProvider(const QString &provider);

    QString apiKey(const QString &providerId = QString()) const;
    void setApiKey(const QString &key, const QString &providerId = QString());

    QString model(const QString &providerId = QString()) const;
    void setModel(const QString &model, const QString &providerId = QString());

    QString customBaseUrl(const QString &providerId = QString()) const;
    QString rawCustomBaseUrl(const QString &providerId = QString()) const;
    void setCustomBaseUrl(const QString &url, const QString &providerId = QString());

    QString activeBaseUrl() const;

private:
    SettingsManager();
};

}  // namespace mcw

