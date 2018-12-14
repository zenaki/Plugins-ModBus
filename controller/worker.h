#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QDateTime>
#include <QTimer>
#include <QTcpSocket>

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

#include <qmath.h>
#include <errno.h>

#include "imodbus.h"

class worker : public QObject
{
    Q_OBJECT
public:
    explicit worker(QObject *parent = 0);
    modbus_t *m_tcpModbus;

    QStringList request_modbus(
            QString ip,
            int port,
            int slave_id,
            int function,
            int start_address,
            int num_of_coils,
            int num_of_byte,
            QString type_conversion,
            QString data_write
            );
    void print_result(QStringList result);

    QTcpSocket *socket;
    int socket_count;
    QStringList custom_request(
            QString ip,
            int port,
            int slave_id,
            int function,
            QString type_conversion,
            QString data_custom
            );
protected:
    void releaseTcpModbus();
    void connectTcpModbus(const QString &address, int portNbr);
private:
    bool writeData(QByteArray data);
};

#endif // WORKER_H
