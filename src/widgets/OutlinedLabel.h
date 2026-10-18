#pragma once

#include <QFont>
#include <QString>
#include <QWidget>

namespace mcw {

// Bold white text with a black outline (used for the "Chats" heading).
class OutlinedLabel : public QWidget {
public:
    OutlinedLabel(const QString &text, int pixelSize, QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *) override;

private:
    static constexpr int kPadX = 14;
    QString m_text;
    QFont m_font;
};

}  // namespace mcw
