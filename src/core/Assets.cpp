#include "core/Assets.h"

#include <QCoreApplication>
#include <QDir>
#include <QPainter>
#include <QSvgRenderer>

namespace mcw::assets {

QString path(const QString &relative) {
    static const QString root = [] {
        QDir dir(QCoreApplication::applicationDirPath());
        for (int i = 0; i < 5; ++i) {
            if (dir.exists("assets")) return dir.absoluteFilePath("assets");
            if (!dir.cdUp()) break;
        }
        return QString();
    }();
    return root.isEmpty() ? relative : QDir(root).absoluteFilePath(relative);
}

QString toolPath(const QString &relative) {
    static const QString root = [] {
        QDir dir(QCoreApplication::applicationDirPath());
        for (int i = 0; i < 5; ++i) {
            if (dir.exists("tools")) return dir.absoluteFilePath("tools");
            if (!dir.cdUp()) break;
        }
        return QString();
    }();
    return root.isEmpty() ? relative : QDir(root).absoluteFilePath(relative);
}

QPixmap svgPixmap(const QString &relative, const QSize &size) {
    QSvgRenderer renderer(path(relative));
    QPixmap pm(size);
    pm.fill(Qt::transparent);
    if (renderer.isValid()) {
        QPainter p(&pm);
        p.setRenderHint(QPainter::Antialiasing);
        renderer.render(&p);
    }
    return pm;
}

}  // namespace mcw::assets

