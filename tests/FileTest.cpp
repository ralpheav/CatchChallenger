#include "FileTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION(FileTestCase);

void FileTestCase::setUp()
{
    file = new File("test.dat");
}

void FileTestCase::tearDown()
{
    delete file;
    file = NULL;
}


void FileTestCase::loadTest()
{
    // our load test logic
    bool result = file->open(FileMode::WriteOnly);

    CPPUNIT_ASSERT_EQUAL(result, true);
}


void FileTestCase::storeTest()
{
    std::string out = file->filename();

    CPPUNIT_ASSERT_EQUAL(out, std::string("test.dat"));
}
