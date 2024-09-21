#include "SmartHomeConfig.h"
//------------------------------------------------------------------------------------

#include <QHeaderView> //
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
#include <QThread>
#include <QTime>
#include <QFileDialog>
#include <QMessageBox>
#include <QToolBar>
#include <QAction>
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

    nextBlockSize = 0;

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

    PassportTable = new QTableWidget(this);
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

    QHBoxLayout* hLayout1 = new QHBoxLayout();
    hLayout1->addWidget(addHouseButton);
    hLayout1->addWidget(addRoomButton);
    hLayout1->addWidget(addSensorButton);
    hLayout1->addStretch();

    QHBoxLayout* hLayout2 = new QHBoxLayout();
    hLayout2->addWidget(ObjectsTree);
    hLayout2->addWidget(PassportTable);

    QToolBar* toolBar = new QToolBar(this);

    addAction = new QAction(this);
    addAction->setToolTip("Добавить");
    addAction->setIcon(QIcon(":/add.png"    ));

    removeAction = new QAction(this);
    removeAction->setToolTip("Удалить");
    removeAction->setIcon(QIcon(":/remoove.png"));

    saveAction = new QAction(this);
    saveAction->setToolTip("Сохранить");
    saveAction->setIcon(QIcon(":/save.png"));

    loadAction = new QAction(this);
    loadAction->setToolTip("Загрузить");
    loadAction->setIcon(QIcon(":/load.png"));

    clearAction = new QAction(this);
    clearAction->setToolTip("Очистить");
    clearAction->setIcon(QIcon(":/clear.png"));

    sendAction = new QAction(this);
    sendAction->setToolTip("Отправить");
    sendAction->setIcon(QIcon(":/send.png"));

    toolBar->addAction(addAction);
    toolBar->addAction(removeAction);
    toolBar->addAction(saveAction);
    toolBar->addAction(loadAction);
    toolBar->addAction(clearAction);
    toolBar->addAction(sendAction);
    toolBar->setIconSize(QSize(20, 20));
    toolBar->addSeparator();

    QVBoxLayout* vLayout = new QVBoxLayout(this);
    vLayout->addWidget(toolBar);
    vLayout->addLayout(hLayout1);
    vLayout->addLayout(hLayout2);

    socket = new QTcpSocket(this);

    activButton(nullptr);

    connect(addHouseButton,  &QPushButton::clicked,            this, &SmartHomeConfig::addHouse);
    connect(addRoomButton,   &QPushButton::clicked,            this, &SmartHomeConfig::addRoom);
    connect(addSensorButton, &QPushButton::clicked,            this, &SmartHomeConfig::addSensor);

    connect(ObjectsTree,     &QTreeWidget::currentItemChanged, this, &SmartHomeConfig::activButton);
    connect(ObjectsTree,     &QTreeWidget::currentItemChanged, this, &SmartHomeConfig::showPassport);

    connect(socket,          &QTcpSocket::readyRead,           this, &SmartHomeConfig::readyRead);
    connect(socket,          &QTcpSocket::stateChanged,        this, &SmartHomeConfig::stateChangeSocket);

    connect(sendAction,      &QAction::triggered,              this, &SmartHomeConfig::send);
    connect(saveAction,      &QAction::triggered,              this, &SmartHomeConfig::saveToFile);
    connect(loadAction,      &QAction::triggered,              this, &SmartHomeConfig::load);
    connect(clearAction,     &QAction::triggered,              this, &SmartHomeConfig::clear);
    connect(removeAction,    &QAction::triggered,              this, &SmartHomeConfig::deleteItem);
}
//------------------------------------------------------------------------------------

void SmartHomeConfig::saveToFile()
{
    QString pathOut = QFileDialog::getSaveFileName(this, "Введите имя файла конфигурации", "E:/", "", nullptr, QFileDialog::DontUseNativeDialog);

    QFile fileOut(pathOut);

    if(QFile::exists(pathOut))
    {
        QFile::remove(pathOut);
    }

    if(!fileOut.open(QIODevice::WriteOnly))
    {
        qWarning() << Q_FUNC_INFO << "E:/myHome.bin" << "not open";
        return;
    }

    QDataStream dataStream(&fileOut);
    for(PropHouse* propHouse : vectorHouse)
    {
        dataStream << quint8(House)
                   << propHouse->name
                   << propHouse->address
                   << propHouse->id;
    }

    for(PropRoom* propRoom : vectorRoom)
    {
        dataStream << quint8(Room)
                   << propRoom->name
                   << propRoom->square
                   << propRoom->countWindow
                   << propRoom->id
                   << propRoom->idHouse;
    }

    for(PropSensor* propSensor : vectorSensor)
    {
        dataStream << quint8(Sensor)
                   << propSensor->name
                   << propSensor->typeSensor
                   << propSensor->id
                   << propSensor->idRoom;
    }

    fileOut.close();
}
//------------------------------------------------------------------------------------

