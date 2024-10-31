#ifndef ARGUMENTS_H
#define ARGUMENTS_H

#endif // ARGUMENTS_H

#include <QString>

struct Arguments
{
public:
    Arguments();
    Arguments(int argc, char* argv[]);
    ~Arguments();

    QString hostName;
    QString userName;
    QString pwd;
    QString dataBaseName;
    bool isHelp;
    int port;
};
