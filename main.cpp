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


// -hn=localHost   || --hostName=localHost
// -p=5432         || --port=5432
// -un=postgres    || --username=postgres
// -pwd=19871      || --password=19871
// -dbn=SQLPractic || --databasename=SQLPractic

// -hN=localHost  -p=5432 -uN=postgres -pwd=19871 -dBN=SmartHomeConfig


void printHelp()
{
    qDebug() << "-h or --help\n"
             << "Example:\n"
             << "-h\n"

             << "-hN or --hostName\n"
             << "Example:\n"
             << "-hN=localhost\n"

             << "-p or --port\n"
             << "Example:\n"
             << "-p=5432\n"

             << "-uN or --userName\n"
             << "Example:\n"
             << "-uN=postgres\n"

             << "-pwd or --password\n"
             << "Example:\n"
             << "-pwd=19871\n"

             << "-dBN or --dataBaseName\n"
             << "Example:\n"
             << "-dBN=SmartHomeConfig\n";
}
//----------------------------------------------------------------------


bool createHouseTable(QSqlDatabase& dbase)
{
    QSqlQuery query = QSqlQuery(dbase);

    QString createHomeTableSQL = "CREATE TABLE IF NOT EXISTS Houses("
                                 "id_     VARCHAR(38) PRIMARY KEY NOT NULL,"
                                 "name    VARCHAR(50),"
                                 "address VARCHAR(50)"
                                 ")";

    if(!query.exec(createHomeTableSQL))
    {
        qDebug() << "[x] Error create House table: " << query.lastError().text();
        return false;
    }

    qDebug() << "[v] Success create Houses table";
    return true;
}
//------------------------------------------------------------------------------------


bool createRoomTable(QSqlDatabase& dbase)
{
    QSqlQuery query = QSqlQuery(dbase);

    QString createRoomTable = "CREATE TABLE IF NOT EXISTS Rooms("
                              "id_          VARCHAR(38) PRIMARY KEY NOT NULL,"
                              "id_house     VARCHAR(38) NOT NULL,"
                              "name         VARCHAR(50),"
                              "square       REAL,"
                              "count_window INT"
                              ")";

    if(!query.exec(createRoomTable))
    {
        qDebug() << "[x] Error create Rooms table" << query.lastError().text();
        return false;
    }

    qDebug() << "[v] Success create Rooms table";
    return true;
}
//------------------------------------------------------------------------------------

bool createSensorTable(QSqlDatabase& dbase)
{
    QSqlQuery query = QSqlQuery(dbase);

    QString createSensorTable = "CREATE TABLE IF NOT EXISTS Sensors("
                                "id_         VARCHAR(38) PRIMARY KEY NOT NULL,"
                                "id_room     VARCHAR(38) NOT NULL,"
                                "name        VARCHAR(50),"
                                "type_sensor INT"
                                ")";

    if(!query.exec(createSensorTable))
    {
        qDebug() << "[x] Error create Sensors table" << query.lastError().text();
        return false;
    }

    qDebug() << "[v] Success create Sensors table";
    return true;
}
//----------------------------------------------------------------------


int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    Arguments arguments = Arguments(argc, argv);
    if(arguments.isHelp)
    {
        printHelp();
        return 0;
    }

    QSqlDatabase dbase = QSqlDatabase::addDatabase("QPSQL");
    dbase.setHostName(arguments.hostName);
    dbase.setPort(arguments.port);
    dbase.setUserName(arguments.userName);
    dbase.setPassword(arguments.pwd);
    dbase.setDatabaseName(arguments.dataBaseName);

    if(!dbase.open())
    {
        QMessageBox::warning(nullptr,
                             "Ошибка",
                             QString("Нет соединения с БД: %1").arg(dbase.lastError().text()),
                             QMessageBox::Close,
                             QMessageBox::Close);
        return 0;
    }

    qDebug() << "CONNECT DB";

    SmartHomeConfig sHC;
    sHC.init(&dbase);

    bool isCreateHousesTable  = createHouseTable(dbase);
    bool isCreateRoomsTable   = createRoomTable(dbase);
    bool isCreateSensorsTable = createSensorTable(dbase);

    if(!isCreateHousesTable || !isCreateRoomsTable || !isCreateSensorsTable)
    {
        QMessageBox::warning(nullptr,
                             "Ошибка",
                             QString("Таблицы не созданы"),
                             QMessageBox::Close,
                             QMessageBox::Close);
        return 0;
    }

    sHC.show();

    return a.exec();
}
//----------------------------------------------------------------------
