#include <QApplication>
#include <QStringList>
#include <QWindow>
#include "app/window.hpp"
#include "canbus/psaserial.hpp"

int main(int argc, char *argv[])
{
    QApplication dash(argc, argv);

    dash.setOrganizationName("openDsh");
    dash.setApplicationName("dash");

    QStringList args = dash.arguments();
    //bool use_fixed_size = (args.size() > 2);
    bool use_fixed_size = true;

    QSize size = dash.primaryScreen()->size();
    if (use_fixed_size)
        size = QSize(1024, 600);
        //size = QSize(args.at(1).toInt(), args.at(2).toInt());

    QPixmap pixmap(QPixmap(":/splash.png").scaledToHeight(size.height()/2));
    QSplashScreen splash(pixmap);
    splash.setMask(pixmap.mask());
    splash.show();
    dash.processEvents();
    Window window;
    psaserial();
    window.setWindowIcon(QIcon(":/logo.png"));
    window.setWindowFlags(Qt::FramelessWindowHint);
    /*if (!use_fixed_size)
           window.setWindowState(Qt::WindowFullScreen);
    */


    // force to either screen or custom size
    window.setFixedSize(size);
    window.show();
    splash.finish(&window);

    return dash.exec();
}
