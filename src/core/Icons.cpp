#include "core/Icons.h"

#include <QPainter>
#include <QSvgRenderer>

namespace mcw::icons {

QString svgString(IconType type, const QColor &color, float strokeWidth) {
    const QString hex = color.name();
    const QString stroke = QString::number(strokeWidth, 'f', 1);

    QString pathData;
    switch (type) {
    case IconType::Mic:
        // Lucide: mic
        pathData = QString(
            "<path d=\"M12 2a3 3 0 0 0-3 3v7a3 3 0 0 0 6 0V5a3 3 0 0 0-3-3Z\"/>"
            "<path d=\"M19 10v2a7 7 0 0 1-14 0v-2\"/>"
            "<line x1=\"12\" x2=\"12\" y1=\"19\" y2=\"22\"/>");
        break;
    case IconType::MicOff:
        // Lucide: mic-off
        pathData = QString(
            "<line x1=\"2\" x2=\"22\" y1=\"2\" y2=\"22\"/>"
            "<path d=\"M18.89 13.23A7.12 7.12 0 0 0 19 12v-2\"/>"
            "<path d=\"M5 10v2a7 7 0 0 0 12 5\"/>"
            "<path d=\"M15 9.34V5a3 3 0 0 0-5.68-1.33\"/>"
            "<path d=\"M9 9v3a3 3 0 0 0 5.12 2.12\"/>"
            "<line x1=\"12\" x2=\"12\" y1=\"19\" y2=\"22\"/>");
        break;
    case IconType::Gear:
        // Lucide: settings
        pathData = QString(
            "<path d=\"M12.22 2h-.44a2 2 0 0 0-2 2v.18a2 2 0 0 1-1 1.73l-.43.25a2 2 0 0 1-2 0l-.15-.08a2 2 0 0 0-2.73.73l-.22.38a2 2 0 0 0 .73 2.73l.15.1a2 2 0 0 1 1 1.72v.51a2 2 0 0 1-1 1.74l-.15.09a2 2 0 0 0-.73 2.73l.22.38a2 2 0 0 0 2.73.73l.15-.08a2 2 0 0 1 2 0l.43.25a2 2 0 0 1 1 1.73V20a2 2 0 0 0 2 2h.44a2 2 0 0 0 2-2v-.18a2 2 0 0 1 1-1.73l.43-.25a2 2 0 0 1 2 0l.15.08a2 2 0 0 0 2.73-.73l.22-.39a2 2 0 0 0-.73-2.73l-.15-.08a2 2 0 0 1-1-1.74v-.5a2 2 0 0 1 1-1.74l.15-.09a2 2 0 0 0 .73-2.73l-.22-.38a2 2 0 0 0-2.73-.73l-.15.08a2 2 0 0 1-2 0l-.43-.25a2 2 0 0 1-1-1.73V4a2 2 0 0 0-2-2z\"/>"
            "<circle cx=\"12\" cy=\"12\" r=\"3\"/>");
        break;
    case IconType::Sparkles:
        // Lucide: sparkles
        pathData = QString(
            "<path d=\"m12 3-1.912 5.813a2 2 0 0 1-1.275 1.275L3 12l5.813 1.912a2 2 0 0 1 1.275 1.275L12 21l1.912-5.813a2 2 0 0 1 1.275-1.275L21 12l-5.813-1.912a2 2 0 0 1-1.275-1.275L12 3Z\"/>"
            "<path d=\"M5 3v4\"/><path d=\"M19 17v4\"/><path d=\"M3 5h4\"/><path d=\"M17 19h4\"/>");
        break;
    case IconType::Trash:
        // Lucide: trash-2
        pathData = QString(
            "<path d=\"M3 6h18\"/>"
            "<path d=\"M19 6v14c0 1-1 2-2 2H7c-1 0-2-1-2-2V6\"/>"
            "<path d=\"M8 6V4c0-1 1-2 2-2h4c1 0 2 1 2 2v2\"/>"
            "<line x1=\"10\" x2=\"10\" y1=\"11\" y2=\"17\"/>"
            "<line x1=\"14\" x2=\"14\" y1=\"11\" y2=\"17\"/>");
        break;
    case IconType::Archive:
        // Lucide: archive
        pathData = QString(
            "<rect width=\"20\" height=\"5\" x=\"2\" y=\"3\" rx=\"1\"/>"
            "<path d=\"M4 8v11a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V8\"/>"
            "<path d=\"M10 12h4\"/>");
        break;
    case IconType::RotateCcw:
        // Lucide: rotate-ccw
        pathData = QString(
            "<path d=\"M3 12a9 9 0 1 0 9-9 9.75 9.75 0 0 0-6.74 2.74L3 8\"/>"
            "<path d=\"M3 3v5h5\"/>");
        break;
    case IconType::ChevronDown:
        pathData = "<path d=\"m6 9 6 6 6-6\"/>";
        break;
    case IconType::ChevronRight:
        pathData = "<path d=\"m9 18 6-6-6-6\"/>";
        break;
    case IconType::ChevronUp:
        pathData = "<path d=\"m18 15-6-6-6 6\"/>";
        break;
    case IconType::MoreVertical:
        pathData = "<circle cx=\"12\" cy=\"12\" r=\"1\"/><circle cx=\"12\" cy=\"5\" r=\"1\"/><circle cx=\"12\" cy=\"19\" r=\"1\"/>";
        break;
    case IconType::Plus:
        pathData = "<path d=\"M5 12h14\"/><path d=\"M12 5v14\"/>";
        break;
    case IconType::Check:
        pathData = "<path d=\"M20 6 9 17l-5-5\"/>";
        break;
    case IconType::X:
        pathData = "<path d=\"M18 6 6 18\"/><path d=\"m6 6 12 12\"/>";
        break;
    case IconType::Cpu:
        pathData = QString(
            "<rect width=\"16\" height=\"16\" x=\"4\" y=\"4\" rx=\"2\"/>"
            "<rect width=\"6\" height=\"6\" x=\"9\" y=\"9\" rx=\"1\"/>"
            "<path d=\"M15 2v2\"/><path d=\"M15 20v2\"/><path d=\"M2 15h2\"/><path d=\"M2 9h2\"/>"
            "<path d=\"M20 15h2\"/><path d=\"M20 9h2\"/><path d=\"M9 2v2\"/><path d=\"M9 20v2\"/>");
        break;
    case IconType::Bot:
        // Lucide: bot
        pathData = QString(
            "<path d=\"M12 8V4H8\"/>"
            "<rect width=\"16\" height=\"12\" x=\"4\" y=\"8\" rx=\"2\"/>"
            "<path d=\"M2 14h2\"/><path d=\"M20 14h2\"/>"
            "<path d=\"M15 13v2\"/><path d=\"M9 13v2\"/>");
        break;
    case IconType::Key:
        pathData = QString(
            "<path d=\"m15.5 7.5 2.3 2.3a1 1 0 0 0 1.4 0l2.1-2.1a1 1 0 0 0 0-1.4L19 4\"/>"
            "<path d=\"m21 2-9.6 9.6\"/>"
            "<circle cx=\"7.5\" cy=\"15.5\" r=\"5.5\"/>");
        break;
    case IconType::Sun:
        pathData = QString(
            "<circle cx=\"12\" cy=\"12\" r=\"4\"/>"
            "<path d=\"M12 2v2\"/><path d=\"M12 20v2\"/><path d=\"m4.93 4.93 1.41 1.41\"/>"
            "<path d=\"m17.66 17.66 1.41 1.41\"/><path d=\"M2 12h2\"/><path d=\"M20 12h2\"/>"
            "<path d=\"m6.34 17.66-1.41 1.41\"/><path d=\"m19.07 4.93-1.41 1.41\"/>");
        break;
    case IconType::Moon:
        pathData = "<path d=\"M12 3a6 6 0 0 0 9 9 9 9 0 1 1-9-9Z\"/>";
        break;
    case IconType::Monitor:
        pathData = QString(
            "<rect width=\"20\" height=\"14\" x=\"2\" y=\"3\" rx=\"2\"/>"
            "<line x1=\"8\" x2=\"16\" y1=\"21\" y2=\"21\"/>"
            "<line x1=\"12\" x2=\"12\" y1=\"17\" y2=\"21\"/>");
        break;
    case IconType::Search:
        pathData = "<circle cx=\"11\" cy=\"11\" r=\"8\"/><path d=\"m21 21-4.3-4.3\"/>";
        break;
    case IconType::Shield:
        pathData = "<path d=\"M20 13c0 5-3.5 7.5-7.66 8.95a1 1 0 0 1-.67-.01C7.5 20.5 4 18 4 13V6a1 1 0 0 1 1-1c2 0 4.5-1.2 6.24-2.72a1.17 1.17 0 0 1 1.52 0C14.51 3.81 17 5 19 5a1 1 0 0 1 1 1z\"/>";
        break;
    case IconType::ExternalLink:
        pathData = "<path d=\"M15 3h6v6\"/><path d=\"M10 14 21 3\"/><path d=\"M18 13v6a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2V8a2 2 0 0 1 2-2h6\"/>";
        break;
    }

    return QString(
        "<svg xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 24 24\" "
        "fill=\"none\" stroke=\"%1\" stroke-width=\"%2\" stroke-linecap=\"round\" stroke-linejoin=\"round\">"
        "%3"
        "</svg>"
    ).arg(hex, stroke, pathData);
}

QPixmap pixmap(IconType type, const QSize &size, const QColor &color, float strokeWidth) {
    const QString xml = svgString(type, color, strokeWidth);
    QSvgRenderer renderer(xml.toUtf8());
    QPixmap pm(size);
    pm.fill(Qt::transparent);
    if (renderer.isValid()) {
        QPainter p(&pm);
        p.setRenderHint(QPainter::Antialiasing);
        renderer.render(&p);
    }
    return pm;
}

QIcon icon(IconType type, const QColor &color) {
    QIcon ic;
    ic.addPixmap(pixmap(type, QSize(16, 16), color));
    ic.addPixmap(pixmap(type, QSize(20, 20), color));
    ic.addPixmap(pixmap(type, QSize(24, 24), color));
    ic.addPixmap(pixmap(type, QSize(32, 32), color));
    return ic;
}

}  // namespace mcw::icons
