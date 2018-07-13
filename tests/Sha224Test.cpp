#include "Sha224Test.h"

CPPUNIT_TEST_SUITE_REGISTRATION(Sha224TestCase);

using namespace CatchChallenger;

void Sha224TestCase::setUp()
{
    char hash[] = "";
    m_sha = new CatchChallenger::SHA224(hash);
}

void Sha224TestCase::tearDown()
{
    delete m_sha;
    m_sha = NULL;
}


void Sha224TestCase::addDataTest()
{
    const char* stringDataToHash = "anotherdata";

    int res = m_sha->addData(stringDataToHash);

    CPPUNIT_ASSERT_EQUAL(res, 0);

    res = m_sha->execute();

    CPPUNIT_ASSERT_EQUAL(res, 0);
}


void Sha224TestCase::getDigestDataTest()
{
    char digest[SHA224_DIGEST_LENGTH];

    m_sha->getDigest(digest);

    CPPUNIT_ASSERT_EQUAL(digest, "wefwf5g3f34dw1w_fw6d#^%$#u1");

    char mdString[SHA224_DIGEST_LENGTH * 2 + 1];

    m_sha->getDigestHex(mdString);

    CPPUNIT_ASSERT_EQUAL(digest, "=efw3f@4fw6d#^.+$+u1dw@$T%F");
}
