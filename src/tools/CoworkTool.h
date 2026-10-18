#pragma once

#include <QByteArray>
#include <QJsonArray>
#include <QJsonObject>
#include <QMap>
#include <QProcess>
#include <QString>
#include <functional>

namespace mcw {

// Wraps tools/matricX-cowork/matricX-cowork.exe (JSON-lines over stdin/stdout) so the AI can
// control the computer. The process starts lazily on the first call.
class CoworkTool {
public:
    using Callback = std::function<void(const QJsonObject &reply)>;

    CoworkTool();
    ~CoworkTool();

    bool available() const;
    QJsonArray definitions();  // AI tool schemas (runs `--describe` once, cached)
    void call(const QString &tool, const QJsonObject &args, Callback cb);
    void endSession();
    bool sessionActive() const;

private:
    bool ensureStarted();
    void onOutput();
    void failPending(const QString &why);

    QProcess *m_proc = nullptr;
    QByteArray m_buf;
    QMap<qint64, Callback> m_pending;
    qint64 m_nextId = 1;
    QJsonArray m_defs;
};

}  // namespace mcw
