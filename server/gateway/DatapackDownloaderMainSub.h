#ifndef CATCHCHALLENGER_DatapackDownloaderMainSub_H
#define CATCHCHALLENGER_DatapackDownloaderMainSub_H

#include "../../general/base/GeneralVariable.h"
#include "../../client/base/DatapackChecksum.h"

#include <QObject>
#include <QString>
#include <QCoreApplication>
#include <QAbstractSocket>
#include <QString>
#include <QByteArray>
#include <QList>
#include <QPair>
#include <QDir>
#include <QHash>
#include <QFileInfo>
#include <QDateTime>
#include <QCryptographicHash>
#include <QNetworkAccessManager>
#include <QNetworkProxy>
#include <QRegularExpression>

#include <curl/curl.h>

#include "../../general/base/DebugClass.h"
#include "../../general/base/GeneralStructures.h"
#include "../../client/base/qt-tar-xz/QXzDecodeThread.h"

namespace CatchChallenger {
class DatapackDownloaderMainSub
{
public:
    explicit DatapackDownloaderMainSub(const QString &mDatapackBase, const QString &mainDatapackCode, const QString &subDatapackCode);
    virtual ~DatapackDownloaderMainSub();
    static QHash<QString,QHash<QString,DatapackDownloaderMainSub *> > datapackDownloaderMainSub;
    std::vector<void *> clientInSuspend;
    void datapackDownloadError();
    void resetAll();
    void datapackFileList(const char * const data,const unsigned int &size);
    void writeNewFileToRoute(const QString &fileName, const QByteArray &data);

    //datapack related
    void sendDatapackContentMainSub();
    void sendDatapackContentMain();
    void sendDatapackContentSub();
    void test_mirror_main();
    void test_mirror_sub();
    void httpErrorEventMain();
    void httpErrorEventSub();
    void decodedIsFinishMain();
    void decodedIsFinishSub();
    bool mirrorTryNextMain();
    bool mirrorTryNextSub();
    void httpFinishedForDatapackListMain(const QByteArray data=QByteArray());
    void httpFinishedForDatapackListSub(const QByteArray data=QByteArray());
    const QStringList listDatapackMain(QString suffix);
    const QStringList listDatapackSub(QString suffix);
    void cleanDatapackMain(QString suffix);
    void cleanDatapackSub(QString suffix);

    const QString mDatapackBase;
    const QString mDatapackMain;
    QString mDatapackSub;
    QByteArray hashMain;
    QByteArray hashSub;
    QByteArray sendedHashMain;
    QByteArray sendedHashSub;
    static QSet<QString> extensionAllowed;
    static QString commandUpdateDatapackMain;
    static QString commandUpdateDatapackSub;
private:
    static QRegularExpression regex_DATAPACK_FILE_REGEX;
    /// \todo group into one thread by change for queue
    QXzDecodeThread xzDecodeThreadMain;
    QXzDecodeThread xzDecodeThreadSub;
    bool datapackTarXzMain;
    bool datapackTarXzSub;
    CatchChallenger::DatapackChecksum datapackChecksum;
    int index_mirror_main;
    int index_mirror_sub;
    static QRegularExpression excludePathMain;
    enum DatapackStatus
    {
        Main=0x02,
        Sub=0x03
    };
    DatapackStatus datapackStatus;

    bool wait_datapack_content_main;
    bool wait_datapack_content_sub;
    QStringList datapackFilesListMain;
    QStringList datapackFilesListSub;
    QList<quint32> partialHashListMain;
    QList<quint32> partialHashListSub;
    static QString text_slash;
    static QString text_dotcoma;
    bool httpError;
    bool httpModeMain;
    bool httpModeSub;
    const QString mainDatapackCode;
    const QString subDatapackCode;
    CURL *curl;
private:
    bool getHttpFileMain(const QString &url, const QString &fileName);
    bool getHttpFileSub(const QString &url, const QString &fileName);
private slots:
    void writeNewFileMain(const QString &fileName, const QByteArray &data);
    void writeNewFileSub(const QString &fileName, const QByteArray &data);
    void checkIfContinueOrFinished();
    void haveTheDatapackMainSub();
    void datapackDownloadFinishedMain();
    void datapackDownloadFinishedSub();
    void datapackChecksumDoneMain(const QStringList &datapackFilesList,const QByteArray &hash, const QList<quint32> &partialHash);
    void datapackChecksumDoneSub(const QStringList &datapackFilesList,const QByteArray &hash, const QList<quint32> &partialHash);
};
}

#endif // Protocol_and_data_H