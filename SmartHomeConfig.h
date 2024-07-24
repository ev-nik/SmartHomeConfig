#ifndef SMARTHOMECONFIG_H
#define SMARTHOMECONFIG_H
//------------------------------------------------------------------------------------

#include <QWidget>
#include <QTreeWidget>
#include <QPushButton>
#include <QTableWidget>
#include <QComboBox>
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
};

struct PropRoom
{
    QString name;
    double square;
    int countWindow;
    QString id;

    PropRoom()
    {
        square = 0;
        name = "Комната";
    }
};

struct PropSensor
{
    QString name;
    int typeSensor;
    QString id;

    PropSensor()
    {
        name = "Датчик";
    }
};
//------------------------------------------------------------------------------------

class SmartHomeConfig : public QWidget
{
    Q_OBJECT

public:
    SmartHomeConfig(QWidget* parent = nullptr);

private:
    QTreeWidget*  ObjectsTree;
    QTableWidget* PassportTable;

    QPushButton* addHouseButton;
    QPushButton* addRoomButton;
    QPushButton* addSensorButton;

    QPushButton* deleteButton;

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

protected:
    bool eventFilter(QObject* obj, QEvent* event);
};
//------------------------------------------------------------------------------------
#endif // SMARTHOMECONFIG_H
