#include "SmartHomeConfig.h"
//------------------------------------------------------------------------------------

#include <QDebug>
#include <QMessageBox>
//------------------------------------------------------------------------------------


void SmartHomeConfig::reloadHousesFromDB()
{
    QSqlQuery query = QSqlQuery(*dbase);

    QString selectSQL = "SELECT * FROM Houses;";

    if(!query.exec(selectSQL))
    {
        QMessageBox::warning(this,
                             "Ошибка",
                             "Не удалось восстановить дома из БД",
                             QMessageBox::Close);

        qDebug() << "[x] Error SELECT:" << query.lastError().text();
        return;
    }

    qDebug() << "[v] Success SELECT ";

    while(query.next())
    {
        PropHouse* propHouse = new PropHouse();
        propHouse->id      = query.value("id_").toString();
        propHouse->name    = query.value("name").toString();
        propHouse->address = query.value("address").toString();

        vectorHouse.append(propHouse);

        QTreeWidgetItem* houseItem = createHouseItem(propHouse);

        reloadRoomsFromDB(houseItem);
    }

    ObjectsTree->expandAll();
    QTreeWidgetItem* houseItem = ObjectsTree->topLevelItem(0);
    ObjectsTree->setCurrentItem(houseItem);
}
//------------------------------------------------------------------------------------

//  Добавляем дом в БД
/*!
 * \brief Добавление дома в БД
 * \param propHouse  свойства дома
 * \return           true, если успешно выполнен запрос к БД. Иначе false.
 *
 * В случае ошибки информация о ней выводится в MessageBox.
 */
bool SmartHomeConfig::insertHouseTable(PropHouse* propHouse)
{
    QSqlQuery query = QSqlQuery(*dbase);

    QString insertHouseSQL = QString("INSERT INTO Houses (id_, name, address) VALUES (:id_, :name, :address);");
    query.prepare(insertHouseSQL);
    query.bindValue(":id_"    , propHouse->id);
    query.bindValue(":name"   , propHouse->name);
    query.bindValue(":address", propHouse->address);

    if(!query.exec())
    {
// TODO
        qDebug() << "[x] Error insert house: " << query.lastError().text();

        QMessageBox::warning(this,
                             "Ошибка",
                             QString("Не удалось добавить дом в БД"),
                             QMessageBox::Close,
                             QMessageBox::Close);
        return false;
    }

// TODO
    qDebug() << "[v] Success insert house";

    return true;
}
//------------------------------------------------------------------------------------

void SmartHomeConfig::updateNameHouseTable(PropHouse* propHouse)
{
    QSqlQuery query = QSqlQuery(*dbase);

    QString findHouse = QString("UPDATE HOUSES SET name = :name WHERE id_ = :id;");
    query.prepare(findHouse);
    query.bindValue(":name", propHouse->name);
    query.bindValue(":id"  , propHouse->id);

    if(!query.exec())
    {
// TODO
        qDebug() << "[x] Error update house:" << query.lastError().text();
        QMessageBox::warning(this,
                             "Ошибка",
                             "Не удалось обновить дом",
                             QMessageBox::Close,
                             QMessageBox::Close);
        return;
    }
// TODO
        qDebug() << "[v] Success update house ";
}
//------------------------------------------------------------------------------------

void SmartHomeConfig::updateAddressHouseTable(PropHouse* propHouse)
{
    QSqlQuery query = QSqlQuery(*dbase);

    QString findHouse = QString("UPDATE HOUSES SET address = :address WHERE id_ = :id;");
    query.prepare(findHouse);
    query.bindValue(":address", propHouse->address);
    query.bindValue(":id"     , propHouse->id);

    if(!query.exec())
    {
// TODO
        qDebug() << "[x] Error UPDATE address house: " << query .lastError().text();

        QMessageBox::warning(this,
                             "Ошибка",
                             "Не удалось обновить адрес дома",
                             QMessageBox::Close,
                             QMessageBox::Close);
        return;
    }
//TODO
    qDebug() << "[v] Success UPDATE address house";
}
//------------------------------------------------------------------------------------


