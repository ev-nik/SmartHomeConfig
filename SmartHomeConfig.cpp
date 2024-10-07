#include "SmartHomeConfig.h"
//------------------------------------------------------------------------------------

#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>
#include <QLineEdit>
#include <QUuid>
#include <QComboBox>
#include <QSpinBox>
#include <QApplication>
#include <QDesktopWidget>
#include <QEvent>
#include <QKeyEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QToolBar>
#include <QAction>
#include <QDesktopServices>
#include <QFileInfo>
//------------------------------------------------------------------------------------

#define EXT_SCH ".shc"
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

    isRequestSend = false;
    nextBlockSize = 0;

    { // Отображение окна по центру экрана
        int width = 900;
        int height = 500;

        QRect screenRect = QApplication::desktop()->rect();
        int x = (screenRect.width() / 2) - (width / 2);
        int y = (screenRect.height() / 2) - (height / 2);

        setGeometry(x, y, width, height);
    }

    this->setWindowTitle("Конфигуратор умного дома");
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

    QHBoxLayout* hLayout1 = new QHBoxLayout();
    hLayout1->addWidget(ObjectsTree);
    hLayout1->addWidget(PassportTable);

    addActionHouse = new QAction(this);
    addActionHouse->setText("Добавить дом");
    addActionHouse->setIcon(QIcon(":/add.png"));
    addActionHouse->setToolTip("Добавить");

    addActionRoom = new QAction(this);
    addActionRoom->setText("Добавить комнату");
    addActionRoom->setIcon(QIcon(":/add.png"));
    addActionRoom->setToolTip("Добавить");

    addActionSensor = new QAction(this);
    addActionSensor->setText("Добавить датчик");
    addActionSensor->setIcon(QIcon(":/add.png"));
    addActionSensor->setToolTip("Добавить");

    QMenu* addMenu = new QMenu(this);
    addMenu->addAction(addActionHouse);
    addMenu->addAction(addActionRoom);
    addMenu->addAction(addActionSensor);

    removeAction = new QAction(this);
    removeAction->setToolTip("Удалить");
    removeAction->setIcon(QIcon(":/remoove.png"));
    removeAction->setText("Удалить");

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

    QToolButton* addButton = new QToolButton(this);
    addButton->setPopupMode(QToolButton::InstantPopup);
    addButton->setToolTip("Добавить");
    addButton->setIcon(QIcon(":/add.png"));
    addButton->setMenu(addMenu);

    QToolBar* toolBar = new QToolBar(this);
    toolBar->addWidget(addButton);
    toolBar->addAction(removeAction);
    toolBar->addSeparator();
    toolBar->addAction(saveAction);
    toolBar->addAction(loadAction);
    toolBar->addAction(clearAction);
    toolBar->addAction(sendAction);
    toolBar->setIconSize(QSize(20, 20));
    toolBar->addSeparator();

    contextMenu = new QMenu(this);
    contextMenu->addAction(addActionHouse);
    contextMenu->addAction(addActionRoom);
    contextMenu->addAction(addActionSensor);
    contextMenu->addAction(removeAction);

    QVBoxLayout* vLayout = new QVBoxLayout(this);
    vLayout->addWidget(toolBar);
    vLayout->addLayout(hLayout1);

    socket = new QTcpSocket(this);

    activButton(nullptr);

    ObjectsTree->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ObjectsTree, &QWidget::customContextMenuRequested, this, &SmartHomeConfig::showContextMenu);

    connect(addActionHouse,  &QAction::triggered,              this, &SmartHomeConfig::addHouse);
    connect(addActionRoom,   &QAction::triggered,              this, &SmartHomeConfig::addRoom);
    connect(addActionSensor, &QAction::triggered,              this, &SmartHomeConfig::addSensor);

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

void SmartHomeConfig::messageOfUnconectedToServer()
{
    QMessageBox::warning(this,
                         "Предупреждение",
                         QString("Подключение к серверу не установлено"),
                         QMessageBox::Close);
}
//------------------------------------------------------------------------------------

void SmartHomeConfig::showContextMenu(const QPoint& pos)
{
    contextMenu->popup(mapToGlobal(pos));
}
//------------------------------------------------------------------------------------

void SmartHomeConfig::saveToFile()
{
    QString pathOut = QFileDialog::getSaveFileName(this,
                                                   "Введите имя файла конфигурации",
                                                   QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/" + "Конфигурация умного дома",
                                                   QString("*%1").arg(EXT_SCH));

    if(pathOut.isEmpty())
    {
        return;
    }

    if(!pathOut.endsWith(EXT_SCH))
    {
        pathOut += EXT_SCH;
    }

    QFile fileOut(pathOut);

    if(QFile::exists(pathOut))
    {
        QFile::remove(pathOut);
    }

    if(!fileOut.open(QIODevice::WriteOnly))
    {
        return;
    }

    QDataStream out(&fileOut);
    for(PropHouse* propHouse : vectorHouse)
    {
        out << *propHouse;
    }

    for(PropRoom* propRoom : vectorRoom)
    {
        out << *propRoom;
    }

    for(PropSensor* propSensor : vectorSensor)
    {
        out << *propSensor;
    }

    fileOut.close();
}
//------------------------------------------------------------------------------------

