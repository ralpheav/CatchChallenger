#include <string>

#include "../net/FakeSocket.h"
#include "../net/TCPSocket.h"
#include "../net/ConnectedSocket.h"

bool applySocket(CatchChallenger::ISocket* pSocket) {

    CatchChallenger::ConnectedSocket* con = new CatchChallenger::ConnectedSocket(pSocket);

    con->setTcpCork(false);
    con->abort();

    return true;
}


int main() {

        CatchChallenger::FakeSocket* faker = new CatchChallenger::FakeSocket();

        applySocket(faker);

        CatchChallenger::SSLSocket* ssl = new CatchChallenger::SSLSocket();

        applySocket(ssl);

        CatchChallenger::TCPSocket* tcp = new CatchChallenger::TCPSocket();

        applySocket(tcp);

	return 0;
}
