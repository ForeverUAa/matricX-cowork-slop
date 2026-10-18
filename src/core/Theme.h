#pragma once

#include <QColor>

// Central place for sizes and colours used across the UI.
namespace mcw::theme {

// geometry
inline constexpr int kDefaultWidth = 1000;
inline constexpr int kDefaultHeight = 711;
inline constexpr int kMinWidth = 720;
inline constexpr int kMinHeight = 480;
inline constexpr int kSidebarWidth = 221;
inline constexpr int kWindowRadius = 16;
inline constexpr int kCaptionHeight = 56;  // draggable title strip at the top
inline constexpr int kResizeBorder = 8;    // invisible resize zone around the window

// window controls
inline const QColor kMinimize(255, 221, 0);
inline const QColor kMaximize(43, 255, 0);
inline const QColor kClose(255, 0, 0);

// sidebar
inline const QColor kSidebarBg(64, 64, 64);
inline const QColor kChatItemBg(93, 93, 93);
inline const QColor kChatItemHoverBg(104, 104, 104);
inline const QColor kChatItemText(232, 232, 232);
inline const QColor kSettingsBg(88, 88, 88);

// chat area
inline const QColor kTitleFill(34, 34, 34);
inline const QColor kTitleOutline(10, 125, 0);
inline const QColor kGradientBottom(16, 170, 0);
inline const QColor kInputInner(61, 61, 61);
inline const QColor kInputBorder(51, 51, 51);
inline const QColor kInputText(238, 238, 238);
inline const QColor kInputPlaceholder(150, 150, 160);
inline const QColor kInputDivider(44, 44, 44);
inline const QColor kUserBubbleBg(18, 54, 15);
inline const QColor kUserBubbleBorder(29, 122, 18);
inline const QColor kBotBubbleBg(43, 43, 43);
inline const QColor kBotBubbleText(221, 221, 221);

}  // namespace mcw::theme
