#include "core/ThemeManager.h"

#include <QApplication>
#include <QPalette>
#include <QSettings>

namespace mcw {

ThemeManager::ThemeManager() {
    QSettings s("MCowork", "MCowork");
    const QString saved = s.value("appearance/theme", "dark").toString();
    if (saved == "light") m_mode = ThemeMode::Light;
    else if (saved == "system") m_mode = ThemeMode::System;
    else m_mode = ThemeMode::Dark;

    updateEffectiveTheme();
}

ThemeManager &ThemeManager::instance() {
    static ThemeManager s_inst;
    return s_inst;
}

ThemeMode ThemeManager::mode() const { return m_mode; }

void ThemeManager::setMode(ThemeMode mode) {
    if (m_mode == mode) return;
    m_mode = mode;

    QSettings s("MCowork", "MCowork");
    if (m_mode == ThemeMode::Light) s.setValue("appearance/theme", "light");
    else if (m_mode == ThemeMode::System) s.setValue("appearance/theme", "system");
    else s.setValue("appearance/theme", "dark");

    updateEffectiveTheme();
    emit themeChanged();
}

bool ThemeManager::isDark() const { return m_effectiveDark; }

const ThemeColors &ThemeManager::colors() const { return m_colors; }

void ThemeManager::updateEffectiveTheme() {
    if (m_mode == ThemeMode::System) {
        // Query system window background lightness
        const QPalette pal = QGuiApplication::palette();
        m_effectiveDark = (pal.color(QPalette::Window).lightness() < 128);
    } else {
        m_effectiveDark = (m_mode == ThemeMode::Dark);
    }

    if (m_effectiveDark) {
        // Authentic OpenAI Codex Dark Palette
        m_colors.windowBg = QColor(24, 24, 27);        // #18181b
        m_colors.sidebarBg = QColor(18, 18, 20);       // #121214
        m_colors.chatBg = QColor(24, 24, 27);          // #18181b
        m_colors.panelBg = QColor(33, 33, 36);         // #212124
        m_colors.surfaceBg = QColor(39, 39, 42);       // #27272a
        m_colors.cardBg = QColor(33, 33, 36);          // #212124
        m_colors.cardBorder = QColor(46, 46, 51);      // #2e2e33

        m_colors.textPrimary = QColor(244, 244, 245);
        m_colors.textSecondary = QColor(161, 161, 170);
        m_colors.textMuted = QColor(113, 113, 122);

        m_colors.borderSubtle = QColor(39, 39, 42);
        m_colors.borderFocus = QColor(16, 185, 129);   // #10b981
        m_colors.divider = QColor(39, 39, 42);

        m_colors.accent = QColor(16, 185, 129);
        m_colors.accentHover = QColor(52, 211, 153);
        m_colors.accentMuted = QColor(6, 78, 59);
        m_colors.accentGlow = QColor(16, 185, 129, 60);

        m_colors.userBubbleBg = QColor(39, 39, 42);
        m_colors.userBubbleBorder = QColor(63, 63, 70);
        m_colors.userBubbleText = QColor(244, 244, 245);
        m_colors.botBubbleBg = Qt::transparent;
        m_colors.botBubbleBorder = Qt::transparent;
        m_colors.botBubbleText = QColor(244, 244, 245);

        m_colors.winClose = QColor(255, 95, 87);
        m_colors.winMin = QColor(254, 188, 46);
        m_colors.winMax = QColor(40, 200, 64);
    } else {
        // Light theme tokens
        m_colors.windowBg = QColor(248, 250, 252);
        m_colors.sidebarBg = QColor(241, 245, 249);
        m_colors.chatBg = QColor(255, 255, 255);
        m_colors.panelBg = QColor(241, 245, 249);
        m_colors.surfaceBg = QColor(226, 232, 240);
        m_colors.cardBg = QColor(255, 255, 255);
        m_colors.cardBorder = QColor(226, 232, 240);

        m_colors.textPrimary = QColor(15, 23, 42);
        m_colors.textSecondary = QColor(71, 85, 105);
        m_colors.textMuted = QColor(148, 163, 184);

        m_colors.borderSubtle = QColor(226, 232, 240);
        m_colors.borderFocus = QColor(16, 185, 129);
        m_colors.divider = QColor(226, 232, 240);

        m_colors.accent = QColor(16, 185, 129);
        m_colors.accentHover = QColor(5, 150, 105);
        m_colors.accentMuted = QColor(209, 250, 229);
        m_colors.accentGlow = QColor(16, 185, 129, 40);

        m_colors.userBubbleBg = QColor(209, 250, 229);
        m_colors.userBubbleBorder = QColor(110, 231, 183);
        m_colors.userBubbleText = QColor(6, 78, 59);
        m_colors.botBubbleBg = QColor(241, 245, 249);
        m_colors.botBubbleBorder = QColor(203, 213, 225);
        m_colors.botBubbleText = QColor(30, 41, 59);

        m_colors.winClose = QColor(255, 95, 87);
        m_colors.winMin = QColor(254, 188, 46);
        m_colors.winMax = QColor(40, 200, 64);
    }
}

QString ThemeManager::qssVariables() const {
    return QString(
        "/* MCowork Theme Tokens */\n"
        "QWidget { font-family: 'Segoe UI', -apple-system, BlinkMacSystemFont, sans-serif; }\n"
    );
}

}  // namespace mcw
