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

    QHBoxLayout* hLayout = new QHBoxLayout();
    hLayout->addWidget(addHouseButton);
    hLayout->addWidget(addRoomButton);
    hLayout->addWidget(addSensorButton);
    hLayout->addStretch();

    QVBoxLayout* vLayout = new QVBoxLayout(this);
    vLayout->addLayout(hLayout);
    vLayout->addWidget(treeWidget);

    connect(addHouseButton,  &QPushButton::clicked, this, &SmartHomeConfig::addHouse);
    connect(addRoomButton,   &QPushButton::clicked, this, &SmartHomeConfig::addRoom);
    connect(addSensorButton, &QPushButton::clicked, this, &SmartHomeConfig::addSensor);
}
//------------------------------------------------------------------------------------

void SmartHomeConfig::addHouse()
{
    QTreeWidgetItem* houseItem = new QTreeWidgetItem(treeWidget);
    houseItem->setData(0, Qt::DisplayRole, "Дом");
    houseItem->setData(0, Qt::UserRole, 1);

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

    if(houseItem->data(0, Qt::UserRole).toInt() != 1)
    {
        return;
    }

    QTreeWidgetItem* roomItem = new QTreeWidgetItem(houseItem);
    roomItem->setData(0, Qt::DisplayRole, "Комната");

    // Задать признак(по признаку можно определить что за item)
    roomItem->setData(0, Qt::UserRole, 2);

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

    if(roomItem->data(0, Qt::UserRole).toInt() != 2)
    {
        return;
    }

    QTreeWidgetItem* sensorItem = new QTreeWidgetItem(roomItem);
    sensorItem->setData(0, Qt::DisplayRole, "Датчик");
    sensorItem->setData(0, Qt::UserRole, 3);

    treeWidget->setCurrentItem(sensorItem);
}
//------------------------------------------------------------------------------------
