#include "ui/ChatArea.h"

#include <QFont>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QLinearGradient>
#include <QPainter>
#include <QPainterPath>
#include <QPalette>
#include <QScrollArea>
#include <QScrollBar>
#include <QTimer>
#include <QVBoxLayout>

#include <algorithm>

#include "core/Assets.h"
#include "widgets/IconButton.h"
#include "widgets/WindowControls.h"

namespace mcw {

ChatArea::ChatArea(QWidget *parent) : QWidget(parent) {
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // window controls & status indicator (top)
    auto *top = new QHBoxLayout;
    top->setContentsMargins(24, 14, 18, 0);
    top->setSpacing(12);

    m_statusPill = new QLabel(this);
    m_statusPill->setFixedHeight(26);
    m_statusPill->setStyleSheet(
        "background: rgba(30, 41, 59, 0.7); border: 1px solid rgba(56, 189, 248, 0.3);"
        "border-radius: 13px; color: #38bdf8; font-size: 12px; font-weight: 600; padding: 0 12px;");
    m_statusPill->hide();
    top->addWidget(m_statusPill, 0, Qt::AlignVCenter);

    top->addStretch();
    m_controls = new WindowControls;
    top->addWidget(m_controls, 0, Qt::AlignTop);
    root->addLayout(top);

    // messages
    m_scroll = new QScrollArea;
    m_scroll->setWidgetResizable(true);
    m_scroll->setFrameShape(QFrame::NoFrame);
    m_scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scroll->setStyleSheet(
        "QScrollArea{background:transparent;border:none;}"
        "QScrollBar:vertical{background:transparent;width:8px;margin:2px;}"
        "QScrollBar::handle:vertical{background:rgba(34, 197, 94, 0.4);border-radius:4px;min-height:30px;}"
        "QScrollBar::handle:vertical:hover{background:rgba(34, 197, 94, 0.7);}"
        "QScrollBar::add-line:vertical,QScrollBar::sub-line:vertical{height:0;}");
    m_scroll->viewport()->setAutoFillBackground(false);
    auto *content = new QWidget;
    content->setObjectName("msgContent");
    content->setStyleSheet("#msgContent{background:transparent;}");
    m_msgLayout = new QVBoxLayout(content);
    m_msgLayout->setContentsMargins(60, 10, 60, 10);
    m_msgLayout->setSpacing(12);
    m_msgLayout->addStretch();
    m_scroll->setWidget(content);
    root->addWidget(m_scroll, 1);

    // input bar
    auto *barRow = new QHBoxLayout;
    barRow->setContentsMargins(60, 8, 60, 25);
    m_inputBar = new QFrame;
    m_inputBar->setObjectName("bar");
    m_inputBar->setFixedHeight(66);
    m_inputBar->setStyleSheet(
        "QFrame#bar {"
        "  background: #242424;"
        "  border: 2px solid #383838;"
        "  border-radius: 14px;"
        "  transition: border 0.3s ease;"
        "}"
        "QFrame#bar:focus-within {"
        "  border: 2px solid #10b981;"
        "  background: #282828;"
        "}");
    auto *barLayout = new QHBoxLayout(m_inputBar);
    barLayout->setContentsMargins(12, 0, 12, 0);
    barLayout->setSpacing(10);

    m_input = new QLineEdit;
    m_input->setPlaceholderText("Ask M-Cowork anything, or type /tools...");
    m_input->setStyleSheet(
        "QLineEdit {"
        "  background: transparent;"
        "  border: none;"
        "  color: #f1f5f9;"
        "  font-size: 17px;"
        "  padding-left: 8px;"
        "}");
    QPalette pal = m_input->palette();
    pal.setColor(QPalette::PlaceholderText, QColor(140, 140, 150));
    m_input->setPalette(pal);
    barLayout->addWidget(m_input, 1);

    auto *divider = new QFrame;
    divider->setFixedSize(2, 28);
    divider->setStyleSheet("background: #3e3e3e; border: none;");
    barLayout->addWidget(divider);

    auto *mic = new IconButton(assets::path("icons/mic.svg"), assets::path("icons/mic-hover.svg"),
                               QSize(44, 44), QSize(36, 36), QColor(0, 0, 0, 0));
    barLayout->addWidget(mic);

    barRow->addWidget(m_inputBar);
    root->addLayout(barRow);

    connect(m_input, &QLineEdit::returnPressed, this, [this] { send(); });

    // Background pulse timer (~30 FPS for gentle glow animation)
    connect(&m_animTimer, &QTimer::timeout, this, [this] {
        m_pulsePhase += 0.04f;
        if (m_pulsePhase > 6.28318f) m_pulsePhase -= 6.28318f;
        if (!m_hasMessages || m_busy) {
            update();
        }
    });
    m_animTimer.start(33);
}

void ChatArea::focusInput() { m_input->setFocus(); }

void ChatArea::setCornerRadius(int radius) {
    m_radius = radius;
    update();
}

void ChatArea::updateStatusIndicator(const QString &text, bool active) {
    if (text.isEmpty()) {
        m_statusPill->hide();
    } else {
        m_statusPill->setText(text);
        if (active) {
            m_statusPill->setStyleSheet(
                "background: rgba(16, 185, 129, 0.15); border: 1px solid rgba(16, 185, 129, 0.5);"
                "border-radius: 13px; color: #34d399; font-size: 12px; font-weight: 600; padding: 0 12px;");
        } else {
            m_statusPill->setStyleSheet(
                "background: rgba(30, 41, 59, 0.7); border: 1px solid rgba(56, 189, 248, 0.3);"
                "border-radius: 13px; color: #38bdf8; font-size: 12px; font-weight: 600; padding: 0 12px;");
        }
        m_statusPill->show();
    }
}

void ChatArea::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // rounded on the right side only (the sidebar owns the left corners)
    QPainterPath clip;
    clip.addRoundedRect(QRectF(-m_radius, 0, width() + m_radius, height()), m_radius, m_radius);
    p.setClipPath(clip);
    p.fillRect(rect(), QColor(14, 14, 15));

