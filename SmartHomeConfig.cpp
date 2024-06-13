#include "SmartHomeConfig.h"
//------------------------------------------------------------------------------------

#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>
#include <QLineEdit>
#include <QFrame>
#include <QUuid>
//------------------------------------------------------------------------------------

enum HouseObject
{
    House  = 1,
    Room   = 2,
    Sensor = 3
};
//------------------------------------------------------------------------------------

SmartHomeConfig::SmartHomeConfig(QWidget* parent) : QWidget(parent)
{
    setGeometry(40, 40, 900, 500);

    ObjectsTree = new QTreeWidget(this);
    ObjectsTree->setSelectionMode(QAbstractItemView::SingleSelection);

    QHeaderView* header = ObjectsTree->header();
    header->hide();

    PassportTable = new QTableWidget();
    PassportTable->setColumnCount(2);
    PassportTable->verticalHeader()->setVisible(false);
    PassportTable->setHorizontalHeaderLabels({"Свойство", "Значение"});
    QHeaderView* headerView = PassportTable->horizontalHeader();
    headerView->setSectionResizeMode(QHeaderView::Stretch);

    addHouseButton = new QPushButton(this);
    addHouseButton->setText("Добавить дом");

    addRoomButton = new QPushButton(this);
    addRoomButton->setText("Добавить комнату");
//    addRoomButton->setEnabled(false);

    addSensorButton = new QPushButton(this);
    addSensorButton->setText("Добавить датчик");
//    addSensorButton->setEnabled(false);

    deleteButton = new QPushButton(this);
    deleteButton->setText("Удалить");
//    deleteButton->setEnabled(false);

    activButton(nullptr, nullptr);

    QHBoxLayout* hLayout1 = new QHBoxLayout();
    hLayout1->addWidget(addHouseButton);
    hLayout1->addWidget(addRoomButton);
    hLayout1->addWidget(addSensorButton);
    hLayout1->addWidget(deleteButton);
    hLayout1->addStretch();

    QHBoxLayout* hLayout2 = new QHBoxLayout();
    hLayout2->addWidget(ObjectsTree);
    hLayout2->addWidget(PassportTable);

    QVBoxLayout* vLayout = new QVBoxLayout(this);
    vLayout->addLayout(hLayout1);
    vLayout->addLayout(hLayout2);

    connect(addHouseButton,  &QPushButton::clicked,     this, &SmartHomeConfig::addHouse);
    connect(addRoomButton,   &QPushButton::clicked,     this, &SmartHomeConfig::addRoom);
    connect(addSensorButton, &QPushButton::clicked,     this, &SmartHomeConfig::addSensor);
    connect(deleteButton,    &QPushButton::clicked,     this, &SmartHomeConfig::deleteItem);

    connect(ObjectsTree,     &QTreeWidget::currentItemChanged, this, &SmartHomeConfig::activButton);
    connect(ObjectsTree,     &QTreeWidget::currentItemChanged, this, &SmartHomeConfig::showPassport);
}
//------------------------------------------------------------------------------------

void SmartHomeConfig::addHouse()
{
    PropHouse* propHouse = new PropHouse();
    QString uuidHouse = QUuid::createUuid().toString();

    QTreeWidgetItem* houseItem = new QTreeWidgetItem(ObjectsTree);
    houseItem->setData(0, Qt::DisplayRole, propHouse->name);
    houseItem->setData(0, Qt::UserRole, House);
    houseItem->setData(0, Qt::ToolTipRole, uuidHouse);

    //    Установить цвет фона/цвет текста/стиль текста
    //    houseItem->setData(0, Qt::BackgroundRole, QBrush(Qt::green));
    houseItem->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicatorWhenChildless);

    propHouse->id = uuidHouse;
    vectorHouse.append(propHouse);

    ObjectsTree->setCurrentItem(houseItem);
}
//------------------------------------------------------------------------------------

