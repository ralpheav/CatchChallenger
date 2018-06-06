#include "Api_client_virtual.h"

using namespace CatchChallenger;

#ifdef Q_CC_GNU
//this next header is needed to change file time/date under gcc
#include <utime.h>
#include <sys/stat.h>
#endif

//need host + port here to have datapack base

Api_client_virtual::Api_client_virtual(ConnectedSocket *socket, const Settings& settings) :
    Api_protocol(socket)
{
    mDatapackBase = settings.getAppPath() + settings.getDatapackDir();
}

Api_client_virtual::~Api_client_virtual()
{
}

void Api_client_virtual::sendDatapackContentBase(const std::string &hashBase)
{
    Q_UNUSED(hashBase);
    haveTheDatapack();
}

void Api_client_virtual::sendDatapackContentMainSub(const std::string &hashMain,const std::string &hashSub)
{
    Q_UNUSED(hashMain);
    Q_UNUSED(hashSub);
    haveTheDatapackMainSub();
}

void Api_client_virtual::tryDisconnect()
{
    if (socket != NULL) {
        socket->disconnectFromHost();
    }
}

//general data
void Api_client_virtual::defineMaxPlayers(const uint16_t &)
{
}