    // Smooth ambient bottom gradient with gentle sine pulsing
    const float pulse = 0.85f + 0.15f * std::sin(m_pulsePhase);
    const double gy = height() * 0.70;
    QLinearGradient g(0, gy, 0, height());
    g.setColorAt(0, QColor(14, 14, 15, 0));
    g.setColorAt(0.6, QColor(8, 45, 15, int(80 * pulse)));
    g.setColorAt(1, QColor(16, 150, 40, int(130 * pulse)));
    p.fillRect(QRectF(0, gy, width(), height() - gy + 1), QBrush(g));

    if (!m_hasMessages) {
        QFont f;
        f.setFamilies({"Rockwell", "Rockwell Extra Bold", "Segoe UI", "Georgia"});
        f.setBold(true);
        f.setPixelSize(std::max(38, int(width() * 0.085)));
        QPainterPath path;
        path.addText(0, 0, f, "M-Cowork");
        const QRectF br = path.boundingRect();
        path.translate(width() * 0.5 - br.center().x(), height() * 0.34 - br.center().y());
        
        // Layered glow effect behind title
        QPen outlinePen(QColor(16, 185, 129, int(180 * pulse)), 5, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin);
        p.setPen(outlinePen);
        p.setBrush(QColor(24, 24, 26));
        p.drawPath(path);

        // Subtitle slogan
        QFont subFont("Segoe UI", 13);
        p.setFont(subFont);
        p.setPen(QColor(140, 140, 150));
        const QString sub = "Autonomous Desktop Operator & AI Co-worker";
        p.drawText(QRectF(0, height() * 0.34 + br.height() * 0.5 + 16, width(), 30), Qt::AlignCenter, sub);
    }
}

void ChatArea::addMessage(const QString &text, bool fromUser) {
    auto *lbl = new QLabel(text);
    lbl->setWordWrap(true);
    lbl->setMaximumWidth(580);
    lbl->setTextInteractionFlags(Qt::TextSelectableByMouse);
    lbl->setStyleSheet(
        fromUser ? "background: #064e3b; color: #ecfdf5; border: 1px solid #059669; border-radius: 12px;"
                   "padding: 11px 16px; font-size: 15px; font-weight: 500; line-height: 1.4;"
                 : "background: #1f2937; color: #f3f4f6; border: 1px solid #374151; border-radius: 12px;"
                   "padding: 11px 16px; font-size: 15px; line-height: 1.4;");
    auto *row = new QHBoxLayout;
    row->setContentsMargins(0, 0, 0, 0);
    if (fromUser) row->addStretch();
    row->addWidget(lbl);
    if (!fromUser) row->addStretch();
    m_msgLayout->insertLayout(m_msgLayout->count() - 1, row);
    QTimer::singleShot(0, this, [this] {
        m_scroll->verticalScrollBar()->setValue(m_scroll->verticalScrollBar()->maximum());
    });
}

