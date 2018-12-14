#include "worker.h"

worker::worker(QObject *parent) : QObject(parent)
{

}

QStringList worker::request_modbus(
        QString ip,
        int port,
        int slave_id,
        int function_code,
        int start_address,
        int num_of_coils,
        int num_of_byte,
        QString type_conversion,
        QString data_write
        )
{
    QStringList result;
    connectTcpModbus(ip, port);
    if (!m_tcpModbus) {
        result.append("ERR");
        result.append("ModBus Not Connected");
        return result;
    }

    const int slave = slave_id;
    const int func = function_code;
    const int addr = start_address;
    int num = num_of_coils;
    const int byte = num_of_byte;
    QString type = type_conversion;

    uint8_t dest[1024];
    uint16_t *dest16 = (uint16_t *) dest;

    memset( dest, 0, 1024 );

    int ret = -1;
    bool is16Bit = false;
    bool writeAccess = false;

    modbus_set_slave( m_tcpModbus, slave );

    switch( func )
    {
        case MODBUS_FC_READ_COILS:
            ret = modbus_read_bits( m_tcpModbus, addr, num, dest );
            break;
        case MODBUS_FC_READ_DISCRETE_INPUTS:
            ret = modbus_read_input_bits( m_tcpModbus, addr, num, dest );
            break;
        case MODBUS_FC_READ_HOLDING_REGISTERS:
            ret = modbus_read_registers( m_tcpModbus, addr, num, dest16 );
            is16Bit = true;
            break;
        case MODBUS_FC_READ_INPUT_REGISTERS:
            ret = modbus_read_input_registers( m_tcpModbus, addr, num, dest16 );
            is16Bit = true;
            break;
        case MODBUS_FC_WRITE_SINGLE_COIL:
            ret = modbus_write_bit( m_tcpModbus, addr,
                    data_write.toInt(0,0)?1:0);
            writeAccess = true;
            num = 1;
            break;
        case MODBUS_FC_WRITE_SINGLE_REGISTER:
            ret = modbus_write_register( m_tcpModbus, addr,
                    data_write.toInt(0,0));
            writeAccess = true;
            num = 1;
            break;
        case MODBUS_FC_WRITE_MULTIPLE_COILS:
        {
            uint8_t * data = new uint8_t[num];
            QStringList dt = data_write.split("#");
            for( int i = 0; i < num; ++i )
            {
                if (num <= dt.length()) {
                    data[i] = dt.at(i).toInt(0,0);
                } else {
                    data[i] = 0;
                }
            }
            ret = modbus_write_bits( m_tcpModbus, addr, num, data );
            delete[] data;
            writeAccess = true;
            break;
        }
        case MODBUS_FC_WRITE_MULTIPLE_REGISTERS:
        {
            uint16_t * data = new uint16_t[num];
            QStringList dt = data_write.split("#");
            for( int i = 0; i < num; ++i )
            {
                if (num <= dt.length()) {
                    data[i] = dt.at(i).toInt(0,0);
                } else {
                    data[i] = 0;
                }
            }
            ret = modbus_write_registers( m_tcpModbus, addr, num, data );
            delete[] data;
            writeAccess = true;
            break;
        }
        default:
            break;
    }

    QJsonArray resultArray;
    QJsonObject resultObject;

    if( ret == num  )
    {
        if( writeAccess )
        {
            resultObject["monita"] = resultArray;
            resultObject["success"] = "true";
            QJsonDocument resultDocument(resultObject);
            QString strJson(resultDocument.toJson(QJsonDocument::Compact));
            printf("%s\n\n", strJson.toLatin1().data());
            resultObject.remove("monita");
            resultObject.remove("success");
        }
        else
        {
            QString data_int;
            QString data_hex;
            int data_before;
            QString data_result;
            QString data_temp1; QString data_temp2;
            for( int i = 1; i < num+1; ++i )
            {
                int data = is16Bit ? dest16[(i-1)] : dest[(i-1)];
                data_int.sprintf("%d", data);
                data_hex.sprintf("0x%04x", data);
//                result.append(data_hex);
                if (i % byte == 0) {
                    if (byte == 1) {
                        data_temp2.sprintf("%04x", data);
                    } else if (byte == 2) {
                        data_temp2.sprintf("%04x%04x", data_before, data);
                    } else if (byte == 4) {
                        data_temp1.sprintf("%04x", data);
                        data_temp2.append(data_temp1);
                    }
                    bool ok;
                    if (type == "FLOAT") {
                        unsigned int d_hex = data_temp2.toUInt(&ok, 16);
                        float data_float = (*(float *) &d_hex);
                        data_result = QString::number(data_float, 'f', 5);
                        data_before = 0;
                        data_temp2.clear();

                    } else if (type == "DEC") {
                        data_result = QString::number(data_temp2.toLongLong(&ok, 16));
                        data_before = 0;
                        data_temp2.clear();
                    } else if (type == "HEX") {
                        data_result = data_temp2;
                        data_before = 0;
                        data_temp2.clear();
                    } else if (type == "BIN") {
                        data_result = QString::number(data_temp2.toUtf8().toLongLong(&ok, 16),2);
                        data_before = 0;
                        data_temp2.clear();
                    }
                } else {
                    if (byte == 4) {
                        data_temp1.sprintf("%04x", data);
                        data_temp2.append(data_temp1);
                    } else if (byte == 2) {
                        data_before = data;
                        data_result.clear();
                    }
                }

                if (!data_result.isEmpty()) {
                    result.append(data_result);
                    result.append(QString::number(QDateTime::currentMSecsSinceEpoch()));
                }
            }
        }
    } else {
        if ( ret < 0 ) {
            if (
#ifdef WIN32
                errno == WSAETIMEDOUT ||
#endif
                errno == EIO
                    ){
                releaseTcpModbus();
            } else {
                releaseTcpModbus();
            }
        } else {
            releaseTcpModbus();
        }
        resultObject["monita"] = resultArray;
        resultObject["success"] = "true";
        QJsonDocument resultDocument(resultObject);
        QString strJson(resultDocument.toJson(QJsonDocument::Compact));
        printf("%s\n\n", strJson.toLatin1().data());
        resultObject.remove("monita");
        resultObject.remove("success");
    }
    return result;
}

