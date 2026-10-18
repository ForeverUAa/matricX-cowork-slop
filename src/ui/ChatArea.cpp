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

    // window controls (top-right)
    auto *top = new QHBoxLayout;
    top->setContentsMargins(0, 12, 15, 0);
    top->setSpacing(0);
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
        "QScrollBar::handle:vertical{background:#2f6b27;border-radius:4px;min-height:30px;}"
        "QScrollBar::add-line:vertical,QScrollBar::sub-line:vertical{height:0;}");
    m_scroll->viewport()->setAutoFillBackground(false);
    auto *content = new QWidget;
    content->setObjectName("msgContent");
    content->setStyleSheet("#msgContent{background:transparent;}");
    m_msgLayout = new QVBoxLayout(content);
    m_msgLayout->setContentsMargins(60, 10, 60, 10);
    m_msgLayout->setSpacing(10);
    m_msgLayout->addStretch();
    m_scroll->setWidget(content);
    root->addWidget(m_scroll, 1);

    // input bar
    auto *barRow = new QHBoxLayout;
    barRow->setContentsMargins(60, 8, 60, 25);
    auto *bar = new QFrame;
    bar->setObjectName("bar");
    bar->setFixedHeight(66);
    bar->setStyleSheet(QString("QFrame#bar{background:%1;border:5px solid %2;border-radius:14px;}")
                           .arg(theme::kInputInner.name(), theme::kInputBorder.name()));
    auto *barLayout = new QHBoxLayout(bar);
    barLayout->setContentsMargins(10, 0, 10, 0);
    barLayout->setSpacing(8);

    m_input = new QLineEdit;
    m_input->setPlaceholderText("Type your message here...");
    m_input->setStyleSheet(QString("QLineEdit{background:transparent;border:none;color:%1;"
                                   "font-size:21px;padding-left:12px;}")
                               .arg(theme::kInputText.name()));
    QPalette pal = m_input->palette();
    pal.setColor(QPalette::PlaceholderText, theme::kInputPlaceholder);
    m_input->setPalette(pal);
    barLayout->addWidget(m_input, 1);

    auto *divider = new QFrame;
    divider->setFixedSize(6, 36);
    divider->setStyleSheet(
        QString("background:%1;border:none;border-radius:3px;").arg(theme::kInputDivider.name()));
    barLayout->addWidget(divider);

    auto *mic = new IconButton(assets::path("icons/mic.svg"), assets::path("icons/mic-hover.svg"),
                               QSize(46, 46), QSize(46, 46), QColor(0, 0, 0, 0));
    barLayout->addWidget(mic);

    barRow->addWidget(bar);
    root->addLayout(barRow);

    connect(m_input, &QLineEdit::returnPressed, this, [this] { send(); });
}

void ChatArea::focusInput() { m_input->setFocus(); }

void ChatArea::setCornerRadius(int radius) {
    m_radius = radius;
    update();
}

void ChatArea::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // rounded on the right side only (the sidebar owns the left corners)
    QPainterPath clip;
    clip.addRoundedRect(QRectF(-m_radius, 0, width() + m_radius, height()), m_radius, m_radius);
    p.setClipPath(clip);
    p.fillRect(rect(), Qt::black);

    const double gy = height() * 0.76;
    QLinearGradient g(0, gy, 0, height());
    g.setColorAt(0, QColor(0, 0, 0));
    g.setColorAt(1, theme::kGradientBottom);
    p.fillRect(QRectF(0, gy, width(), height() - gy + 1), QBrush(g));

    if (!m_hasMessages) {
        QFont f;
        f.setFamilies({"Rockwell", "Rockwell Extra Bold", "Georgia"});
        f.setBold(true);
        f.setPixelSize(std::max(40, int(width() * 0.1)));
        QPainterPath path;
        path.addText(0, 0, f, "M-Cowork");
        const QRectF br = path.boundingRect();
        path.translate(width() * 0.5 - br.center().x(), height() * 0.33 - br.center().y());
        p.setPen(QPen(theme::kTitleOutline, 6, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));
        p.setBrush(theme::kTitleFill);
        p.drawPath(path);
    }
}

void ChatArea::addMessage(const QString &text, bool fromUser) {
    auto *lbl = new QLabel(text);
    lbl->setWordWrap(true);
    lbl->setMaximumWidth(560);
    lbl->setTextInteractionFlags(Qt::TextSelectableByMouse);
    lbl->setStyleSheet(
        fromUser ? QString("background:%1;color:#ffffff;border:1px solid %2;border-radius:12px;"
                           "padding:10px 14px;font-size:16px;")
                       .arg(theme::kUserBubbleBg.name(), theme::kUserBubbleBorder.name())
                 : QString("background:%1;color:%2;border-radius:12px;padding:10px 14px;font-size:16px;")
                       .arg(theme::kBotBubbleBg.name(), theme::kBotBubbleText.name()));
    auto *row = new QHBoxLayout;
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
    m_history.append({"user", t});

    runAgentLoop();
}

void ChatArea::runAgentLoop() {
    m_busy = true;
    m_input->setEnabled(false);

    QJsonArray tools = m_tool.definitions();

    m_ai.sendMessage(m_history, tools, [this](const AiResponse &resp) {
        if (!resp.success) {
            m_busy = false;
            m_input->setEnabled(true);
            m_input->setFocus();
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
    addMessage(QString("⚙ Running tool: %1").arg(tc.name), false);

    m_tool.call(tc.name, tc.arguments, [this, calls, index, tc](const QJsonObject &reply) {
        QString toolResultStr = QString::fromUtf8(QJsonDocument(reply).toJson(QJsonDocument::Compact));
        m_history.append({"user", QString("Tool result for %1: %2").arg(tc.name, toolResultStr)});
        executeNextToolCall(calls, index + 1);
    });
}

}  // namespace mcw