void SmartHomeConfig::reloadRoomsFromDB(QTreeWidgetItem* houseItem)
{
    QSqlQuery query = QSqlQuery(*dbase);

    QString idHouse = houseItem->data(0, Qt::ToolTipRole).toString();

    QString selectSQL = QString("SELECT * FROM Rooms WHERE id_House = '%1';").arg(idHouse);

    if(!query.exec(selectSQL))
    {
        QMessageBox::warning(this,
                             "Ошибка",
                             "Не удалось восстановить комнаты из БД",
                             QMessageBox::Close);
//TODO
        qDebug() << "[x] Error SELECT:" << query.lastError().text();
        return;
    }
//TODO
    qDebug() << "[v] Success SELECT Rooms";

    while(query.next())
    {
        PropRoom* propRoom = new PropRoom();

        propRoom->id          = query.value("id_").toString();
        propRoom->idHouse     = query.value("id_house").toString();
        propRoom->name        = query.value("name").toString();
        propRoom->square      = query.value("square").toDouble();
        propRoom->countWindow = query.value("count_window").toInt();

        vectorRoom.append(propRoom);

        QTreeWidgetItem* roomItem = createRoomItem(propRoom, houseItem);

        reloadSensorsFromDB(roomItem);
    }
}

//------------------------------------------------------------------------------------

bool SmartHomeConfig::insertRoomTable(PropRoom* propRoom)
{
    QSqlQuery query = QSqlQuery(*dbase);

    QString insertRoomSQL = QString("INSERT INTO Rooms (id_, id_house, name, square, count_window) VALUES (:id_, :id_house, :name, :square, :count_window);");
    query.prepare(insertRoomSQL);
    query.bindValue(":id_"         , propRoom->id);
    query.bindValue(":id_house"    , propRoom->idHouse);
    query.bindValue(":name"        , propRoom->name);
    query.bindValue(":square"      , propRoom->square);
    query.bindValue(":count_window", propRoom->countWindow);

    if(!query.exec())
    {
//TODO
        qDebug() << "[x] Error insert room:" << query.lastError().text();
        QMessageBox::warning(this,
                             "Ошибка",
                             QString("Не удалось добавить комнату в БД"),
                             QMessageBox::Close,
                             QMessageBox::Close);
        return false;
    }

// TODO
    qDebug() << "[v] Success insert room";
    return true;
}
//------------------------------------------------------------------------------------

void SmartHomeConfig::updateNameRoomTable(PropRoom* propRoom)
{
    QSqlQuery query = QSqlQuery(*dbase);

    QString updateNameRoom = QString("UPDATE ROOMS SET name = :name WHERE id_ = :id;");
    query.prepare(updateNameRoom);
    query.bindValue(":name", propRoom->name);
    query.bindValue(":id"  , propRoom->id);

    if(!query.exec())
    {
//TODO
        qDebug() << "[x] Error UPDATE name room: " << query .lastError().text();
        QMessageBox::warning(this,
                             "Ошибка",
                             "Не удалось обновить имя комнаты",
                             QMessageBox::Close,
                             QMessageBox::Close);
        return;
    }
//TODO
    qDebug() << "[v] Success update name Room";
}
//------------------------------------------------------------------------------------

void SmartHomeConfig::updateSquareRoomTable(PropRoom* propRoom)
{
    QSqlQuery query = QSqlQuery(*dbase);

    QString updateSquareRoom = QString("UPDATE ROOMS SET square = :square WHERE id_ = :id;");
    query.prepare(updateSquareRoom);
    query.bindValue(":square", propRoom->square);
    query.bindValue(":id"    , propRoom->id);

    if(!query.exec())
    {
//TODO
        qDebug() << "[x] Error UPDATE square room: " << query .lastError().text();
        QMessageBox::warning(this,
                             "Ошибка",
                             "Не удалось обновить площадь комнаты",
                             QMessageBox::Close,
                             QMessageBox::Close);
        return;
    }
//TODO
    qDebug() << "[v] Success update square Room";
}
//------------------------------------------------------------------------------------

void SmartHomeConfig::updateWindowRoomTable(PropRoom* propRoom)
{
    QSqlQuery query = QSqlQuery(*dbase);

    QString updateWindowRoom = QString("UPDATE ROOMS SET count_window = :countWindow WHERE id_ = :id;");
    query.prepare(updateWindowRoom);
    query.bindValue(":countWindow", propRoom->countWindow);
    query.bindValue(":id"         , propRoom->id);

    if(!query.exec())
    {
//TODO
        qDebug() << "[x] Error UPDATE count_window room: " << query .lastError().text();
        QMessageBox::warning(this,
                             "Ошибка",
                             "Не удалось обновить колличество окон комнаты",
                             QMessageBox::Close,
                             QMessageBox::Close);
        return;
    }

//TODO
    qDebug() << "[v] Success update countWindow Room";
}
//------------------------------------------------------------------------------------




