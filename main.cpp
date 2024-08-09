#include <QApplication>
#include <QDesktopWidget>

#include "SmartHomeConfig.h"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    SmartHomeConfig sHC;
    sHC.show();

    return a.exec();
}
