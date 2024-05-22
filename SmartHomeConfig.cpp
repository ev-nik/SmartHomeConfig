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

    QVBoxLayout* vLayout = new QVBoxLayout(this);
    vLayout->addLayout(hLayout1);
    vLayout->addWidget(treeWidget);

    connect(addHouseButton,  &QPushButton::clicked,     this, &SmartHomeConfig::addHouse);
    connect(addRoomButton,   &QPushButton::clicked,     this, &SmartHomeConfig::addRoom);
    connect(addSensorButton, &QPushButton::clicked,     this, &SmartHomeConfig::addSensor);
    connect(deleteButton,    &QPushButton::clicked,     this, &SmartHomeConfig::deleteItem);

    connect(treeWidget,      &QTreeWidget::currentItemChanged, this, &SmartHomeConfig::activButton);
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

void SmartHomeConfig::deleteItem()
{
    QTreeWidgetItem* item = treeWidget->currentItem();

    if(item == nullptr)
    {
        return;
    }

    if(item->data(0, Qt::UserRole).toInt() == Room)
    {
        addSensorButton->setEnabled(false);
    }

    if(item->data(0, Qt::UserRole).toInt() == House)
    {
        addRoomButton  ->setEnabled(false);
    }

    delete item;
}
//------------------------------------------------------------------------------------

void SmartHomeConfig::activButton(QTreeWidgetItem *item, QTreeWidgetItem *previous)
{
    addRoomButton   ->setEnabled(false);
    addSensorButton ->setEnabled(false);
    deleteButton    ->setEnabled(false);

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