QStringList worker::custom_request(
        QString ip,
        int port,
        int slave_id,
        int function,
        QString type_conversion,
        QString data_custom
        )
{
    socket = new QTcpSocket(this);
    socket->connectToHost(ip, port);

    QStringList result;

    if(socket->waitForConnected(1000)) {
//        qDebug() << "Connected!";

        socket_count++;
        QByteArray BA;
        /*Modbus Header*/
        BA[0] = socket_count >> 8;
        BA[1] = socket_count && 0xFF;
        BA[2] = 0x00;
        BA[3] = 0x00;
        BA[4] = 0x00;
        BA[5] = 0x08;
        /*ID*/
    //    BA[6] = 0x10;
        BA[6] = slave_id;
        /*Func Code*/
    //    BA[7] = 0x05;
        BA[7] = function;
    //    /*Address*/
    //    BA[8] = 0x00;
    //    BA[9] = 0x00;
    //    /*data*/
    //    BA[10] = 0xFF;
    //    BA[11] = 0x00;
        QStringList data = data_custom.split("#");
        for (int i = 0; i < data.length(); i++) {
            bool bHex = false;
            BA.append(data.at(i).toUInt(&bHex,16));
        }

//        qDebug() << BA.toHex();
        writeData(BA);

        if(socket->state() == QAbstractSocket::ConnectedState)
        {
            if(socket->waitForReadyRead() == false)
            {
                result.append("ERR");
                result.append("No Data From the Source");
                return result;
            }
    //        qDebug() << "Return";
    //        qDebug() << socket->readAll().toHex();
            QString data = socket->readAll().toHex();
            QString id; id.append(data.at(12)).append(data.at(13));
            QString fc; fc.append(data.at(14)).append(data.at(15));
            QString slv_id; slv_id.append(data.at(16)).append(data.at(17));
            QString slv_fc; slv_fc.append(data.at(18)).append(data.at(19));
            QString dt_l; dt_l.append(data.at(20)).append(data.at(21));
            QStringList dt;
            for (int i = 22; i < data.size(); i+=4) {
                QString temp; temp.append(data.at(i)).append(data.at(i+1)).append(data.at(i+2)).append(data.at(i+3));
                dt.append(temp);
            }

            bool ok;
            QJsonObject resultObjectMaster;
            QJsonObject resultObjectSlave;
            QJsonArray  resultArraySlave;
            resultObjectMaster["id"] = QString::number(id.toLongLong(&ok, 16));
            resultObjectMaster["fc"] = QString::number(fc.toLongLong(&ok, 16));
            resultObjectSlave["id"] = QString::number(slv_id.toLongLong(&ok, 16));
            resultObjectSlave["fc"] = QString::number(slv_fc.toLongLong(&ok, 16));
            resultObjectSlave["dt_l"] = QString::number(dt_l.toLongLong(&ok, 16));
//            qDebug() << type_conversion;
            for (int i = 0; i < dt.length(); i++) {
                if (type_conversion == "FLOAT") {
                    unsigned int d_hex = dt.at(i).toUInt(&ok, 16);
                    float data_float = (*(float *) &d_hex);
                    dt[i] = QString::number(data_float, 'f', 5);
                } else if (type_conversion == "DEC") {
                    dt[i] = QString::number(dt.at(i).toLongLong(&ok, 16));
                } else if (type_conversion == "BIN") {
                    dt[i] = QString::number(dt.at(i).toUtf8().toLongLong(&ok, 16),2);
                }
                resultArraySlave.append(dt.at(i));
            }
            resultObjectSlave["dt"] = resultArraySlave;
            resultObjectMaster["slv"] = resultObjectSlave;
            QJsonDocument resultDocumentMaster(resultObjectMaster);
            QString strJson(resultDocumentMaster.toJson(QJsonDocument::Compact));

//            printf("%s\n\n", strJson.toLatin1().data());
            result.append(strJson);
            result.append(QString::number(QDateTime::currentMSecsSinceEpoch()));

        }
    } else {
//        qDebug() << "Not connected!";
        result.append("ERR");
        result.append("Cannot Connect From the Source");
    }

//    socket->disconnectFromHost();
    return result;
}

