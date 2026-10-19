#pragma once

#include <QByteArray>
#include <QJsonArray>
#include <QJsonObject>
#include <QObject>
#include <QString>
#include <functional>

class QNetworkAccessManager;
class QNetworkReply;

namespace mcw {

struct ChatMessage {
    QString role;      // "user", "assistant", or "model"
    QString content;
    QString imagePath; // Optional path to image attachment (e.g. from screenshot tool)
};

struct ToolCallRequest {
    QString id;
    QString name;
    QJsonObject arguments;
};

struct AiResponse {
    bool success = false;
    QString text;
    QList<ToolCallRequest> toolCalls;
    QString errorMessage;
};

class AiClient {
public:
    using ResponseCallback = std::function<void(const AiResponse &response)>;
    using ModelsCallback = std::function<void(bool success, const QStringList &models, const QString &error)>;

    AiClient();
    ~AiClient();

    // Sends conversation history + tool definitions to configured provider
    void sendMessage(const QList<ChatMessage> &history,
                     const QJsonArray &toolDefinitions,
                     ResponseCallback callback);

    // Queries provider endpoint to fetch available models for current API key
    void fetchAvailableModels(const QString &providerId, ModelsCallback callback);

private:
    void sendGemini(const QList<ChatMessage> &history,
                    const QJsonArray &toolDefinitions,
                    ResponseCallback callback);

    void sendOpenAiCompatible(const QList<ChatMessage> &history,
                              const QJsonArray &toolDefinitions,
                              ResponseCallback callback);

    void sendAnthropic(const QList<ChatMessage> &history,
                       const QJsonArray &toolDefinitions,
                       ResponseCallback callback);

    QNetworkAccessManager *m_nam = nullptr;
};

}  // namespace mcw

