#include "SmartHomeConfig.h"
//------------------------------------------------------------------------------------

#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>
#include <QLineEdit>
#include <QFrame>
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



//------------------------------------------------------------------------------------

void SmartHomeConfig::addHouse()
{
    QTreeWidgetItem* houseItem = new QTreeWidgetItem(ObjectsTree);
    houseItem->setData(0, Qt::DisplayRole, "Дом");
    houseItem->setData(0, Qt::UserRole, House);

    //    Установить цвет фона/цвет текста/стиль текста
    //    houseItem->setData(0, Qt::BackgroundRole, QBrush(Qt::green));
    houseItem->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicatorWhenChildless);

    ObjectsTree->setCurrentItem(houseItem);
}
//------------------------------------------------------------------------------------

void SmartHomeConfig::addRoom()
{
    QTreeWidgetItem* houseItem = ObjectsTree->currentItem();

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

    ObjectsTree->setCurrentItem(roomItem);
}
//------------------------------------------------------------------------------------

void SmartHomeConfig::addSensor()
{
    QTreeWidgetItem* roomItem = ObjectsTree->currentItem();

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

    ObjectsTree->setCurrentItem(sensorItem);
}
//------------------------------------------------------------------------------------

void SmartHomeConfig::deleteItem()
{
    QTreeWidgetItem* item = ObjectsTree->currentItem();

    if(item == nullptr)
    {
        return;
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
            QString str = item->data(0, Qt::DisplayRole).toString();

            QTableWidgetItem* name = new QTableWidgetItem();
            QTableWidgetItem* prop = new QTableWidgetItem();

            name->setData(Qt::DisplayRole, "Наименование");
            prop->setData(Qt::DisplayRole, val);

            PassportTable->insertRow(0);
            PassportTable->setItem(0, 0, name);
//            tableWidget->setItem(0, 1, prop);
            QLineEdit* lineEditPassport = new QLineEdit(this);
            lineEditPassport->setFrame(false);
            lineEditPassport->setText(str);
            PassportTable->setCellWidget(0, 1, lineEditPassport);

            connect(lineEditPassport, &QLineEdit::editingFinished, this, &SmartHomeConfig::fillPassport);

            break;
        }
        case Room:
        {
            QString str = item->data(0, Qt::DisplayRole).toString();

            QTableWidgetItem* name = new QTableWidgetItem();
//            QTableWidgetItem* prop = new QTableWidgetItem();

            name->setData(Qt::DisplayRole, "Наименование");
//            prop->setData(Qt::DisplayRole, val);

            PassportTable->insertRow(0);
            PassportTable->setItem(0, 0, name);
//            PassportTable->setItem(0, 1, prop);

            QLineEdit* lineEditPassport = new QLineEdit(this);
            lineEditPassport->setFrame(false);
            lineEditPassport->setText(str);
            PassportTable->setCellWidget(0, 1, lineEditPassport);

            connect(lineEditPassport, &QLineEdit::editingFinished, this, &SmartHomeConfig::fillPassport);

            break;
        }
        case Sensor:
        {
            QString str = item->data(0, Qt::DisplayRole).toString();

            QTableWidgetItem* name = new QTableWidgetItem();
            QTableWidgetItem* prop = new QTableWidgetItem();

            name->setData(Qt::DisplayRole, "Наименование");
            prop->setData(Qt::DisplayRole, val);

            PassportTable->insertRow(0);
            PassportTable->setItem(0, 0, name);
//            PassportTable->setItem(0, 1, prop);

            QLineEdit* lineEditPassport = new QLineEdit(this);
            lineEditPassport->setFrame(false);
            lineEditPassport->setText(str);
            PassportTable->setCellWidget(0, 1, lineEditPassport);

            connect(lineEditPassport, &QLineEdit::editingFinished, this, &SmartHomeConfig::fillPassport);

            break;
        }
        default: break;
    }
}
//------------------------------------------------------------------------------------

void SmartHomeConfig::fillPassport()
{
    QLineEdit* lEdit = qobject_cast<QLineEdit*>(sender());

    if(lEdit == nullptr)
    {
        qWarning() << Q_FUNC_INFO << lEdit << "nullptr";
        return;
    }

    QTreeWidgetItem* valItem = ObjectsTree->currentItem();
    if(valItem == nullptr)
    {
        qWarning() << Q_FUNC_INFO << valItem << "nullptr";
        return;
    }
    ObjectsTree->currentItem()->setData(0, Qt::DisplayRole, lEdit->text());
}
//------------------------------------------------------------------------------------
