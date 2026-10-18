#pragma once

#include <QPixmap>
#include <QSize>
#include <QString>

namespace mcw::assets {

// Absolute path of a file inside the project's assets folder, e.g. "icons/gear.svg".
// The folder is found by walking up from the executable's directory.
QString path(const QString &relative);

// Absolute path of a file inside the project's tools folder, e.g. "matricX-cowork/matricX-cowork.exe".
QString toolPath(const QString &relative);

// Renders an SVG from the assets folder into a pixmap of the given size.
QPixmap svgPixmap(const QString &relative, const QSize &size);

}  // namespace mcw::assets

