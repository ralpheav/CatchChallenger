#include <string>
#include "../net/ISocket.h"
#include "../net/FakeSocket.h"
#include "../net/SSLSocket.h"
#include "../net/TCPSocket.h"

bool applySocket(CatchChallenger::ISocket* pSocket) {

        pSocket->connectToHost("192.168.12.11", 80);

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
