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
                             "Ошибка восстановления дома из БД",
                             QMessageBox::Close);

        logWrite("reloadHousesFromDB()", QString("[x] Error SELECT %1").arg(query.lastError().text()));
        return;
    }

    while(query.next())
    {
        PropHouse* propHouse = new PropHouse();
        propHouse->id      = query.value("id_").toString();
        propHouse->name    = query.value("name").toString();
        propHouse->address = query.value("address").toString();

        vectorHouse.append(propHouse);

        QTreeWidgetItem* houseItem = createHouseItem(propHouse);

        if(!reloadRoomsFromDB(houseItem))
        {
            return;
        }
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
        QMessageBox::warning(this,
                             "Ошибка",
                             "Ошибка добавления дома в БД",
                             QMessageBox::Close,
                             QMessageBox::Close);

        logWrite("insertHouseTable()", QString("[x] Error INSERT house %1").arg(query.lastError().text()));
        return false;
    }

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
        QMessageBox::warning(this,
                             "Ошибка",
                             "Ошибка обновления дома",
                             QMessageBox::Close,
                             QMessageBox::Close);

        logWrite("updateNameHouseTable()", QString("[x] Error UPDATE house %1").arg(query.lastError().text()));
        return;
    }
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
        QMessageBox::warning(this,
                             "Ошибка",
                             "Ошибка обновления адреса дома",
                             QMessageBox::Close,
                             QMessageBox::Close);

        logWrite("updateAddressHouseTable()", QString("[x] Error UPDATE address house %1").arg(query .lastError().text()));
        return;
    }
}
//------------------------------------------------------------------------------------

bool SmartHomeConfig::reloadRoomsFromDB(QTreeWidgetItem* houseItem)
{
    QSqlQuery query = QSqlQuery(*dbase);

    QString idHouse = houseItem->data(0, Qt::ToolTipRole).toString();

    QString selectSQL = QString("SELECT * FROM Rooms WHERE id_House = '%1';").arg(idHouse);

    if(!query.exec(selectSQL))
    {
        QMessageBox::warning(this,
                             "Ошибка",
                             QString("Не удалось восстановить комнаты из БД: %1").arg(query.lastError().text()),
                             QMessageBox::Close);

        logWrite("reloadRoomsFromDB()", QString("[x] Error SELECT %1").arg(query.lastError().text()));
        return false;
    }

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

    return true;
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
        QMessageBox::warning(this,
                             "Ошибка",
                             QString("Не удалось добавить комнату в БД: %1").arg(query.lastError().text()),
                             QMessageBox::Close,
                             QMessageBox::Close);

        logWrite("insertRoomTable()", QString("[x] Error INSERT room %1").arg(query.lastError().text()));
        return false;
    }

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
        QMessageBox::warning(this,
                             "Ошибка",
                             "Ошибка обновления имени комнаты",
                             QMessageBox::Close,
                             QMessageBox::Close);

        logWrite("updateNameRoomTable()", QString("[x] Error UPDATE name room %1").arg(query .lastError().text()));
        return;
    }
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
        QMessageBox::warning(this,
                             "Ошибка",
                             "Ошибка обновления площади комнаты",
                             QMessageBox::Close,
                             QMessageBox::Close);

        logWrite("updateSquareRoomTable()", QString("[x] Error UPDATE square room %1").arg(query .lastError().text()));
        return;
    }
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
        QMessageBox::warning(this,
                             "Ошибка",
                             "Ошибка обновления колличества окон комнаты",
                             QMessageBox::Close,
                             QMessageBox::Close);

        logWrite("updateWindowRoomTable()", QString("[x] Error UPDATE count_window room: ").arg(query .lastError().text()));
        return;
    }
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
                             "Ошибка восстановления датчика из БД",
                             QMessageBox::Close);

        logWrite("reloadSensorsFromDB()", QString("[x] Error SELECT Sensors %1").arg(query.lastError().text()));
        return;
    }

    while(query.next())
    {
        PropSensor* propSensor = new PropSensor();

        propSensor->id         = query.value("id_").toString();
        propSensor->idRoom     = query.value("id_room").toString();
        propSensor->name       = query.value("name").toString();
        propSensor->typeSensor = query.value("type_sensor").toInt();

        vectorSensor.append(propSensor);

        createSensorItem(propSensor, roomItem);
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
        QMessageBox::warning(this,
                             "Ошибка",
                             "Ошибка добавления датчика в БД",
                             QMessageBox::Close,
                             QMessageBox::Close);

        logWrite("insertSensorTable()", QString("[x] Error INSERT sensor %1").arg(query.lastError().text()));
        return false;
    }
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
        QMessageBox::warning(this,
                             "Ошибка",
                             "Не удалось обновить имя датчика",
                             QMessageBox::Close,
                             QMessageBox::Close);

        logWrite("updateNameSensorTable()", QString("[x] Error UPDATE name sensor %1").arg(query.lastError().text()));
        return;
    }
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
        QMessageBox::warning(this,
                             "Ошибка",
                             "Ошибка при  обновлении типа датчика",
                             QMessageBox::Close,
                             QMessageBox::Close);

        logWrite("updateTypeSensorTable()", QString("[x] Error UPDATE type_sensor %1").arg(query.lastError().text()));
        return;
    }
}
//------------------------------------------------------------------------------------

