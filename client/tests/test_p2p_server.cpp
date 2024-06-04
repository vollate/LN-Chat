#include "ClientManager.hpp"

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    ClientManager clientManager(46573);

    std::this_thread::sleep_for(std::chrono::seconds(1));

    QTcpSocket socket;
    QHostAddress address("127.0.0.1");
    socket.connectToHost(address, 46573);
    if(socket.waitForConnected(3000)) {
        std::cout << "Connected" << std::endl;
        QByteArray example = "Hello";
        socket.write(example);
        if(socket.waitForBytesWritten(3000)) {
            std::cout << "Message sent: " << example.toStdString() << std::endl;
        } else {
            std::cout << "Failed to send message: " << socket.errorString().toStdString() << std::endl;
        }
    } else {
        std::cout << "Failed to connect: " << socket.errorString().toStdString() << std::endl;
    }
    return app.exec();
}
