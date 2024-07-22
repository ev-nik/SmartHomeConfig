#include "SmartHomeConfig.h"
//------------------------------------------------------------------------------------

#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>
#include <QLineEdit>
#include <QFrame>
#include <QUuid>
#include <QComboBox>
#include <QSpinBox>
#include <QApplication>
#include <QDesktopWidget>
#include <QEvent>
#include <QKeyEvent>
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
    qApp->installEventFilter(this);

    { // Отображение окна по центру экрана
        int width = 900;
        int height = 500;

        QRect screenRect = QApplication::desktop()->rect();
        int x = (screenRect.width() / 2) - (width / 2);
        int y = (screenRect.height() / 2) - (height / 2);

        setGeometry(x, y, width, height);
    }

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

    addSensorButton = new QPushButton(this);
    addSensorButton->setText("Добавить датчик");

    deleteButton = new QPushButton(this);
    deleteButton->setText("Удалить");

    activButton(nullptr);

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

bool SmartHomeConfig::eventFilter(QObject* obj, QEvent* event)
{
    if(event->type() == QEvent::KeyPress)
    {
        if(obj == ObjectsTree)
        {
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);

            if(keyEvent->key() == Qt::Key_Delete)
            {
                if(deleteButton->isEnabled())
                {
                    deleteItem();
                    return false;
                }
            }

            if(keyEvent->modifiers() == Qt::ControlModifier && keyEvent->key()==Qt::Key_N)
            {
                QTreeWidgetItem* currentItem = ObjectsTree->currentItem();

                if(currentItem == nullptr)
                {
                    addHouse();
                    return false;
                }

                int itemType = currentItem->data(0, Qt::UserRole).toInt();
                switch(itemType)
                {
                    case House:
                    {
                        addRoom();
                        break;
                    }
                    case Room:
                    {
                        addSensor();
                        break;
                    }
                    default: break;
                }
            }
        }
    }

    return false;
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

            QLineEdit* nameEdit = new QLineEdit(this);
            nameEdit->setFrame(false);
            nameEdit->setText("");

            QLineEdit* addressEdit = new QLineEdit(this);
            addressEdit->setFrame(false);
            addressEdit->setText("");

            if(properties == nullptr)
            {
                qWarning() << Q_FUNC_INFO << "The element in the vectorHouse not found";
                // 0 строка 1 ячейка          Дом
                PassportTable->setCellWidget(0, 1, nameEdit);

                // 1 строка 1 ячейка         Ленина 101`
                PassportTable->setCellWidget(1, 1, addressEdit);
                break;
            }

            // 0 строка 1 ячейка          Дом
            nameEdit->setText(properties->name);
            PassportTable->setCellWidget(0, 1, nameEdit);

            // 1 строка 1 ячейка         Ленина 101`
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

            // 2 строка 0 ячейка         Кол-во окон
            QTableWidgetItem* windowItem = new QTableWidgetItem();
            windowItem->setData(Qt::DisplayRole, "Кол-во окон");
            PassportTable->insertRow(2);
            PassportTable->setItem(2, 0, windowItem);

            QString idRoomItem = item->data(0, Qt::ToolTipRole).toString();

            PropRoom* properties = findObjectRoom(idRoomItem);

            QLineEdit* nameEdit = new QLineEdit(this);
            nameEdit->setFrame(false);
            nameEdit->setText("");

//            QLineEdit* squareEdit = new QLineEdit(this);
//            squareEdit->setFrame(false);
//            squareEdit->setText("");

             QDoubleSpinBox* squareSpinBox = new QDoubleSpinBox(this);
             squareSpinBox->setFrame(false);
             squareSpinBox->setValue(0);

            QSpinBox* countWindowBox = new QSpinBox(this);
            countWindowBox->setFrame(false);
            countWindowBox->setValue(0);

            if(properties == nullptr)
            {
                // 0 строка 1 ячейка          Зал
                PassportTable->setCellWidget(0, 1, nameEdit);

                // 1 строка 1 ячейка         55м2`
                PassportTable->setCellWidget(1, 1, /*squareEdit*/squareSpinBox);

                // 2 строка 1 ячейка         кол-во окон`
                PassportTable->setCellWidget(2, 1, countWindowBox);
                break;
            }

            // 0 строка 1 ячейка          Зал
            nameEdit->setText(properties->name);
            PassportTable->setCellWidget(0, 1, nameEdit);

            // 1 строка 1 ячейка         55м2`
            /*squareEdit*/squareSpinBox->setValue(properties->square);
            PassportTable->setCellWidget(1, 1,  /*squareEdit*/squareSpinBox);

            // 2 строка 1 ячейка         кол-во окон
            countWindowBox->setValue(properties->countWindow);
            PassportTable->setCellWidget(2, 1, countWindowBox);

            connect(nameEdit,   &QLineEdit::editingFinished, this, &SmartHomeConfig::fillNameRoomPassport);
//            connect(squareSpinBox, &QDoubleSpinBox::textChanged, this, &SmartHomeConfig::fillSquareRoomPassport);
            connect(squareSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &SmartHomeConfig::fillSquareRoomPassport);
