#pragma once

#include <QColor>

// Central place for sizes and colors used across the OpenAI Codex-inspired desktop UI.
namespace mcw::theme {

// geometry
inline constexpr int kDefaultWidth = 1100;
inline constexpr int kDefaultHeight = 780;
inline constexpr int kMinWidth = 780;
inline constexpr int kMinHeight = 520;
inline constexpr int kSidebarWidth = 244;
inline constexpr int kCollapsedSidebarWidth = 54;
inline constexpr int kWindowRadius = 12;
inline constexpr int kCaptionHeight = 46;
inline constexpr int kResizeBorder = 8;
inline constexpr int kChatMaxWidth = 760;

// macOS window controls
inline const QColor kClose(255, 95, 87);
inline const QColor kMinimize(254, 188, 46);
inline const QColor kMaximize(40, 200, 64);

// Authentic OpenAI Codex Dark Palette
inline const QColor kDarkCanvas(24, 24, 27);       // #18181b
inline const QColor kDarkSidebar(18, 18, 20);      // #121214
inline const QColor kDarkSurface(33, 33, 36);      // #212124
inline const QColor kDarkBorder(46, 46, 51);       // #2e2e33
inline const QColor kDarkTextPrimary(244, 244, 245);
inline const QColor kDarkTextSecondary(161, 161, 170);
inline const QColor kDarkTextMuted(113, 113, 122);

// Accent
inline const QColor kAccent(16, 185, 129);         // #10b981
inline const QColor kAccentHover(52, 211, 153);    // #34d399

}  // namespace mcw::theme
