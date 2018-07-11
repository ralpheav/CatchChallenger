#include "DirTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION(DirTestCase);

void DirTestCase::setUp()
{
    dir = new CatchChallenger::Dir();

    dirpath = new CatchChallenger::Dir("/home/");
}

void DirTestCase::tearDown()
{
    delete dir;
    dir = NULL;

    delete dirpath;
    dirpath = NULL;
}


void DirTestCase::loadTest()
{
    dir->mkpath();

    dirpath->mkpath("/home/ralph/CatchChallengerRalph");

    CPPUNIT_ASSERT_EQUAL(dir->path(), std::string("/home/ralph/CatchChallengerRalph"));

    CPPUNIT_ASSERT_EQUAL(dirpath->path(), std::string("/home/ralph/CatchChallengerRalph"));

    CPPUNIT_ASSERT_EQUAL(dir->absolutePath(), std::string("/home/ralph/CatchChallengerRalph"));

    CPPUNIT_ASSERT_EQUAL(dirpath->currentPath(), std::string("/home/ralph/CatchChallengerRalph"));

}


void DirTestCase::storeTest()
{
    std::string path = CatchChallenger::Dir::appPath();

    CPPUNIT_ASSERT_EQUAL(path, std::string("/"));
}
