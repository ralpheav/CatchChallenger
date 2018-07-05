#include "Api_client_virtual.h"

using namespace CatchChallenger;

#ifdef __GNUC__
    #include <utime.h>
    #include <sys/stat.h>
#endif

//need host + port here to have datapack base

Api_client_virtual::Api_client_virtual(ConnectedSocket* socket, const Settings& settings) :
    Api_protocol_2(socket, settings)
{
    mDatapackBase = settings.getAppPath() + settings.getDatapackDir();
}

Api_client_virtual::~Api_client_virtual()
{
}

void Api_client_virtual::sendDatapackContentBase(const std::string& hashBase)
{
    (void)hashBase;
    haveTheDatapack();
}

void Api_client_virtual::sendDatapackContentMainSub(const std::string& hashMain, const std::string& hashSub)
{
    (void)hashMain;
    (void)hashSub;
    haveTheDatapackMainSub();
}

void Api_client_virtual::tryDisconnect()
{
    if (socket != NULL) {
        socket->disconnectFromHost();
    }
}

//general data
void Api_client_virtual::defineMaxPlayers(const uint16_t&)
{
    //What is comming here
}
