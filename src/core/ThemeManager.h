#pragma once

#include <QColor>
#include <QObject>
#include <QString>

namespace mcw {

enum class ThemeMode {
    Dark,
    Light,
    System
};

struct ThemeColors {
    // Window & background
    QColor windowBg;
    QColor sidebarBg;
    QColor chatBg;
    QColor panelBg;
    QColor surfaceBg;
    QColor cardBg;
    QColor cardBorder;

    // Text & labels
    QColor textPrimary;
    QColor textSecondary;
    QColor textMuted;

    // Borders & dividers
    QColor borderSubtle;
    QColor borderFocus;
    QColor divider;

    // Accents (Derived from prototype #10b981 / emerald)
    QColor accent;
    QColor accentHover;
    QColor accentMuted;
    QColor accentGlow;

    // Message bubbles
    QColor userBubbleBg;
    QColor userBubbleBorder;
    QColor userBubbleText;
    QColor botBubbleBg;
    QColor botBubbleBorder;
    QColor botBubbleText;

    // Window controls
    QColor winClose;
    QColor winMin;
    QColor winMax;
};

class ThemeManager : public QObject {
    Q_OBJECT
public:
    static ThemeManager &instance();

    ThemeMode mode() const;
    void setMode(ThemeMode mode);

    bool isDark() const;
    const ThemeColors &colors() const;

    // Helpers for stylesheets
    QString qssVariables() const;

signals:
    void themeChanged();

private:
    ThemeManager();
    void updateEffectiveTheme();

    ThemeMode m_mode = ThemeMode::Dark;
    bool m_effectiveDark = true;
    ThemeColors m_colors;
};

}  // namespace mcw
