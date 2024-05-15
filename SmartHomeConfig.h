#ifndef SMARTHOMECONFIG_H
#define SMARTHOMECONFIG_H

#include <QWidget>
#include <QTreeWidget>
#include <QPushButton>

class SmartHomeConfig : public QWidget
{
    Q_OBJECT

public:
    SmartHomeConfig(QWidget* parent = nullptr);

private:
    QTreeWidget* treeWidget;
    QPushButton* addHomeButton;
    QPushButton* addRoomButton;
    QPushButton* addSensorButton;

public slots:
    void addHome();
    void addRoom();
    void addSensor();




};

#endif // SMARTHOMECONFIG_H
