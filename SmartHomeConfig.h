#ifndef SMARTHOMECONFIG_H
#define SMARTHOMECONFIG_H
//------------------------------------------------------------------------------------

#include <QWidget>
#include <QTreeWidget>
#include <QPushButton>
#include <QTableWidget>
//#include <QVector>
//------------------------------------------------------------------------------------

struct PropRoom
{
    QString name;
    QString square;
    QString id;

    PropRoom()
    {
        name = "Зал";
    }
};

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

struct PropSensor
{
    QString name;
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
    QVector<PropHouse*>  vectorHouse;
    QVector<PropRoom*>   vectorRoom;
    QVector<PropSensor*> vectorSensor;

public slots:
    void addHouse();
    void addRoom();
    void addSensor();

    void deleteItem();
    void activButton(QTreeWidgetItem*  item, QTreeWidgetItem *previous);
    void showPassport(QTreeWidgetItem* item);

    void fillNameHousePassport();
    void fillAddressHousePassport();

    void fillNameRoomPassport();
    void fillSquareRoomPassport();

    void fillNameSensorPassport();



    void fillAddressPassportq();
};
//------------------------------------------------------------------------------------
#endif // SMARTHOMECONFIG_H