bool SmartHomeConfig::deleteHouseFromTable(PropHouse* propHouse)
{
    QSqlQuery query = QSqlQuery(*dbase);

    QString deleteHouseSQL = QString("DELETE FROM Houses WHERE id_ = '%1';").arg(propHouse->id);

    if(!query.exec(deleteHouseSQL))
    {
        QMessageBox::warning(this,
                             "Ошибка",
                             "Ошибка при удалении дома",
                             QMessageBox::Close,
                             QMessageBox::Close);

        logWrite("deleteHouseFromTable()", QString("[x] Error DELETE House %1").arg(query.lastError().text()));
        return false;
    }

    return true;
}
//------------------------------------------------------------------------------------

bool SmartHomeConfig::deleteRoomFromTable(PropRoom* propRoom)
{
    QSqlQuery query = QSqlQuery(*dbase);

    QString deleteRoomSQL = QString("DELETE FROM Rooms WHERE id_ = '%1';").arg(propRoom->id);

    if(!query.exec(deleteRoomSQL))
    {
        QMessageBox::warning(this,
                             "Ошибка",
                             "Ошибка при удалении комнаты",
                             QMessageBox::Close,
                             QMessageBox::Close);

        logWrite("deleteRoomFromTable()", QString("[x] Error DELETE Room %1").arg(query.lastError().text()));
        return false;
    }

    return true;
}
//------------------------------------------------------------------------------------

bool SmartHomeConfig::deleteSensorFromTable(PropSensor* propSensor)
{
    QSqlQuery query = QSqlQuery(*dbase);

    QString deleteSensorSQL = QString("DELETE FROM SENSORS WHERE id_ = '%1';").arg(propSensor->id);

    if(!query.exec(deleteSensorSQL))
    {
        QMessageBox::warning(this,
                             "Ошибка",
                             "Ошибка при удалении датчика",
                             QMessageBox::Close,
                             QMessageBox::Close);

        logWrite("deleteSensorFromTable()", QString("[x] Error DELETE Sensor %1").arg(query.lastError().text()));
        return false;
    }

    return true;
}
//------------------------------------------------------------------------------------

bool SmartHomeConfig::clearFromTables()
{
    QSqlQuery query = QSqlQuery(*dbase);

    QString clearSQL = "DELETE FROM Houses; DELETE FROM Rooms; DELETE FROM Sensors;";

    if(!query.exec(clearSQL))
    {
        QMessageBox::warning(this,
                             "Ошибка",
                             "Ошибка при удалении данных",
                             QMessageBox::Close,
                             QMessageBox::Close);
        logWrite("clearFromTables()", QString("[x] Error DELETE tables %1").arg(query.lastError().text()));
        return false;
    }

    return true;
}
//------------------------------------------------------------------------------------