void worker::print_result(QStringList result)
{
    QJsonArray resultArray;
    QJsonObject resultObject;

    if (result.at(0) != "ERR") {
        for (int i = 0; i < result.length(); i+=2) {
            QJsonDocument jsonResult = QJsonDocument::fromJson(result.at(i).toUtf8());
            if (!jsonResult.object().isEmpty()) {
                resultObject["value"] = jsonResult.object();
            } else {
                resultObject["value"] = result.at(i);
            }
            resultObject["epochtime"] = result.at(i+1);
            resultArray.append(resultObject);
            resultObject.remove("value");
            resultObject.remove("epochtime");
        }
        resultObject["monita"] = resultArray;
        resultObject["success"] = "true";
        QJsonDocument resultDocument(resultObject);
        QString strJson(resultDocument.toJson(QJsonDocument::Compact));
        printf("%s\n\n", strJson.toLatin1().data());
        resultObject.remove("monita");
        resultObject.remove("success");
    } else {
        QString strJson("{\"" + result.at(0) + "\": \"" + result.at(1) + "\", \"success\": \"false\"}");
        printf("%s\n\n", strJson.toLatin1().data());
    }

//    printf("\n\n");
//    int cnt = 1;
//    for (int i = 0; i < result.length(); i+=2) {
//        printf("%i: %s - %s\n", cnt, result.at(i).toLatin1().data(), result.at(i+1).toLatin1().data());
//        cnt++;
    //    }
}

void worker::releaseTcpModbus()
{
    if ( m_tcpModbus ) {
        modbus_close( m_tcpModbus );
        modbus_free( m_tcpModbus );
        m_tcpModbus = NULL;
    }
}

void worker::connectTcpModbus(const QString &address, int portNbr)
{
    releaseTcpModbus();

    m_tcpModbus = modbus_new_tcp( address.toLatin1().constData(), portNbr );
    if ( modbus_connect( m_tcpModbus ) == -1 )
    {
        releaseTcpModbus();
    }
}

bool worker::writeData(QByteArray data)
{
    if(socket->state() == QAbstractSocket::ConnectedState)
    {
//        socket->write(IntToArray(data.size())); //write size of data
        socket->write(data); //write the data itself
        return socket->waitForBytesWritten();
    }
    else
    {
        return false;
    }
}
