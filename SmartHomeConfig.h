#ifndef SMARTHOMECONFIG_H
#define SMARTHOMECONFIG_H
//------------------------------------------------------------------------------------

#include <QWidget>
#include <QTreeWidget>
#include <QPushButton>
//------------------------------------------------------------------------------------

class SmartHomeConfig : public QWidget
{
    Q_OBJECT

public:
    SmartHomeConfig(QWidget* parent = nullptr);

private:
    QTreeWidget* treeWidget;
    QPushButton* addHouseButton;
    QPushButton* addRoomButton;
    QPushButton* addSensorButton;

public slots:
    void addHouse();
    void addRoom();
    void addSensor();
};
//------------------------------------------------------------------------------------

#endif // SMARTHOMECONFIG_H