void SmartHomeConfig::addRoom()
{
    PropRoom* propRoom = new PropRoom();
    QString uuidRoom = QUuid::createUuid().toString();

    QTreeWidgetItem* houseItem = ObjectsTree->currentItem();

    if(houseItem == nullptr)
    {
        qWarning() << Q_FUNC_INFO << "The element in the tree is not selected";
        return;
    }

    if(houseItem->data(0, Qt::UserRole).toInt() != House)
    {
        qWarning() << Q_FUNC_INFO << "The element  is not House";
        return;
    }

    QTreeWidgetItem* roomItem = new QTreeWidgetItem(houseItem);
    roomItem->setData(0, Qt::DisplayRole, propRoom->name);
    roomItem->setData(0, Qt::UserRole, Room);
    roomItem->setData(0, Qt::ToolTipRole, uuidRoom);

    propRoom->id = uuidRoom;
    vectorRoom.append(propRoom);
    ObjectsTree->setCurrentItem(roomItem);
}
//------------------------------------------------------------------------------------

void SmartHomeConfig::addSensor()
{
    PropSensor* propSensor = new PropSensor();
    QString uuidSensor = QUuid::createUuid().toString();

    QTreeWidgetItem* roomItem = ObjectsTree->currentItem();

    if(roomItem == nullptr)
    {
        qWarning() << Q_FUNC_INFO << "The element in the tree is not selected";
        return;
    }

    if(roomItem->data(0, Qt::UserRole).toInt() != Room)
    {
        qWarning() << Q_FUNC_INFO << "The element  is not Room";
        return;
    }

    QTreeWidgetItem* sensorItem = new QTreeWidgetItem(roomItem);
    sensorItem->setData(0, Qt::DisplayRole, propSensor->name);
    sensorItem->setData(0, Qt::UserRole, Sensor);
    sensorItem->setData(0, Qt::ToolTipRole, uuidSensor);

    propSensor->id = uuidSensor;
    vectorSensor.append(propSensor);
    ObjectsTree->setCurrentItem(sensorItem);
}
//------------------------------------------------------------------------------------


