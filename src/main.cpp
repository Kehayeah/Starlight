#include <QApplication>
#include <QStringList>
#include <QWindow>
#include "app/window.hpp"
#include "canbus/psaserial.hpp"

int main(int argc, char *argv[])
{
    QApplication starlight(argc, argv);

    starlight.setOrganizationName("openDsh");
    starlight.setApplicationName("dash");

    QStringList args = starlight.arguments();
    bool use_fixed_size = (args.size() > 2);

    QSize size = starlight.primaryScreen()->size();
    if (use_fixed_size)
        size = QSize(args.at(1).toInt(), args.at(2).toInt());

    QPixmap pixmap(QPixmap(":/splash.png").scaledToHeight(size.height()/2));
    QSplashScreen splash(pixmap);
    splash.setMask(pixmap.mask());
    splash.show();
    starlight.processEvents();

    Window window;
    window.setWindowIcon(QIcon(":/logo.png"));
    window.setWindowFlags(Qt::FramelessWindowHint);
    if (!use_fixed_size)
        window.setWindowState(Qt::WindowFullScreen);

    // force to either screen or custom size
    window.setFixedSize(size);
    window.show();
    splash.finish(&window);

    return starlight.exec();
}
