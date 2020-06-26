#ifndef OURTCPSERVER_H
#define OURTCPSERVER_H

#include "OurTcpClientHandler.h"

//QT
#include <QObject>
#include <QScopedPointer>
#include <QTcpServer>
#include <QTcpSocket>
#include <QPointer>
#include <QMap>

//Local
#include "Mqtt/CMqttTopic.h"

//STD
#include <mutex>


class OurTcpServerPrivate;

class OurTcpServer : public QObject
{
    Q_OBJECT
public:
    static OurTcpServer * getInstance();

public slots:
    void startServer(int listenPort);
    void stopServer();

    void readDataSlot();

    void disconnectClientSlot();

    void onNewConnection();
signals:
    void transactionCounterUpdate(uint8_t total);
    void publishToMqtt(const CMqttTopic &, bool );

private:
    explicit OurTcpServer( QObject *parent = nullptr );
    OurTcpServer(const OurTcpServer&) = delete;
    OurTcpServer& operator = (const OurTcpServer) = delete;

private:
    static std::once_flag m_onceFlag;
    static QScopedPointer<OurTcpServer>    m_instance;

    QScopedPointer<OurTcpClientHandler> m_clientHandler;

    QTcpServer* m_server;
    QMap<int, QTcpSocket*>  m_clients;
};

#endif // OURTCPSERVER_H
