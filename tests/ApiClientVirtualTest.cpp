#include "ApiClientVirtualTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION(ApiClientVirtualTestCase);

using namespace CatchChallenger;

void ApiClientVirtualTestCase::setUp()
{
    ConnectedSocket* socket;
    Settings settings;
    apiClientVirtual = new Api_client_virtual(socket, settings);
}

void ApiClientVirtualTestCase::tearDown()
{
    delete apiClientVirtual;
    apiClientVirtual = NULL;
}


void ApiClientVirtualTestCase::addDataTest()
{
    //TODO
}


void ApiClientVirtualTestCase::geDataTest()
{
    //TODO
}
