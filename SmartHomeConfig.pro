QT += gui core widgets  network
QT += sql

CONFIG += c++17
CONFIG += console

SOURCES += main.cpp\
			Arguments.cpp \
			SmartHomeConfig.cpp \
			SmartHomeConfig_DB.cpp
			
HEADERS += SmartHomeConfig.h \
    Arguments.h

RESOURCES += \
    resources/resources.qrc


DISTFILES +=

