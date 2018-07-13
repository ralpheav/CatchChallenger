#if !defined(API_CLIENT_VIRTUAL_TESTCASE_H_INCLUDED)
#define API_CLIENT_VIRTUAL_TESTCASE_H_INCLUDED

#include "cppunit/TestCase.h"
#include "cppunit/extensions/HelperMacros.h"

#include "client/base/CORE/Api_client_virtual.h"

class ApiClientVirtualTestCase : public CppUnit::TestCase
{
  CPPUNIT_TEST_SUITE(ApiClientVirtualTestCase);
      CPPUNIT_TEST(addDataTest);
      CPPUNIT_TEST(getDataTest);
  CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();

protected:
    void addDataTest();
    void getDataTest();

private:
    CatchChallenger::Api_client_virtual* apiClientVirtual;
};

#endif // API_CLIENT_VIRTUAL_TESTCASE_H_INCLUDED
