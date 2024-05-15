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

    addHomeButton   = new QPushButton();
    addHomeButton->setText("Добавить дом");

    addRoomButton   = new QPushButton();
    addRoomButton->setText("Добавить комнату");

    addSensorButton = new QPushButton();
    addSensorButton->setText("Добавить датчик");

    QHBoxLayout* hLayout = new QHBoxLayout();
    hLayout->addWidget(addHomeButton);
    hLayout->addWidget(addRoomButton);
    hLayout->addWidget(addSensorButton);
    hLayout->addStretch();

    QVBoxLayout* vLayout = new QVBoxLayout(this);
    vLayout->addLayout(hLayout);
    vLayout->addWidget(treeWidget);

    connect(addHomeButton,   &QPushButton::clicked, this, &SmartHomeConfig::addHome);
    connect(addRoomButton,   &QPushButton::clicked, this, &SmartHomeConfig::addRoom);
    connect(addSensorButton, &QPushButton::clicked, this, &SmartHomeConfig::addSensor);
}
//------------------------------------------------------------------------------------

void SmartHomeConfig::addHome()
{
    QTreeWidgetItem* homeItem = new QTreeWidgetItem(treeWidget);
    homeItem->setData(0, Qt::DisplayRole, "Дом");
}
//------------------------------------------------------------------------------------

void SmartHomeConfig::addRoom()
{
    QTreeWidgetItem* homeItem = treeWidget->currentItem();

    QTreeWidgetItem* kitchenItem = new QTreeWidgetItem(homeItem);
    kitchenItem->setData(0, Qt::DisplayRole, "Кухня");
}
//------------------------------------------------------------------------------------

void SmartHomeConfig::addSensor()
{
    QTreeWidgetItem* roomItem = treeWidget->currentItem();

    QTreeWidgetItem* bathroomtemperItem = new QTreeWidgetItem(roomItem);
    bathroomtemperItem->setData(0, Qt::DisplayRole, "Датчик температуры");
}
//------------------------------------------------------------------------------------