void SmartHomeConfig::reloadSensorsFromDB(QTreeWidgetItem* roomItem)
{
    QSqlQuery query = QSqlQuery(*dbase);

    QString idRoom = roomItem->data(0, Qt::ToolTipRole).toString();

    QString selectSQL = QString("SELECT * FROM Sensors WHERE id_room = '%1';").arg(idRoom);

    if(!query.exec(selectSQL))
    {
        QMessageBox::warning(this,
                             "Ошибка",
                             "Не удалось восстановить датчики из БД",
                             QMessageBox::Close);
//TODO
        qDebug() << "[x] Error SELECT Sensors: " << query.lastError().text();
        return;
    }
//TODO
    qDebug() << "[v] Success SELECT Sensors";

    while(query.next())
    {
        PropSensor* propSensor = new PropSensor();

        propSensor->id         = query.value("id_").toString();
        propSensor->idRoom     = query.value("id_room").toString();
        propSensor->name       = query.value("name").toString();
        propSensor->typeSensor = query.value("type_sensor").toInt();

        vectorSensor.append(propSensor);

        QTreeWidgetItem* sensorItem = createSensorItem(propSensor, roomItem);
    }
}
//------------------------------------------------------------------------------------

bool SmartHomeConfig::insertSensorTable(PropSensor* propSensor)
{
    QSqlQuery query = QSqlQuery(*dbase);

    QString insertSensorTable = QString("INSERT INTO Sensors (id_, id_room, name, type_sensor) VALUES (:id, :idRoom, :name, :typeSensor);");
    query.prepare(insertSensorTable);
    query.bindValue(":id"        , propSensor->id);
    query.bindValue(":idRoom"    , propSensor->idRoom);
    query.bindValue(":name"      , propSensor->name);
    query.bindValue(":typeSensor", propSensor->typeSensor);

    if(!query.exec())
    {
// TODO
        qDebug() << "[x] Error insert sensor";
        QMessageBox::warning(this,
                             "Ошибка",
                             "Не удалось добавить датчик в БД",
                             QMessageBox::Close,
                             QMessageBox::Close);
        return false;
    }
// TODO
    qDebug() << "[v] Success insert sensor";
    return true;
}
//------------------------------------------------------------------------------------

void SmartHomeConfig::updateNameSensorTable(PropSensor* propSensor)
{
    QSqlQuery query = QSqlQuery(*dbase);

    QString updateNameSensor = QString("UPDATE SENSORS SET name = :name WHERE id_ = :id;");
    query.prepare(updateNameSensor);
    query.bindValue(":name", propSensor->name);
    query.bindValue(":id"  , propSensor->id);

    if(!query.exec())
    {
//TODO
        qDebug() << "[x] Error UPDATE name sensor: " << query .lastError().text();
        QMessageBox::warning(this,
                             "Ошибка",
                             "Не удалось обновить имя датчика",
                             QMessageBox::Close,
                             QMessageBox::Close);
        return;
    }
//TODO
    qDebug() << "[v] Success update name sensor";
}
//------------------------------------------------------------------------------------

void SmartHomeConfig::updateTypeSensorTable(PropSensor* propSensor)
{
    QSqlQuery query = QSqlQuery(*dbase);

    QString updateTypeSensor = QString("UPDATE Sensors SET type_sensor = :typeSensor WHERE id_ = :id;");
    query.prepare(updateTypeSensor);
    query.bindValue(":typeSensor", propSensor->typeSensor);
    query.bindValue(":id"        , propSensor->id);

    if(!query.exec())
    {
//TODO
        qDebug() << "[x] Error UPDATE type_sensor";

        QMessageBox::warning(this,
                             "Ошибка",
                             "Не удалось обновить тип датчика",
                             QMessageBox::Close,
                             QMessageBox::Close);
        return;
    }

//TODO
    qDebug() << "[v] Success update type_sensor";
}
//------------------------------------------------------------------------------------





































