#include "LocalClientHandlerWithoutSender.h"
#include "LocalClientHandler.h"
#include "GlobalServerData.h"

using namespace CatchChallenger;

LocalClientHandlerWithoutSender LocalClientHandlerWithoutSender::localClientHandlerWithoutSender;

LocalClientHandlerWithoutSender::LocalClientHandlerWithoutSender()
{
}

LocalClientHandlerWithoutSender::~LocalClientHandlerWithoutSender()
{
}

void LocalClientHandlerWithoutSender::doAllAction()
{
    if(GlobalServerData::serverSettings.database.secondToPositionSync>0)
    {
        int index=0;
        const int &list_size=allClient.size();
        while(index<list_size)
        {
            static_cast<LocalClientHandler*>(allClient.at(index))->savePosition();
            index++;
        }
    }
}