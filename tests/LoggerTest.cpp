#include "LoggerTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION(LoggerTestCase);

void LoggerTestCase::setUp()
{
}

void LoggerTestCase::tearDown()
{
}

void LoggerTestCase::LoggerInstanceTest()
{
    Logger& instance = Logger::instance();

    CPPUNIT_ASSERT(strcmp(typeid(instance).name(),"Logger") == 0);
}

void LoggerTestCase::logTest()
{
    Logger::instance().log(Logger::Debug, "this is the debug");
    std::ifstream logFile("catchlog.txt");
    std::string content1((std::istreambuf_iterator<char>(logFile)), (std::istreambuf_iterator<char>()));
    std::string aux(content1.c_str());

    CPPUNIT_ASSERT_EQUAL(aux, std::string("debug: this is the debug"));
    logFile.close();
    std::remove("catchlog.txt");

    Logger::instance().log(Logger::Info, "this is the info");
    logFile.open("catchlog.txt");
    std::string content2((std::istreambuf_iterator<char>(logFile)), (std::istreambuf_iterator<char>()));
    std::string aux2(content2.c_str());

    CPPUNIT_ASSERT_EQUAL(aux2, std::string("info: this is the info"));
    logFile.close();
    std::remove("catchlog.txt");

    Logger::instance().log(Logger::Error, "this is the error");
    logFile.open("catchlog.txt");
    std::string content3((std::istreambuf_iterator<char>(logFile)), (std::istreambuf_iterator<char>()));
    std::string aux3(content3.c_str());

    CPPUNIT_ASSERT_EQUAL(aux3, std::string("error: this is the error"));
    logFile.close();
    std::remove("catchlog.txt");

    std::vector<std::string> lines;

    lines.push_back("log line 1");
    lines.push_back("log line 2");
    lines.push_back("log line 3");

    Logger::instance().log(Logger::Error, lines);
    logFile.open("catchlog.txt");
    std::string content4((std::istreambuf_iterator<char>(logFile)), (std::istreambuf_iterator<char>()));
    std::string aux4(content4.c_str());

    CPPUNIT_ASSERT_EQUAL(aux4, std::string("error: log line 1\nerror: log line 2\nerror: log line 3"));
    logFile.close();
    std::remove("catchlog.txt");

}


void LoggerTestCase::logHelperTest()
{
    //TODO: test for protected function helper

    //Logger::instance().logHelper(Logger::Debug, "help");
    //CPPUNIT_ASSERT_EQUAL();

}

