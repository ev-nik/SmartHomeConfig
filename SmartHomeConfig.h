#ifndef SMARTHOMECONFIG_H
#define SMARTHOMECONFIG_H
//------------------------------------------------------------------------------------

#include <QWidget>
#include <QTreeWidget>
#include <QPushButton>
#include <QTableWidget>
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

    int val;

public slots:
    void addHouse();
    void addRoom();
    void addSensor();

    void deleteItem();
    void activButton(QTreeWidgetItem *item, QTreeWidgetItem *previous);
    void showPassport(QTreeWidgetItem* item);

    void fillPassport();

};
//------------------------------------------------------------------------------------

#endif // SMARTHOMECONFIG_H
