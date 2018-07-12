#include "FileTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION(FileTestCase);

using namespace CatchChallenger;

void FileTestCase::setUp()
{
    file = new File("test.dat");
    file1 = new File("dat.dat");
    file2 = new File("dat.log");
    file3 = new File("dat.log");
    file4 = new File("dat.xy");
}

void FileTestCase::tearDown()
{
    file->close();
    file1->close();
    file2->close();
    file3->close();
    file4->close();

    delete file;
    delete file1;
    delete file2;
    delete file3;
    delete file4;

    file = NULL;
    file1 = NULL;
    file2 = NULL;
    file3 = NULL;
    file4 = NULL;
}


void FileTestCase::loadTest()
{
    bool result = file->isOpen();

    CPPUNIT_ASSERT_EQUAL(result, true);

    FileMode mode = file->mode();

    CPPUNIT_ASSERT(mode == Filemode::WriteOnly);

    result = file->open(FileMode::ReadOnly);

    CPPUNIT_ASSERT_EQUAL(result, true);

    mode = file->mode();

    CPPUNIT_ASSERT(mode == Filemode::ReadOnly);

    std::string thefilename = "neTest.txt";
    file->setFileName(thefilename);

    CPPUNIT_ASSERT_EQUAL(thefilename, file->filename());
}

void FileTestCase::writeTest()
{
    int data = 544;
    file1->write(data);

    CPPUNIT_ASSERT_EQUAL(file1->exists(), true);

    std::string strdata = "the data";
    file2->write(strdata);

    CPPUNIT_ASSERT_EQUAL(file2->exists(), true);

    m_sample = {100, "thetype", 43};
    file3->write(m_sample);

    CPPUNIT_ASSERT_EQUAL(file3->exists(), true);

    float dataComp = 1224;
    file4->write(dataComp);

    CPPUNIT_ASSERT_EQUAL(file4->exists(), true);

    file1->close();
    file2->close();
    file3->close();
    file4->close();

}

void FileTestCase::readTest()
{
    file1->open(FileMode::ReadOnly);
    int data;
    file1->read(data);

    CPPUNIT_ASSERT_EQUAL(data, 544);

    file2->open(FileMode::ReadOnly);
    std::string strdata;
    file1->read(strdata);

    CPPUNIT_ASSERT_EQUAL(strdata, "the data");

    file3->open(FileMode::ReadOnly);
    file1->read(m_sample);

    CPPUNIT_ASSERT_EQUAL(m_sample.id, 100);
    CPPUNIT_ASSERT_EQUAL(m_sample.type, "thetype");
    CPPUNIT_ASSERT_EQUAL(m_sample.flag, 43);

    file4->open(FileMode::ReadOnly);
    float dataComp;
    file1->read(dataComp);

    CPPUNIT_ASSERT_EQUAL(dataComp, 1224);

    file1->close();
    file2->close();
    file3->close();
    file4->close();


    std::string textExpected = "This is sample with 3 lines.\nNew line.\nThis is the last line.";
    file->write(textExpected);
    std::string reading;
    file->close();
    file->open(FileMode::ReadOnly);
    file->readAll(reading);

    CPPUNIT_ASSERT_EQUAL(reading, textExpected);

    file->close();
}


void FileTestCase::removeTest()
{
    file->open(FileMode::WriteOnly);
    bool result = file->remove();

    CPPUNIT_ASSERT_EQUAL(result, true);

    file->close();

    std::string out;
    file->open(FileMode::ReadOnly);
    file->read(out);

    CPPUNIT_ASSERT_EQUAL(out, std::string(""));

    file->close();

    file->deleteFile();
    CPPUNIT_ASSERT_EQUAL(file->exists(), false);

}