void SmartHomeConfig::load()
{
    clear();

    QString pathIn = QFileDialog::getOpenFileName(this, "Выберите файл конфигурации", "E:/", "*.bin", nullptr, QFileDialog::DontUseNativeDialog);

    if(pathIn.isEmpty())
    {
        qWarning() << Q_FUNC_INFO << "File: " << pathIn << "empty";
        return;
    }

    QFile fileIn(pathIn);

    if(!fileIn.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this,
                             "Ошибка",
                             QString("Не удалось открыть файл: %1").arg(pathIn),
                             QMessageBox::Close,
                             QMessageBox::Close);
        return;
    }

    QDataStream dataStream(&fileIn);

    while( dataStream.atEnd() == false )
    {
        quint8 typeObject;
        dataStream >> typeObject;

        switch (typeObject)
        {
            case House:
            {
                PropHouse*    propHouse  = new PropHouse();
                dataStream >> propHouse->name
                           >> propHouse->address
                           >> propHouse->id;
                vectorHouse.append(propHouse);
                break;
            }
            case Room:
            {
                PropRoom*     propRoom   = new PropRoom();
                dataStream >> propRoom->name
                           >> propRoom->square
                           >> propRoom->countWindow
                           >> propRoom->id
                           >> propRoom->idHouse;
                vectorRoom.append(propRoom);
                break;
            }
            case Sensor:
            {
                PropSensor*   propSensor = new PropSensor();
                dataStream >> propSensor->name
                           >> propSensor->typeSensor
                           >> propSensor->id
                           >> propSensor->idRoom;
                vectorSensor.append(propSensor);
                break;
            }
            default:
            {
                qWarning() << Q_FUNC_INFO << QString("Unknown type object: %1. Must been: House(%2) or Room(%3) or Sensor(%4)").arg(typeObject).arg(House).arg(Room).arg(Sensor);
                break;
            }
        }
    }

    fileIn.close();

    for(int i = 0; i < vectorHouse.size(); i++)
    {
        PropHouse* propHouse = vectorHouse[i];

        QTreeWidgetItem* houseItem = createHouseItem(propHouse);

        for(int j = 0; j < vectorRoom.size(); j++)
        {
            PropRoom* propRoom = vectorRoom[j];

            if(propRoom->idHouse != propHouse->id)
            {
                continue;
            }

            QTreeWidgetItem* roomItem = createRoomItem(propRoom, houseItem);

            for(int k = 0; k < vectorSensor.size(); k++)
            {
                PropSensor* propSensor = vectorSensor[k];

                if(propSensor->idRoom != propRoom->id)
                {
                    continue;
                }

                createSensorItem(propSensor, roomItem);
            }
        }
    }

    ObjectsTree->expandAll();
    QTreeWidgetItem* houseItem = ObjectsTree->topLevelItem(0);
    ObjectsTree->setCurrentItem(houseItem);
}
//------------------------------------------------------------------------------------

void SmartHomeConfig::clear()
{
    for(int i = 0; i < vectorHouse.count(); i++)
    {
        delete vectorHouse[i];
    }
    vectorHouse.clear();

    for(int i = 0; i < vectorRoom.count(); i++)
    {
        delete vectorRoom[i];
    }
    vectorRoom.clear();

    for(int i = 0; i < vectorSensor.count(); i++)
    {
        delete vectorSensor[i];
    }
    vectorSensor.clear();

    ObjectsTree->clear();
}
//------------------------------------------------------------------------------------

void SmartHomeConfig::stateChangeSocket(QAbstractSocket::SocketState socketState)
{
    switch(socketState)
    {
        case QTcpSocket::ConnectedState:
        {
            send();
            break;
        }
        default:
        {
            break;
        }
    }
}
//------------------------------------------------------------------------------------

