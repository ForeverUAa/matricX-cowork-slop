#pragma once

#include <QFrame>
#include <QJsonObject>
#include <QString>

class QLabel;
class QPushButton;
class QVBoxLayout;

namespace mcw {

// Modern Codex-style collapsible action step card for OS control tool executions.
// Displays tool name, arguments, status badge, execution details, and image preview if screenshot.
class ActionStepCard : public QFrame {
    Q_OBJECT
public:
    enum class Status {
        Running,
        Success,
        Failed
    };

    explicit ActionStepCard(const QString &toolName, const QJsonObject &args, QWidget *parent = nullptr);

    void setStatus(Status s, const QString &resultText = QString(), const QString &imagePath = QString());

private:
    void updateTheme();

    QString m_toolName;
    QJsonObject m_args;
    Status m_status = Status::Running;

    QLabel *m_iconLabel = nullptr;
    QLabel *m_titleLabel = nullptr;
    QLabel *m_statusBadge = nullptr;
    QPushButton *m_chevronBtn = nullptr;

    QWidget *m_detailsWidget = nullptr;
    QLabel *m_argsLabel = nullptr;
    QLabel *m_resultLabel = nullptr;
    QLabel *m_imagePreview = nullptr;
};

}  // namespace mcw
