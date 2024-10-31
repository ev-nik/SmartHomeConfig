#include <QStringList>


#include "Arguments.h"
//----------------------------------------------------------------------

Arguments::Arguments()
{
    isHelp = false;
}
//----------------------------------------------------------------------

Arguments::Arguments(int argc, char* argv[])
{
    isHelp = false;
    port = 0;

    for(int i = 0; i < argc; i++)
    {
        QString tmp = argv[i];

        if(tmp == "-h" || tmp == "--help")
        {
            isHelp = true;
            return;
        }

        QStringList tmp_split = tmp.split('=');

        if(tmp_split.count() != 2)
        {
            continue;
        }

        QString& key = tmp_split[0];
        QString& val = tmp_split[1];

        if(key == "-hN" || key == "--hostName")
        {
            hostName = val;
        }

        if(key == "-p" || key == "--port")
        {
            port = val.toInt();
        }

        if(key == "-uN" || key == "--userName")
        {
            userName = val;
        }

        if(key == "-pwd" || key == "--password")
        {
            pwd = val;
        }

        if(key == "-dBN" || key == "--dataBaseName")
        {
            dataBaseName = val;
        }
    }
}

Arguments::~Arguments()
{
}