//            connect(countWindowBox, &QSpinBox::textChanged, this, &SmartHomeConfig::fillWindowRoomPassport);
            connect( countWindowBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &SmartHomeConfig::fillWindowRoomPassport );
            break;
        }
        case Sensor:
        {
            // 0 строка 0 ячейка          Наименование
            QTableWidgetItem* name = new QTableWidgetItem();
            name->setData(Qt::DisplayRole, "Наименование");
            PassportTable->insertRow(0);
            PassportTable->setItem(0, 0, name);

            // 1 строка 0 ячейка          Наименование
            QComboBox* cBoxSensor = new QComboBox(this);
            cBoxSensor->addItems({"Температуры", "Влажности", "Дыма"});
            PassportTable->insertRow(1);
            PassportTable->setCellWidget(1, 0, cBoxSensor);

            QString idSensorItem = item->data(0, Qt::ToolTipRole).toString();

            PropSensor*properties = findObjectSensor(idSensorItem);

            QLineEdit* nameEdit = new QLineEdit(this);
            nameEdit->setFrame(false);
            nameEdit->setText("");

            QLineEdit* valSensorEdit = new QLineEdit(this);
            valSensorEdit->setFrame(false);
            valSensorEdit->setText("1");

            if(properties == nullptr)
            {
                // 0 строка 1 ячейка          Датчик
                PassportTable->setCellWidget(0, 1, nameEdit);

                // 1 строка 1 ячейка          Значение датчика
                PassportTable->setCellWidget(1, 1, valSensorEdit);
                break;
            }

            // 0 строка 1 ячейка          Датчик
            nameEdit->setText(properties->name);
            PassportTable->setCellWidget(0, 1, nameEdit);

            // 1 строка 1 ячейка          Значение датчика
            valSensorEdit->setText(properties->viewSensor);
            PassportTable->setCellWidget(1, 1, valSensorEdit);

            connect(nameEdit, &QLineEdit::editingFinished, this, &SmartHomeConfig::fillNameSensorPassport);
            connect(valSensorEdit, &QLineEdit::editingFinished, this, &SmartHomeConfig::fillViewSensorPassport);
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
    QDoubleSpinBox* squareEditPassport = qobject_cast<QDoubleSpinBox*>(sender());
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

    room->square = squareEditPassport->value();
    ObjectTreeItem->setData(1, Qt::DisplayRole, room->square);
}
//------------------------------------------------------------------------------------

void SmartHomeConfig::fillWindowRoomPassport(int count)
{
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

    room->countWindow = count;
    ObjectsTreeItem->setData(2, Qt::DisplayRole, room->countWindow);
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

void SmartHomeConfig::fillViewSensorPassport()
{
    QLineEdit* cBoxEditPassport = qobject_cast<QLineEdit*>(sender());
    if(cBoxEditPassport == nullptr)
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

    QString idSensorItem = ObjectsTreeItem->data(0, Qt::ToolTipRole).toString();

    PropSensor* sensor = findObjectSensor(idSensorItem);

    if(sensor == nullptr)
    {
        qWarning() << Q_FUNC_INFO << idSensorItem << "The element in the vectorHouse not found";
        return;
    }

    sensor->viewSensor = cBoxEditPassport->text();
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

    int itemType = item->data(0, Qt::UserRole).toInt();

    switch(itemType)
    {
        case House:
        {
            deleteHouse(item);
            break;
        }
        case Room:
        {
            deleteRoom(item);
            break;
        }
        case Sensor:
        {
            deleteSensor(item);
            break;
        }
        default:
        {
            break;
        }
    }

    delete item;
}
//------------------------------------------------------------------------------------

void SmartHomeConfig::activButton(QTreeWidgetItem *item)
{
    addRoomButton  ->setEnabled(false);
    addSensorButton->setEnabled(false);
    deleteButton   ->setEnabled(false);

    if(item == nullptr)
    {
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

void SmartHomeConfig::deleteHouse(QTreeWidgetItem* item)
{
    for(int i = 0; i < item->childCount(); i++)
    {
        deleteRoom(item->child(i));
    }

    QString idHouse = item->data(0, Qt::ToolTipRole).toString();

    PropHouse* house = nullptr;
    house = findObjectHouse(idHouse);

    if(house != nullptr)
    {
        vectorHouse.removeAll(house);
        delete house;
    }
    else
    {
        qWarning() << Q_FUNC_INFO << house << "The element is not found";
    }

}
//------------------------------------------------------------------------------------

void SmartHomeConfig::deleteRoom(QTreeWidgetItem* item)
{
    for(int i = 0; i < item->childCount(); i++)
    {
        QTreeWidgetItem* itemSensor = nullptr;
        itemSensor = item->child(i);
        deleteSensor(itemSensor);
    }

    QString idRoom = item->data(0, Qt::ToolTipRole).toString();

    PropRoom* room = findObjectRoom(idRoom);

    if(room != nullptr)
    {
        vectorRoom.removeAll(room);
        delete room;
    }
    else
    {
        qWarning() << Q_FUNC_INFO << idRoom << "The element in the vectorRoom not found";
    }
}
//------------------------------------------------------------------------------------

void SmartHomeConfig::deleteSensor(QTreeWidgetItem* item)
{
    QString idSensor = item->data(0, Qt::ToolTipRole).toString();

    PropSensor* sensor = findObjectSensor(idSensor);

    if(sensor != nullptr)
    {
        vectorSensor.removeAll(sensor);
        delete sensor;
    }
    else
    {
        qWarning() << Q_FUNC_INFO << idSensor << "The element in the vectorHouse not found";
    }
}