void SmartHomeConfig::showPassport(QTreeWidgetItem* item)
{
    PassportTable->setRowCount(0);

    if(item == nullptr)
    {
        qWarning() << Q_FUNC_INFO << "The element in the tree is not selected";//??????????????????????????
        return;
    }

    int itemType = item->data(0, Qt::UserRole).toInt();

    switch(itemType)
    {
        case House:
        {
// 0 строка 0 ячейка          Наименование
            QTableWidgetItem* nameItem = new QTableWidgetItem();
            nameItem->setData(Qt::DisplayRole, "Наименование");
            PassportTable->insertRow(0);
            PassportTable->setItem(0, 0, nameItem);

            // 1 строка 0 ячейка         Адрес
            QTableWidgetItem* addressItem = new QTableWidgetItem();
            addressItem->setData(Qt::DisplayRole, "Адрес");
            PassportTable->insertRow(1);
            PassportTable->setItem(1, 0, addressItem);

            QString idHouseItem = item->data(0, Qt::ToolTipRole).toString();

            PropHouse* properties = findObjectHouse(idHouseItem);

            if(properties == nullptr)
            {
                // 0 строка 1 ячейка          Дом
                QLineEdit* nameEdit = new QLineEdit(this);
                nameEdit->setFrame(false);
                nameEdit->setText("");
                PassportTable->setCellWidget(0, 1, nameEdit);

                // 1 строка 1 ячейка         Ленина 101`
                QLineEdit* addressEdit = new QLineEdit(this);
                addressEdit->setFrame(false);
                addressEdit->setText("");
                PassportTable->setCellWidget(1, 1, addressEdit);
                break;
            }

            // 0 строка 1 ячейка          Дом
            QLineEdit* nameEdit = new QLineEdit(this);
            nameEdit->setFrame(false);
            nameEdit->setText(properties->name);
            PassportTable->setCellWidget(0, 1, nameEdit);

            // 1 строка 1 ячейка         Ленина 101`
            QLineEdit* addressEdit = new QLineEdit(this);
            addressEdit->setFrame(false);
            addressEdit->setText(properties->address);
            PassportTable->setCellWidget(1, 1, addressEdit);

            connect(nameEdit,    &QLineEdit::editingFinished, this, &SmartHomeConfig::fillNameHousePassport);
            connect(addressEdit, &QLineEdit::editingFinished, this, &SmartHomeConfig::fillAddressHousePassport);
            break;
        }
        case Room:
        {
            // 0 строка 0 ячейка          Наименование
            QTableWidgetItem* nameItem = new QTableWidgetItem();
            nameItem->setData(Qt::DisplayRole, "Наименование");
            PassportTable->insertRow(0);
            PassportTable->setItem(0, 0, nameItem);

            // 1 строка 0 ячейка         Площадь
            QTableWidgetItem* squareItem = new QTableWidgetItem();
            squareItem->setData(Qt::DisplayRole, "Площадь");
            PassportTable->insertRow(1);
            PassportTable->setItem(1, 0, squareItem);

            QString idRoomItem = item->data(0, Qt::ToolTipRole).toString();

            PropRoom* properties = findObjectRoom(idRoomItem);

            if(properties == nullptr)
            {
                // 0 строка 1 ячейка          Зал
                QLineEdit* nameEdit = new QLineEdit(this);
                nameEdit->setFrame(false);
                nameEdit->setText("");
                PassportTable->setCellWidget(0, 1, nameEdit);

                // 1 строка 1 ячейка         55м2`
                QLineEdit* squareEdit = new QLineEdit(this);
                squareEdit->setFrame(false);
                squareEdit->setText("");
                PassportTable->setCellWidget(1, 1, squareEdit);
                break;
            }

            // 0 строка 1 ячейка          Зал
            QLineEdit* nameEdit = new QLineEdit(this);
            nameEdit->setFrame(false);
            nameEdit->setText(properties->name);
            PassportTable->setCellWidget(0, 1, nameEdit);

            // 1 строка 1 ячейка         55м2`
            QLineEdit* squareEdit = new QLineEdit(this);
            squareEdit->setFrame(false);
            squareEdit->setText(properties->square);
            PassportTable->setCellWidget(1, 1, squareEdit);

            connect(nameEdit,   &QLineEdit::editingFinished, this, &SmartHomeConfig::fillNameRoomPassport);
            connect(squareEdit, &QLineEdit::editingFinished, this, &SmartHomeConfig::fillSquareRoomPassport);
            break;
        }
        case Sensor:
        {
            // 0 строка 0 ячейка          Наименование
            QTableWidgetItem* name = new QTableWidgetItem();
            name->setData(Qt::DisplayRole, "Наименование");
            PassportTable->insertRow(0);
            PassportTable->setItem(0, 0, name);

            QString idSensorItem = item->data(0, Qt::ToolTipRole).toString();

            PropSensor*properties = findObjectSensor(idSensorItem);

            if(properties == nullptr)
            {
                // 0 строка 1 ячейка          Датчик
                QLineEdit* nameEdit = new QLineEdit(this);
                nameEdit->setFrame(false);
                nameEdit->setText("");
                PassportTable->setCellWidget(0, 1, nameEdit);
                break;
            }

            QLineEdit* nameEdit = new QLineEdit(this);
            nameEdit->setFrame(false);
            nameEdit->setText(properties->name);
            PassportTable->setCellWidget(0, 1, nameEdit);

            connect(nameEdit, &QLineEdit::editingFinished, this, &SmartHomeConfig::fillNameSensorPassport);
            break;
        }
        default: break;
    }
}
//------------------------------------------------------------------------------------

