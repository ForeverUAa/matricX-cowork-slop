#include "widgets/DotMatrixWordmark.h"

#include <QPainter>
#include <vector>

#include "core/ThemeManager.h"

namespace mcw {

// 5x7 dot-matrix font definition for characters: 'M', '-', 'C', 'O', 'W', 'R', 'K'
// 7 rows of 5 dots (represented as 7 strings of 5 chars, '#' = dot, '.' = space)
struct CharMatrix {
    char ch;
    const char *rows[7];
};

static const CharMatrix kMatrices[] = {
    { 'M', {
        "#...#",
        "##.##",
        "#.#.#",
        "#...#",
        "#...#",
        "#...#",
        "#...#"
    }},
    { '-', {
        ".....",
        ".....",
        ".....",
        "#####",
        ".....",
        ".....",
        "....."
    }},
    { 'C', {
        ".###.",
        "#...#",
        "#....",
        "#....",
        "#....",
        "#...#",
        ".###."
    }},
    { 'O', {
        ".###.",
        "#...#",
        "#...#",
        "#...#",
        "#...#",
        "#...#",
        ".###."
    }},
    { 'W', {
        "#...#",
        "#...#",
        "#...#",
        "#...#",
        "#.#.#",
        "##.##",
        "#...#"
    }},
    { 'R', {
        "####.",
        "#...#",
        "#...#",
        "####.",
        "#..#.",
        "#...#",
        "#...#"
    }},
    { 'K', {
        "#...#",
        "#..#.",
        "#.##.",
        "##...",
        "#.#..",
        "#..#.",
        "#...#"
    }}
};

static const char *kWordmark = "M-COWORK";

DotMatrixWordmark::DotMatrixWordmark(QWidget *parent) : QWidget(parent) {
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    setMinimumHeight(80);
}

void DotMatrixWordmark::setBaseColor(const QColor &color) {
    m_color = color;
    update();
}

void DotMatrixWordmark::setDotScale(float scale) {
    m_dotScale = scale;
    update();
}

void DotMatrixWordmark::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    const bool dark = ThemeManager::instance().isDark();
    const QColor activeColor = dark ? QColor(16, 185, 129) : QColor(5, 150, 105);
    const QColor inactiveColor = dark ? QColor(255, 255, 255, 12) : QColor(0, 0, 0, 10);
    const QColor glowColor = dark ? QColor(16, 185, 129, 60) : QColor(5, 150, 105, 40);

    const int charCount = 8; // "M-COWORK"
    const int colsPerChar = 5;
    const int rows = 7;
    const int charSpacingCols = 2; // spacing between chars in grid units

    const int totalCols = charCount * colsPerChar + (charCount - 1) * charSpacingCols;

    // Calculate dot size to fit widget nicely
    float pitch = std::clamp(float(width()) / (totalCols + 4), 4.0f, 10.0f);
    float dotRadius = pitch * 0.38f;

    float gridWidth = totalCols * pitch;
    float gridHeight = rows * pitch;

    float startX = (width() - gridWidth) / 2.0f;
    float startY = (height() - gridHeight) / 2.0f;

    for (int ci = 0; ci < charCount; ++ci) {
        const char c = kWordmark[ci];
        const CharMatrix *matrix = nullptr;
        for (const auto &m : kMatrices) {
            if (m.ch == c) {
                matrix = &m;
                break;
            }
        }
        if (!matrix) continue;

        float charStartX = startX + ci * (colsPerChar + charSpacingCols) * pitch;

        for (int r = 0; r < rows; ++r) {
            const char *rowStr = matrix->rows[r];
            for (int col = 0; col < colsPerChar; ++col) {
                float cx = charStartX + col * pitch + pitch / 2.0f;
                float cy = startY + r * pitch + pitch / 2.0f;

                const bool isDot = (rowStr[col] == '#');

                if (isDot) {
                    // Soft glow halo
                    p.setPen(Qt::NoPen);
                    p.setBrush(glowColor);
                    p.drawEllipse(QPointF(cx, cy), dotRadius * 1.6f, dotRadius * 1.6f);

                    // Solid crisp dot
                    p.setBrush(activeColor);
                    p.drawEllipse(QPointF(cx, cy), dotRadius, dotRadius);
                } else {
                    // Subtle unlit grid dot
                    p.setPen(Qt::NoPen);
                    p.setBrush(inactiveColor);
                    p.drawEllipse(QPointF(cx, cy), dotRadius * 0.5f, dotRadius * 0.5f);
                }
            }
        }
    }
}

}  // namespace mcw
