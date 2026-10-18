#pragma once

#include <QPainter>
#include <QString>
#include <QWidget>

namespace mcw {

// Renders "M-COWORK" as a crisp, stylish dot-matrix / dotted-pattern wordmark.
// Pure vector dot-grid drawing supporting light and dark themes.
class DotMatrixWordmark : public QWidget {
    Q_OBJECT
public:
    explicit DotMatrixWordmark(QWidget *parent = nullptr);

    void setBaseColor(const QColor &color);
    void setDotScale(float scale);

protected:
    void paintEvent(QPaintEvent *) override;

private:
    QColor m_color = QColor(16, 185, 129);
    float m_dotScale = 1.0f;
};

}  // namespace mcw