void SmartHomeConfig::fillNameHousePassport()
{
    QLineEdit* nameEditPassport = qobject_cast<QLineEdit*>(sender());
    if(nameEditPassport == nullptr)
    {
        qWarning() << Q_FUNC_INFO << "Failed convert sender() to QLineEdit*";
        return;
    }

    QTreeWidgetItem* ObjectsTreeItem = ObjectsTree->currentItem();
    if(ObjectsTreeItem == nullptr)
    {
        qWarning() << Q_FUNC_INFO << "The element in the tree is not selected";
        return;
    }

    QString idHouseItem = ObjectsTreeItem->data(0, Qt::ToolTipRole).toString();

    PropHouse* house = findObjectHouse(idHouseItem);

    if(house == nullptr)
    {
        qWarning() << Q_FUNC_INFO << idHouseItem << "The element in the vectorHouse not found";
        return;
    }

    house->name = nameEditPassport->text();
    ObjectsTreeItem->setData(0, Qt::DisplayRole, house->name);
}
//------------------------------------------------------------------------------------

void SmartHomeConfig::fillAddressHousePassport()
{
    QLineEdit* addressEditPassport = qobject_cast<QLineEdit*>(sender());

    if(addressEditPassport == nullptr)
    {
        qWarning() << Q_FUNC_INFO << "Failed convert sender() to QLineEdit*";
        return;
    }

    // Запись в вектор по номеру строки корневого item
    QTreeWidgetItem* ObjectsTreeItem = ObjectsTree->currentItem();
    if(ObjectsTreeItem == nullptr)
    {
        qWarning() << Q_FUNC_INFO << "The element in the tree is not selected";
        return;
    }

    QString idHouseItem = ObjectsTreeItem->data(0, Qt::ToolTipRole).toString();

    PropHouse* house = findObjectHouse(idHouseItem);

    if(house == nullptr)
    {
        qWarning() << Q_FUNC_INFO << idHouseItem << "The elevent in the vectorHouse not found";
        return;
    }

    house->address = addressEditPassport->text();
    ObjectsTreeItem->setData(1, Qt::DisplayRole, house->address);
}
//------------------------------------------------------------------------------------

void SmartHomeConfig::fillNameRoomPassport()
{
    QLineEdit* nameEditPassport = qobject_cast<QLineEdit*>(sender());
    if(nameEditPassport == nullptr)
    {
        qWarning() << Q_FUNC_INFO << "Failed convert sender() to QLineEdit*";
        return;
    }

    QTreeWidgetItem* ObjectsTreeItem = ObjectsTree->currentItem();
    if(ObjectsTreeItem == nullptr)
    {
        qWarning() << Q_FUNC_INFO << "The element in the tree is not selected";
        return;
    }

    QString idRoomItem = ObjectsTreeItem->data(0, Qt::ToolTipRole).toString();

    PropRoom* room = findObjectRoom(idRoomItem);

    if(room == nullptr)
    {
        qWarning() << Q_FUNC_INFO << idRoomItem << "The element in thr vectorRoom not found";
        return;
    }

    room->name = nameEditPassport->text();
    ObjectsTreeItem->setData(0, Qt::DisplayRole, room->name);
}
//------------------------------------------------------------------------------------

void SmartHomeConfig::fillSquareRoomPassport()
{
    QLineEdit*squareEditPassport = qobject_cast<QLineEdit*>(sender());
    if(squareEditPassport == nullptr)
    {
        qWarning() << Q_FUNC_INFO << "Failed convert sender() to QLineEdit*";
        return;
    }

    QTreeWidgetItem* ObjectTreeItem = ObjectsTree->currentItem();
    if(ObjectTreeItem == nullptr)
    {
        qWarning() << Q_FUNC_INFO << "The element in the tree is not selected";
        return;
    }

    QString idRoomItem = ObjectTreeItem->data(0, Qt::ToolTipRole).toString();

    PropRoom* room = findObjectRoom(idRoomItem);

    if(room == nullptr)
    {
        qWarning() << Q_FUNC_INFO << idRoomItem << "The element in the vectorRoom not found";
        return;
    }

    room->square = squareEditPassport->text();
    ObjectTreeItem->setData(1, Qt::DisplayRole, room->square);
}
//------------------------------------------------------------------------------------