void SmartHomeConfig::connectToServer()
{
    socket->connectToHost("127.0.0.1", 3333);
}
//------------------------------------------------------------------------------------

void SmartHomeConfig::send()
{
    if(socket->state() != QTcpSocket::ConnectedState)
    {
        connectToServer();
        return;
    }

        PropHouse* propHouse;
        for(int i = 0; i < vectorHouse.count(); i++)
        {
            propHouse = vectorHouse[i];
            sendHousesToServer(propHouse);
        }

        PropRoom* propRoom;
        for(int i = 0; i < vectorRoom.count(); i++)
        {
            propRoom = vectorRoom[i];
            sendRoomsToServer(propRoom);
        }

        PropSensor* propSensor;
        for(int i = 0; i < vectorSensor.count(); i++)
        {
            propSensor = vectorSensor[i];
            sendSensorsToServer(propSensor);
        }
    }
//------------------------------------------------------------------------------------

void SmartHomeConfig::sendHousesToServer(PropHouse* propHouse)
{
    data.clear();
    QDataStream out(&data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);
    out << quint16(0) << House << propHouse->name << propHouse->address << propHouse->id;
    qDebug() << propHouse->name << propHouse->address << propHouse->id;
    out.device()->seek(0);
    out << quint16(data.size() - sizeof(quint16));
    quint16 qint16 = quint16(data.size() - sizeof(quint16));
    qDebug() << qint16;
    socket->write(data);
}
//------------------------------------------------------------------------------------

void SmartHomeConfig::sendRoomsToServer(PropRoom* propRoom)
{
    data.clear();
    QDataStream out(&data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);
    out << quint16(0) << Room << propRoom->name << propRoom->square << propRoom->countWindow << propRoom->id;
    qDebug() << propRoom->name << propRoom->square << propRoom->countWindow << propRoom->id;
    out.device()->seek(0);
    out << quint16(data.size() - sizeof(quint16));
    socket->write(data);
}
//------------------------------------------------------------------------------------

void SmartHomeConfig::sendSensorsToServer(PropSensor* propSensor)
{
    data.clear();
    QDataStream out(&data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);
    out << quint16(0) << Sensor << propSensor->name << propSensor->typeSensor << propSensor->id;
    qDebug() << propSensor->name << propSensor->typeSensor << propSensor->id;
    out.device()->seek(0);
    out << quint16(data.size() - sizeof(quint16));
    socket->write(data);
}
//------------------------------------------------------------------------------------

void SmartHomeConfig::readyRead()
{
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_5_15);

    if(in.status() == QDataStream::Ok)
    {
        for(;;)
        {
            if(nextBlockSize == 0)
            {
                if(socket->bytesAvailable() < 2)
                {
                    break;
                }

                in >> nextBlockSize;

                if(socket->bytesAvailable() < nextBlockSize)
                {
                    break;
                }

                QString str;
                in >> str;
                nextBlockSize = 0;
                qDebug() << "from server" << str;
                break;
            }
            else
            {
                qDebug() << "send server->client error!!!!!!!!!";
            }
        }
    }
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
                if(removeAction->isEnabled())
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
    propHouse->id = QUuid::createUuid().toString();
    vectorHouse.append(propHouse);

    QTreeWidgetItem* houseItem = createHouseItem(propHouse);

    ObjectsTree->setCurrentItem(houseItem);
}
//------------------------------------------------------------------------------------

QTreeWidgetItem* SmartHomeConfig::createHouseItem(PropHouse* propHouse)
{
    QTreeWidgetItem* houseItem = new QTreeWidgetItem(ObjectsTree);
    houseItem->setData(0, Qt::DisplayRole, propHouse->name);
    houseItem->setData(0, Qt::UserRole, House);
    houseItem->setData(0, Qt::ToolTipRole, propHouse->id);

    //    Установить цвет фона/цвет текста/стиль текста
    //    houseItem->setData(0, Qt::BackgroundRole, QBrush(Qt::green));
    houseItem->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicatorWhenChildless);

    return houseItem;
}
//------------------------------------------------------------------------------------

