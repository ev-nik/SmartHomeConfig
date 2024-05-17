#include "SmartHomeConfig.h"
//------------------------------------------------------------------------------------

#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>
//------------------------------------------------------------------------------------

enum
{
    House  = 1,
    Room   = 2,
    Sensor = 3
};
//------------------------------------------------------------------------------------

SmartHomeConfig::SmartHomeConfig(QWidget* parent) : QWidget(parent)
{
    setGeometry(40, 40, 500, 500);

    treeWidget = new QTreeWidget(this);
    treeWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    QHeaderView* header = treeWidget->header();
    header->hide();

    addHouseButton   = new QPushButton();
    addHouseButton->setText("Добавить дом");

    addRoomButton   = new QPushButton();
    addRoomButton->setText("Добавить комнату");

    addSensorButton = new QPushButton();
    addSensorButton->setText("Добавить датчик");

    deleteButton = new QPushButton();
    deleteButton->setText("Удалить");

    QHBoxLayout* hLayout1 = new QHBoxLayout();
    hLayout1->addWidget(addHouseButton);
    hLayout1->addWidget(addRoomButton);
    hLayout1->addWidget(addSensorButton);
    hLayout1->addWidget(deleteButton);
    hLayout1->addStretch();

    QVBoxLayout* vLayout = new QVBoxLayout(this);
    vLayout->addLayout(hLayout1);
    vLayout->addWidget(treeWidget);

    connect(addHouseButton,  &QPushButton::clicked, this, &SmartHomeConfig::addHouse);
    connect(addRoomButton,   &QPushButton::clicked, this, &SmartHomeConfig::addRoom);
    connect(addSensorButton, &QPushButton::clicked, this, &SmartHomeConfig::addSensor);

    connect(deleteButton, &QPushButton::clicked, this, &SmartHomeConfig::deleteHouse);
    connect(deleteButton, &QPushButton::clicked, this, &SmartHomeConfig::deleteRoom);
    connect(deleteButton, &QPushButton::clicked, this, &SmartHomeConfig::deleteSensor);
}
//------------------------------------------------------------------------------------

void SmartHomeConfig::addHouse()
{
    QTreeWidgetItem* houseItem = new QTreeWidgetItem(treeWidget);
    houseItem->setData(0, Qt::DisplayRole, "Дом");
    houseItem->setData(0, Qt::UserRole, House);

//    Установить цвет фона/цвет текста/стиль текста
//    houseItem->setData(0, Qt::BackgroundRole, QBrush(Qt::green));

    houseItem->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicatorWhenChildless);

    treeWidget->setCurrentItem(houseItem);
}
//------------------------------------------------------------------------------------

void SmartHomeConfig::addRoom()
{
    QTreeWidgetItem* houseItem = treeWidget->currentItem();

    if(houseItem == nullptr)
    {
        return;
    }

    if(houseItem->data(0, Qt::UserRole).toInt() != House)
    {
        return;
    }

    QTreeWidgetItem* roomItem = new QTreeWidgetItem(houseItem);
    roomItem->setData(0, Qt::DisplayRole, "Комната");

    // Задать признак(по признаку можно определить что за item)
    roomItem->setData(0, Qt::UserRole, Room);

    treeWidget->setCurrentItem(roomItem);
}
//------------------------------------------------------------------------------------

void SmartHomeConfig::addSensor()
{
    QTreeWidgetItem* roomItem = treeWidget->currentItem();

    if(roomItem == nullptr)
    {
        return;
    }

    if(roomItem->data(0, Qt::UserRole).toInt() != Room)
    {
        return;
    }

    QTreeWidgetItem* sensorItem = new QTreeWidgetItem(roomItem);
    sensorItem->setData(0, Qt::DisplayRole, "Датчик");
    sensorItem->setData(0, Qt::UserRole, Sensor);

    treeWidget->setCurrentItem(sensorItem);
}
//------------------------------------------------------------------------------------

void SmartHomeConfig::deleteHouse()
{
    QTreeWidgetItem* houseItem = treeWidget->currentItem();

    if(houseItem == nullptr)
    {
        return;
    }

    if(houseItem->data(0, Qt::UserRole).toInt() != House)
    {
        return;
    }

    delete houseItem;
}
//------------------------------------------------------------------------------------

void SmartHomeConfig::deleteRoom()
{
    QTreeWidgetItem* roomItem = treeWidget->currentItem();

    if(roomItem == nullptr)
    {
        return;
    }

    if(roomItem->data(0, Qt::UserRole).toInt() != Room)
    {
        return;
    }

    delete roomItem;
}
//------------------------------------------------------------------------------------

void SmartHomeConfig::deleteSensor()
{
    QTreeWidgetItem* sensorItem = treeWidget->currentItem();

    if(sensorItem == nullptr)
    {
        return;
    }

    if(sensorItem->data(0, Qt::UserRole).toInt() != Sensor)
    {
        return;
    }

//    treeWidget->removeItemWidget(sensorItem, 0);

    delete sensorItem;
}






