void SmartHomeConfig::fillNameSensorPassport()
{
    QLineEdit* nameEditPassport = qobject_cast<QLineEdit*>(sender());
    if(nameEditPassport == nullptr)
    {
        qWarning() << Q_FUNC_INFO << "Failed convert sender() to QLineEdit*";
        return;
    }

    QTreeWidgetItem* ObjectTreeItem = ObjectsTree->currentItem();
    if(ObjectTreeItem == nullptr)
    {
        qWarning() << Q_FUNC_INFO << "The element in the tree is not selected";
        return;
    }

    QString idSensorItem = ObjectTreeItem->data(0, Qt::ToolTipRole).toString();


    PropSensor* sensor = findObjectSensor(idSensorItem);

    if(sensor == nullptr)
    {
        qWarning() << Q_FUNC_INFO << idSensorItem << "The element in the vectorSensor not found";
        return;
    }

    sensor->name = nameEditPassport->text();
    ObjectTreeItem->setData(0, Qt::DisplayRole, sensor->name);
}
//------------------------------------------------------------------------------------

PropHouse* SmartHomeConfig::findObjectHouse(QString id)
{
    PropHouse* house = nullptr;

    for(int i = 0; i < vectorHouse.count(); i++)
    {
        if(vectorHouse[i]->id == id)
        {
            house = vectorHouse[i];
            break;
        }
    }

    return house;
}
//------------------------------------------------------------------------------------

PropRoom* SmartHomeConfig::findObjectRoom(QString id)
{
    PropRoom* room = nullptr;

    for(int i = 0; i < vectorRoom.count(); i++)
    {
        if(vectorRoom[i]->id == id)
        {
            room = vectorRoom[i];
            break;
        }
    }

    return room;
}
//------------------------------------------------------------------------------------

PropSensor* SmartHomeConfig::findObjectSensor(QString id)
{
    PropSensor* sensor = nullptr;

    for(int i = 0; i < vectorSensor.count(); i++)
    {
        if(vectorSensor[i]->id == id)
        {
            sensor = vectorSensor[i];
            break;
        }
    }

    return sensor;
}

//------------------------------------------------------------------------------------

void SmartHomeConfig::deleteItem()
{
    QTreeWidgetItem* item = ObjectsTree->currentItem();

    if(item == nullptr)
    {
        qWarning() << Q_FUNC_INFO << "The element in the tree is not selected";
        return;
    }

    delete item;
}
//------------------------------------------------------------------------------------

void SmartHomeConfig::activButton(QTreeWidgetItem *item, QTreeWidgetItem *previous)
{
    addRoomButton  ->setEnabled(false);
    addSensorButton->setEnabled(false);
    deleteButton   ->setEnabled(false);

    if(item == nullptr)
    {
        qWarning() << Q_FUNC_INFO << "The element in the tree is not selected";//??????????????????????????
        return;
    }

    deleteButton->setEnabled(true);

    int itemType = item->data(0, Qt::UserRole).toInt();

    switch(itemType)
    {
        case House:
        {
            addRoomButton  ->setEnabled(true);
            break;
        }
        case Room:
        {
            addSensorButton->setEnabled(true);
            break;
        }
        default: break;
    }
}
//------------------------------------------------------------------------------------




//void SmartHomeConfig::fillAddressPassportq()
//{
//    // Получить item корневого уровня по номеру строки(Дом)
//    QTreeWidgetItem* houseItem1 = ObjectsTree->topLevelItem(0);

//    // Получить дочерний item у родителя по номеру строки(Комната)
//    QTreeWidgetItem* roomeItem  = houseItem1->child(0);

//    // Получить дочерний item у родителя по номеру строки(Датчик)
//    QTreeWidgetItem* sensorItem = roomeItem->child(0);

//    // Получить номер строки когда известен корневой item
//    int numberRow = ObjectsTree->indexOfTopLevelItem(houseItem1);

//    // от item дочернего элемента получить item родителя
//    QTreeWidgetItem* _houseItem1 = roomeItem->parent();

//    // у item родителя получить номер строки дочернего элемента
//    int rowRoom = _houseItem1->indexOfChild(roomeItem);
//}
//------------------------------------------------------------------------------------
