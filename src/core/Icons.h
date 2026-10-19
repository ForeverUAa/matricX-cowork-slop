#pragma once

#include <QColor>
#include <QIcon>
#include <QPixmap>
#include <QSize>
#include <QString>

namespace mcw::icons {

// Renders icons from the Lucide icon set as pure vector paths via QSvgRenderer / QPixmap.
// Clean, minimal 24x24 viewBox with 2px stroke and round caps/joins.
enum class IconType {
    Mic,
    MicOff,
    Gear,
    Sparkles,
    Trash,
    Archive,
    RotateCcw,
    ChevronDown,
    ChevronRight,
    ChevronUp,
    MoreVertical,
    MoreHorizontal,
    Plus,
    Check,
    X,
    Cpu,
    Bot,
    User,
    Terminal,
    Image,
    PanelLeft,
    ArrowUp,
    Square,
    Key,
    Sun,
    Moon,
    Monitor,
    Search,
    Shield,
    ExternalLink
};

QString svgString(IconType type, const QColor &color = Qt::white, float strokeWidth = 2.0f);
QPixmap pixmap(IconType type, const QSize &size = QSize(20, 20), const QColor &color = Qt::white, float strokeWidth = 2.0f);
QIcon icon(IconType type, const QColor &color = Qt::white);

}  // namespace mcw::icons
