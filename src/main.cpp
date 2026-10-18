#include <QApplication>
#include <QFont>
#include <QIcon>

#include "core/Assets.h"
#include "ui/MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("M-Cowork");
    app.setFont(QFont("Segoe UI", 10));
    app.setWindowIcon(QIcon(mcw::assets::svgPixmap("icons/app.svg", QSize(256, 256))));

    mcw::MainWindow window;
    window.show();
    window.focusInput();
    return app.exec();
}
