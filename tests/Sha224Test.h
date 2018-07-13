#if !defined(SHA224_TESTCASE_H_INCLUDED)
#define SHA224_TESTCASE_H_INCLUDED

#include "cppunit/TestCase.h"
#include "cppunit/extensions/HelperMacros.h"

#include "general/base/protocol/util/sha224.h"

class Sha224TestCase : public CppUnit::TestCase
{
  CPPUNIT_TEST_SUITE(Sha224TestCase);
      CPPUNIT_TEST(addDataTest);
      CPPUNIT_TEST(getDigestDataTest);
  CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();

protected:
    void addDataTest();
    void getDigestDataTest();

private:
    CatchChallenger::SHA224* m_sha;
};

#endif // SHA224_TESTCASE_H_INCLUDED
