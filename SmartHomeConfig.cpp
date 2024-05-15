#include "SmartHomeConfig.h"
//------------------------------------------------------------------------------------

#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>
//------------------------------------------------------------------------------------

SmartHomeConfig::SmartHomeConfig(QWidget* parent) : QWidget(parent)
{
    setGeometry(40, 40, 500, 500);

    treeWidget = new QTreeWidget(this);

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
}
//------------------------------------------------------------------------------------

void SmartHomeConfig::addRoom()
{
    QTreeWidgetItem* houseItem = treeWidget->currentItem();

    QTreeWidgetItem* roomItem = new QTreeWidgetItem(houseItem);
    roomItem->setData(0, Qt::DisplayRole, "Кухня");
}
//------------------------------------------------------------------------------------

void SmartHomeConfig::addSensor()
{
    QTreeWidgetItem* roomItem = treeWidget->currentItem();

    QTreeWidgetItem* bathroomtemperItem = new QTreeWidgetItem(roomItem);
    bathroomtemperItem->setData(0, Qt::DisplayRole, "Датчик");

}
//------------------------------------------------------------------------------------