void SmartHomeConfig::load()
{
    QString pathIn = QFileDialog::getOpenFileName(this,
                                                  "Выберите файл конфигурации",
                                                  QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),
                                                  QString("*%1").arg(EXT_SCH));

    if(pathIn.isEmpty())
    {
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

    clear();

    QDataStream in(&fileIn);

    while( in.atEnd() == false )
    {
        quint8 typeObject;
        in >> typeObject;

        switch (typeObject)
        {
            case House:
            {
                PropHouse*    propHouse  = new PropHouse();
                in >> *propHouse;
                vectorHouse.append(propHouse);
                break;
            }
            case Room:
            {
                PropRoom*     propRoom   = new PropRoom();
                in >> *propRoom;
                vectorRoom.append(propRoom);
                break;
            }
            case Sensor:
            {
                PropSensor*   propSensor = new PropSensor();
                in >> *propSensor;
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
        case QTcpSocket::UnconnectedState:
        {
            if(isRequestSend)
            {
                messageOfUnconectedToServer();
            }
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
    isRequestSend = true; // устанавливаем признак что нужно отправить данные.
                          // Если вдруг к серверу не удастся подключиться, то тогда будет выдана ошибка.
                          // см. stateChangeSocket() case QTcpSocket::UnconnectedState:

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

    isRequestSend = false; // снимаем признак на отправку данных, т.к. они успешно отправлены,
                           // чтобы не выдавать ошибку в см. stateChangeSocket() case QTcpSocket::UnconnectedState:
                           // если вдруг сервер отключился.
}
//------------------------------------------------------------------------------------

void SmartHomeConfig::sendHousesToServer(PropHouse* propHouse)
{
    data.clear();
    QDataStream out(&data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);
    out << quint16(0) << *propHouse;
    out.device()->seek(0);
    out << quint16(data.size() - sizeof(quint16));
    socket->write(data);
}
//------------------------------------------------------------------------------------

void SmartHomeConfig::sendRoomsToServer(PropRoom* propRoom)
{
    data.clear();
    QDataStream out(&data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);
    out << quint16(0) << *propRoom;
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
    out << quint16(0) << *propSensor;
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
                // 0 строка 1 ячейка          Датчик
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
    QMessageBox::StandardButton msBox = QMessageBox::question(this,
                                                            "Удаление",
                                                            QString("Вы действительно хотите удалить?"),
                                                            QMessageBox::Yes | QMessageBox::No,
                                                            QMessageBox::No);

    if(msBox == QMessageBox::No)
    {
        return;
    }

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
    addActionRoom->setEnabled(false);
    addActionSensor->setEnabled(false);

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
            addActionRoom->setEnabled(true);
            break;
        }
        case Room:
        {
            addActionSensor->setEnabled(true);
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
//------------------------------------------------------------------------------------

QDataStream& operator >> (QDataStream& in,  PropHouse& propHouse)
{
    in >> propHouse.name
       >> propHouse.address
       >> propHouse.id;

    return in;
}

QDataStream& operator << (QDataStream& out, const PropHouse& propHouse)
{
    out << quint8(House)
        << propHouse.name
        << propHouse.address
        << propHouse.id;

    return out;
}
//------------------------------------------------------------------------------------

QDataStream& operator >> (QDataStream& in,        PropRoom& propRoom)
{
    in >> propRoom.name
       >> propRoom.square
       >> propRoom.countWindow
       >> propRoom.id
       >> propRoom.idHouse;

    return in;
}

QDataStream& operator << (QDataStream& out, const PropRoom& propRoom)
{
    out << quint8(Room)
        << propRoom.name
        << propRoom.square
        << propRoom.countWindow
        << propRoom.id
        << propRoom.idHouse;

    return out;
}
//------------------------------------------------------------------------------------

QDataStream& operator >> (QDataStream& in,        PropSensor& propSensor)
{
    in >> propSensor.name
       >> propSensor.typeSensor
       >> propSensor.id
       >> propSensor.idRoom;

    return in;
}

QDataStream& operator << (QDataStream& out, const PropSensor& propSensor)
{
    out << quint8(Sensor)
        << propSensor.name
        << propSensor.typeSensor
        << propSensor.id
        << propSensor.idRoom;

    return out;
}
//------------------------------------------------------------------------------------