void SmartHomeConfig::addRoom()
{
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

    PropRoom* propRoom = new PropRoom();

    propRoom->id = QUuid::createUuid().toString();
    propRoom->idHouse = houseItem->data(0, Qt::ToolTipRole).toString();
    vectorRoom.append(propRoom);

    QTreeWidgetItem* roomItem = createRoomItem(propRoom, houseItem);

    ObjectsTree->setCurrentItem(roomItem);
}
//------------------------------------------------------------------------------------

QTreeWidgetItem* SmartHomeConfig::createRoomItem(PropRoom* propRoom, QTreeWidgetItem* houseItem)
{
    QTreeWidgetItem* roomItem = new QTreeWidgetItem(houseItem);
    roomItem->setData(0, Qt::DisplayRole, propRoom->name);
    roomItem->setData(0, Qt::UserRole, Room);
    roomItem->setData(0, Qt::ToolTipRole, propRoom->id);

    return roomItem;
}
//------------------------------------------------------------------------------------

void SmartHomeConfig::addSensor()
{
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

    PropSensor* propSensor = new PropSensor();

    propSensor->id = QUuid::createUuid().toString();
    propSensor->idRoom = roomItem->data(0, Qt::ToolTipRole).toString();
    vectorSensor.append(propSensor);

    QTreeWidgetItem* sensorItem = createSensorItem(propSensor, roomItem);

    ObjectsTree->setCurrentItem(sensorItem);
}
//------------------------------------------------------------------------------------

QTreeWidgetItem* SmartHomeConfig::createSensorItem(PropSensor* propSensor, QTreeWidgetItem* roomItem)
{
    QTreeWidgetItem* sensorItem = new QTreeWidgetItem(roomItem);
    sensorItem->setData(0, Qt::DisplayRole, propSensor->name);
    sensorItem->setData(0, Qt::UserRole, Sensor);
    sensorItem->setData(0, Qt::ToolTipRole, propSensor->id);

    return sensorItem;
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
                PassportTable->setCellWidget(1, 1, squareSpinBox);

                // 2 строка 1 ячейка         кол-во окон`
                PassportTable->setCellWidget(2, 1, countWindowBox);
                break;
            }

            // 0 строка 1 ячейка          Зал
            nameEdit->setText(properties->name);
            PassportTable->setCellWidget(0, 1, nameEdit);

            // 1 строка 1 ячейка         55м2`
            squareSpinBox->setValue(properties->square);
            PassportTable->setCellWidget(1, 1, squareSpinBox);

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

            QTableWidgetItem* typeItem = new QTableWidgetItem();
            typeItem->setData(Qt::DisplayRole, "Тип");
            PassportTable->insertRow(1);
            PassportTable->setItem(1, 0, typeItem);

            QString idSensorItem = item->data(0, Qt::ToolTipRole).toString();

            PropSensor*properties = findObjectSensor(idSensorItem);

            QLineEdit* nameEdit = new QLineEdit(this);
            nameEdit->setFrame(false);
            nameEdit->setText("");

            QComboBox* cBoxSensor = new QComboBox(this);
            cBoxSensor->addItems({"Температура", "Влажность", "Дым"});
            PassportTable->setCellWidget(1, 1, cBoxSensor);

            if(properties == nullptr)
            {
//                // 0 строка 1 ячейка          Датчик
                PassportTable->setCellWidget(0, 1, nameEdit);

                // 1 строка 1 ячейка          Тип датчика
                PassportTable->setCellWidget(1, 1, cBoxSensor);
                break;
            }

            // 0 строка 1 ячейка          Датчик
            nameEdit->setText(properties->name);
            PassportTable->setCellWidget(0, 1, nameEdit);

            // 1 строка 1 ячейка          Тип датчика
            cBoxSensor->setCurrentIndex(properties->typeSensor);
            PassportTable->setCellWidget(1, 1, cBoxSensor);

            connect(nameEdit, &QLineEdit::editingFinished, this, &SmartHomeConfig::fillNameSensorPassport);
            connect( cBoxSensor, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &SmartHomeConfig::fillTypeSensorPassport );
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

void SmartHomeConfig::fillTypeSensorPassport()
{
    QComboBox* cBoxEditPassport = qobject_cast<QComboBox*>(sender());
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

    sensor->typeSensor = cBoxEditPassport->currentIndex();
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
    removeAction->setEnabled(false);
    saveAction->setEnabled(false);

    if(item == nullptr)
    {
        return;
    }

    removeAction->setEnabled(true);
    saveAction->setEnabled(true);

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
