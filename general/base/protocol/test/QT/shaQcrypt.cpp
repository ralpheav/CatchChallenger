#include <iostream>
#include <string>
#include <QString>
#include <QCryptographicHash>

#include <QByteArray>
#include <QDataStream>

enum Chat_type : uint8_t
{
    Chat_type_local            = 0x01,
    Chat_type_all              = 0x02,
    Chat_type_clan             = 0x03,
    Chat_type_aliance          = 0x04,
    Chat_type_pm               = 0x06,
    Chat_type_system           = 0x07,
    Chat_type_system_important = 0x08
};

 
int main() {
    std::string login = "root";
    std::string pass = "root";
    std::string token = "otro";
    std::string passHash;
    std::string outputdata;

    QString cadena = QString::fromStdString(pass) + "AwjDvPIzfJPTTgHs" + QString::fromStdString(login);


    QCryptographicHash hashPass(QCryptographicHash::Sha224);
    hashPass.addData(cadena.toUtf8());

    passHash = std::string(hashPass.result().data(), hashPass.result().size());
    std::cout << "passhash result: " << hashPass.result().toHex().data() << std::endl;
    std::cout << "passhash result: " << passHash << std::endl;



    QCryptographicHash hashAndToken(QCryptographicHash::Sha224);
    hashAndToken.addData(QByteArray(passHash.data(), passHash.size()));
    hashAndToken.addData(QByteArray(token.data(), token.size()));

    outputdata = std::string(hashAndToken.result().data(), hashAndToken.result().size());
    std::cout << "result: " << hashAndToken.result().toHex().data() << std::endl;
    std::cout << "result: " << outputdata << std::endl;

    const std::string text = "this is a test";
    Chat_type chatType = Chat_type::Chat_type_clan;
    QByteArray outputData;
    QDataStream out(&outputData, QIODevice::ReadWrite);
    out.setVersion(QDataStream::Qt_4_4);
    out.setByteOrder(QDataStream::LittleEndian);
    out << (uint8_t)chatType;

        const std::string& tempText = text;
        if (tempText.size() > 255)
        {
            std::cerr << "text in Utf8 too big, line: " << __FILE__ << ": " << __LINE__ << std::endl;
            return -1;
        }
        out << (uint8_t)tempText.size();
        outputData += QByteArray(tempText.data(), tempText.size());
        //out.device()->seek(out.device()->pos() + tempText.size());


    uint32_t data;
    std::cout<<"datastream test:"<<std::endl;
    out >> data;
    std::cout<<"datastream test:"<<std::endl;
    std::cout <<"data: "<< std::hex << data << std::endl;
    std::cout <<"outdata:" <<std::hex << outputData.data() << std::endl;

    out.device()->seek(out.device()->pos() + tempText.size());//LIKE FLUSH

    out >> data;
    std::cout <<"after data: "<< std::hex << data << std::endl;
 
    return 0;
}
