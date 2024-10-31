#include <QApplication>
#include <QDesktopWidget>
#include <QSqlDatabase>
#include <QtSql>
#include <QDebug>
#include <QMessageBox>
//----------------------------------------------------------------------

#include "SmartHomeConfig.h"
#include "Arguments.h"
//----------------------------------------------------------------------


int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    SmartHomeConfig sHC;
    sHC.show();

    return a.exec();
}
//----------------------------------------------------------------------
