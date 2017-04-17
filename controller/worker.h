#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QDateTime>
#include <QTimer>

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

#include <qmath.h>
#include <errno.h>

#include "imodbus.h"

class worker
{
public:
    worker();
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
protected:
    void releaseTcpModbus();
    void connectTcpModbus(const QString &address, int portNbr);
};

#endif // WORKER_H