void ChatArea::send() {
    const QString t = m_input->text().trimmed();
    if (t.isEmpty() || m_busy) return;
    m_input->clear();
    m_hasMessages = true;
    update();

    addMessage(t, true);

    if (t == "/help") {
        const QString help = "Available commands:\n"
                             "• /tools — List available computer control tools\n"
                             "• /tool <name> [json] — Execute a tool (e.g. /tool screen_info or /tool mouse_move {\"x\":500,\"y\":300})\n"
                             "• /tool end_session — End active tool session\n"
                             "• /help — Show this help message";
        addMessage(help, false);
        return;
    }

    if (t == "/tools") {
        if (!m_tool.available()) {
            addMessage("Error: matricX-cowork tool binary not found. Build it first with scripts/build.ps1 or build.sh.", false);
            return;
        }
        const QJsonArray defs = m_tool.definitions();
        if (defs.isEmpty()) {
            addMessage("No tool definitions returned from matricX-cowork --describe.", false);
            return;
        }
        QString out = QString("Available Tools (%1):\n").arg(defs.size());
        for (const auto &val : defs) {
            const QJsonObject obj = val.toObject();
            out += QString("• %1: %2\n").arg(obj["name"].toString(), obj["description"].toString());
        }
        addMessage(out.trimmed(), false);
        return;
    }

    if (t.startsWith("/tool ") || t == "/tool") {
        if (!m_tool.available()) {
            addMessage("Error: matricX-cowork tool binary not found. Build it first.", false);
            return;
        }
        const QString rest = t.mid(5).trimmed();
        if (rest.isEmpty()) {
            addMessage("Usage: /tool <name> [{\"key\": value, ...}]\nExample: /tool mouse_move {\"x\":500,\"y\":300}", false);
            return;
        }
        const int spaceIdx = rest.indexOf(' ');
        const QString toolName = (spaceIdx == -1) ? rest : rest.left(spaceIdx).trimmed();
        const QString jsonStr = (spaceIdx == -1) ? QString() : rest.mid(spaceIdx + 1).trimmed();

        QJsonObject args;
        if (!jsonStr.isEmpty()) {
            QJsonParseError parseErr;
            const QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8(), &parseErr);
            if (parseErr.error != QJsonParseError::NoError || !doc.isObject()) {
                addMessage(QString("JSON parse error: %1").arg(parseErr.errorString()), false);
                return;
            }
            args = doc.object();
        }

        m_tool.call(toolName, args, [this, toolName](const QJsonObject &reply) {
            const bool ok = reply["ok"].toBool();
            QString text;
            if (ok) {
                QJsonObject copy = reply;
                copy.remove("id");
                copy.remove("ok");
                if (copy.isEmpty()) {
                    text = QString("✓ %1 completed successfully").arg(toolName);
                } else {
                    text = QString("✓ %1 result:\n%2")
                               .arg(toolName, QString::fromUtf8(QJsonDocument(copy).toJson(QJsonDocument::Indented)).trimmed());
                }
            } else {
                text = QString("✗ %1 failed: %2")
                           .arg(toolName, reply["error"].toString("unknown error"));
            }
            addMessage(text, false);
        });
        return;
    }

    m_history.append({"user", t});
    runAgentLoop();
}

void ChatArea::runAgentLoop() {
    m_busy = true;
    m_input->setEnabled(false);
    updateStatusIndicator("Thinking...", false);

    QJsonArray tools = m_tool.definitions();

    m_ai.sendMessage(m_history, tools, [this](const AiResponse &resp) {
        if (!resp.success) {
            m_busy = false;
            m_input->setEnabled(true);
            m_input->setFocus();
            updateStatusIndicator(QString(), false);
            addMessage(QString("Error: %1").arg(resp.errorMessage), false);
            return;
        }

        if (!resp.text.isEmpty()) {
            addMessage(resp.text, false);
            m_history.append({"assistant", resp.text});
        }

        if (resp.toolCalls.isEmpty()) {
            m_busy = false;
            m_input->setEnabled(true);
            m_input->setFocus();
            updateStatusIndicator(QString(), false);
            return;
        }

        executeNextToolCall(resp.toolCalls, 0);
    });
}

void ChatArea::executeNextToolCall(const QList<ToolCallRequest> &calls, int index) {
    if (index >= calls.size()) {
        runAgentLoop();
        return;
    }

    const auto &tc = calls[index];
    updateStatusIndicator(QString("Executing: %1").arg(tc.name), true);
    addMessage(QString("⚙ Running tool: %1").arg(tc.name), false);

    m_tool.call(tc.name, tc.arguments, [this, calls, index, tc](const QJsonObject &reply) {
        QString toolResultStr = QString::fromUtf8(QJsonDocument(reply).toJson(QJsonDocument::Compact));
        m_history.append({"user", QString("Tool result for %1: %2").arg(tc.name, toolResultStr)});
        executeNextToolCall(calls, index + 1);
    });
}

}  // namespace mcw
