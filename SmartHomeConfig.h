#ifndef SMARTHOMECONFIG_H
#define SMARTHOMECONFIG_H
//------------------------------------------------------------------------------------

#include <QWidget>
#include <QTreeWidget>
#include <QPushButton>
#include <QTableWidget>
#include <QComboBox>
#include <QTcpSocket>
#include <QToolButton>
#include <QMenu>
#include <QFile>
#include <QDataStream>

#include <QSqlDatabase>
#include <QtSql>


//------------------------------------------------------------------------------------
struct PropHouse
{
    QString name;
    QString address;
    QString id;

    PropHouse()
    {
        name = "Дом";
    }

    friend QDataStream& operator << (QDataStream& out, const PropHouse& propHouse);
    friend QDataStream& operator >> (QDataStream& in,        PropHouse& propHouse);
};
//------------------------------------------------------------------------------------

struct PropRoom
{
    QString name;
    double square;
    int countWindow;
    QString id;
    QString idHouse;

    PropRoom()
    {
        name = "Комната";
        square = 0;
        countWindow = 0;
    }

    friend QDataStream& operator << (QDataStream& out, const PropRoom& propRoom);
    friend QDataStream& operator >> (QDataStream& in,        PropRoom& propRoom);
};
//------------------------------------------------------------------------------------

struct PropSensor
{
    QString name;
    int typeSensor;
    QString id;
    QString idRoom;

    PropSensor()
    {
        name = "Датчик";
        typeSensor = 0;
    }

    friend QDataStream& operator << (QDataStream& out, const PropSensor& propSensor);
    friend QDataStream& operator >> (QDataStream& in,        PropSensor& propSensor);

};
//------------------------------------------------------------------------------------

class SmartHomeConfig : public QWidget
{
    Q_OBJECT

public:
    SmartHomeConfig(QWidget* parent = nullptr);
    ~SmartHomeConfig();

private:
    QTreeWidget*  ObjectsTree;
    QTableWidget* PassportTable;

    QAction* addActionHouse;
    QAction* addActionRoom;
    QAction* addActionSensor;
    QAction* removeAction;
    QAction* saveAction;
    QAction* loadAction;
    QAction* clearAction;
    QAction* sendAction;

    QMenu* contextMenu;

public:
    QVector<PropHouse*> vectorHouse;
    QVector<PropRoom*>  vectorRoom;
    QVector<PropSensor*>vectorSensor;

    PropHouse*  findObjectHouse (QString id);
    PropRoom*   findObjectRoom  (QString id);
    PropSensor* findObjectSensor(QString id);

    void deleteHouse(QTreeWidgetItem* item);
    void deleteRoom(QTreeWidgetItem* item);
    void deleteSensor(QTreeWidgetItem* item);

public slots:
    void addHouse();
    void addRoom();
    void addSensor();

    void deleteItem();
    void activButton(QTreeWidgetItem*  items);
    void showPassport(QTreeWidgetItem* item);

    void fillNameHousePassport();
    void fillAddressHousePassport();

    void fillNameRoomPassport();
    void fillSquareRoomPassport();
    void fillWindowRoomPassport(int count);

    void fillNameSensorPassport();
    void fillTypeSensorPassport();
    /////////////////////
    void reloadHousesFromDB();
    bool insertHouseTable(PropHouse* propHouse);
    void updateNameHouseTable(PropHouse* propHouse);
    void updateAddressHouseTable(PropHouse* propHouse);


    void reloadRoomsFromDB(QTreeWidgetItem* houseItem);
    bool insertRoomTable(PropRoom* propRoom);
    void updateNameRoomTable(PropRoom* propRoom);
    void updateSquareRoomTable(PropRoom* propRoom);
    void updateWindowRoomTable(PropRoom* propRoom);


    void reloadSensorsFromDB(QTreeWidgetItem* roomItem);
    bool insertSensorTable(PropSensor* propSensor);
    void updateNameSensorTable(PropSensor* propSensor);
    void updateTypeSensorTable(PropSensor* propSensor);




protected:
    bool eventFilter(QObject* obj, QEvent* event);

private:
    QTcpSocket* socket;
    QByteArray data;
    quint16 nextBlockSize;

    bool isRequestSend;// запрос на отправку

    void showContextMenu(const QPoint& pos);

    void sendToServer(QString str);

    void sendHousesToServer (PropHouse*  propHouse);
    void sendRoomsToServer  (PropRoom*   propRoom);
    void sendSensorsToServer(PropSensor* propSensor);
    void send();

    void saveToFile();
    void load();
    void clear();
    void messageOfUnconectedToServer();

    QTreeWidgetItem* createHouseItem(PropHouse* propHouse);
    QTreeWidgetItem* createRoomItem(PropRoom* propRoom, QTreeWidgetItem* houseItem);
    QTreeWidgetItem* createSensorItem(PropSensor* sensor, QTreeWidgetItem* roomItem);

private slots:
    void connectToServer();
    void stateChangeSocket(QAbstractSocket::SocketState socketState);

public slots:
    void readyRead();

private:
    QSqlDatabase* dbase;

public:
    void init(QSqlDatabase* dbase);
};
//------------------------------------------------------------------------------------
#endif // SMARTHOMECONFIG_H
