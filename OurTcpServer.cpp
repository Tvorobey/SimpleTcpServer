#include "OurTcpServer.h"
#include<CLoggerHandler/CLoggerHandler.h>

#include <Mqtt/Topics/IncarnetInspector/CIncarnetInspectorStateTopic.h>

QScopedPointer<OurTcpServer> OurTcpServer::m_instance;
std::once_flag OurTcpServer::m_onceFlag;

OurTcpServer *OurTcpServer::getInstance()
{
    std::call_once(OurTcpServer::m_onceFlag, []()
    {
       m_instance.reset(new OurTcpServer());
    });

    return m_instance.data();
}

OurTcpServer::OurTcpServer(QObject *parent)
    : QObject(parent),
      m_clientHandler(new OurTcpClientHandler())
{
    connect(m_clientHandler.data(), &OurTcpClientHandler::transactionCounterUpdate,
            this, &OurTcpServer::transactionCounterUpdate);
}

void OurTcpServer::startServer(int listenPort)
{
    m_server = new QTcpServer(this);

    if (!m_server->listen(QHostAddress::Any, listenPort))
        PRINT_LOG_MESSAGE_ERROR("Ошибка запуска сервера: " + m_server->errorString());
    else
        PRINT_CONSOLE_MESSAGE_INFO("Server start on port: " + QString::number(listenPort));

    connect(m_server, &QTcpServer::newConnection, this, &OurTcpServer::onNewConnection);
}

void OurTcpServer::stopServer()
{
    m_clients.clear();
}

void OurTcpServer::readDataSlot()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());

    int descriptor = static_cast<int>(socket->socketDescriptor());

    m_clientHandler->parseMessage(m_clients[descriptor]->read(m_clients[descriptor]->bytesAvailable()));
}

void OurTcpServer::disconnectClientSlot()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());

    int descriptor = static_cast<int>(socket->socketDescriptor());

    m_clients.remove(descriptor);
}

void OurTcpServer::onNewConnection()
{
    PRINT_CONSOLE_MESSAGE_INFO("Have new connection");

    QTcpSocket* socket = m_server->nextPendingConnection();

    int descriptor = static_cast<int>(socket->socketDescriptor());

    m_clients[descriptor] = socket;
    connect(m_clients[descriptor], &QTcpSocket::readyRead, this, &OurTcpServer::readDataSlot);
    connect(m_clients[descriptor], &QTcpSocket::disconnected, this, &QTcpSocket::deleteLater);
    connect(m_clients[descriptor], &QTcpSocket::disconnected, this, &OurTcpServer::disconnectClientSlot);

}
