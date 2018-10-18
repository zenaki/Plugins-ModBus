#include "worker.h"

worker::worker()
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

void worker::print_result(QStringList result)
{
    QJsonArray resultArray;
    QJsonObject resultObject;

    if (result.at(0) != "ERR") {
        for (int i = 0; i < result.length(); i+=2) {
            resultObject["value"] = result.at(i);
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
