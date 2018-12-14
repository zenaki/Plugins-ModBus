#include <QCoreApplication>

//#include <QDebug>
//#include "qtservice.h"

#include "controller/worker.h"

#define VERSION "0.2 beta"

worker w;

void processArgs(int argc, char **argv)
{
    bool err = true;
    if (argc < 2) {
        printf(" -dc / --data-custom DATA_CUSTOM\t: ModBus Data Custom\n\n"
               " -f / --function-code FUNCTION_CODE\t: ModBus Function Code\n"
               "  Function Code List :\n"
               "  1   : Read Coils\n"
               "       ex : -tcp -ip 127.0.0.1 -p 502 -s 1 -f 1 -str 1000 -noc 20\n"
               "  2   : Read Discrete Inputs\n"
               "       ex : -tcp -ip 127.0.0.1 -p 502 -s 1 -f 2 -str 1000 -noc 20\n"
               "  3   : Read Holding Registers\n"
               "       ex : -tcp -ip 127.0.0.1 -p 502 -s 1 -f 3 -str 1000 -noc 20 -now 2 -tc FLOAT\n"
               "       ex : -tcp -ip 127.0.0.1 -p 502 -s 1 -f 3 -str 1000 -noc 20 -now 1 -tc DEC\n"
               "  4   : Read Input Registers\n"
               "       ex : -tcp -ip 127.0.0.1 -p 502 -s 1 -f 4 -str 1000 -noc 20\n"
               "  5   : Write Single Coil\n"
               "       ex : -tcp -ip 127.0.0.1 -p 502 -s 1 -f 5 -str 1000 -w 1\n"
               "  6   : Write Single Register\n"
               "       ex : -tcp -ip 127.0.0.1 -p 502 -s 1 -f 6 -str 1000 -w 1234\n"
               "  15  : Write Multiple Coils\n"
               "       ex : -tcp -ip 127.0.0.1 -p 502 -s 1 -f 15 -str 1000 -noc 4 -w 1#0#1#0\n"
               "  16  : Write Multiple Registers\n"
               "       ex : -tcp -ip 127.0.0.1 -p 502 -s 1 -f 16 -str 1000 -noc 4 -w 1234#2341#3412#4123\n"
               "  100 : User Define (Write Multiple Register)\n"
               "       ex : -tcp -ip 127.0.0.1 -p 502 -s 1 -f 100 -dc 05#10#08#28#00#02#04#00#00#00#01\n"
               "  65 : User Define (Read Register)\n"
               "       ex : -tcp -ip 127.0.0.1 -p 502 -s 1 -f 65 -dc 05#01#08#28#00#02\n\n"
               " -h / --help\t\t\t\t: Print this help info\n\n"
               " -ip / --ip-address IP_ADDRESS\t\t: IP Address for ModBus TCP\n\n"
               " -noc / --num-of-coils NUM_OF_COILS\t: Num of Coils for ModBus Protocol (DEFAULT 1)\n\n"
               " -now / --num-of-words NUM_OF_BYTES\t: Num of Bytes for ModBus Protocol Conversion (MAX 4) (DEFAULT 1)\n\n"
               " -p / --port PORT\t\t\t: Port for ModBus TCP (DEFAULT 502)\n\n"
               " -rtu\t\t\t\t\t: ModBus RTU Mode (Not yet)\n\n"
               " -s / --slave-id SLAVE_ID\t\t: Slave Id from ModBus Device\n\n"
               " -str / --start-address START_ADDRESS\t: Start Address for ModBus Protocol\n\n"
               " -t / --timeout TIMEOUT(ms)\t\t: Timeout for end process (DEFAULT 5000ms)\n\n"
               " -tc / --type-conversion TYPE\t\t: Type will be BIN/HEX/DEC/FLOAT (DEFAULT BIN)\n\n"
               " -tcp / --modbus-tcp\t\t\t: ModBus TCP Mode\n\n"
               " -v / --version\t\t\t\t: Print version of plugins\n\n"
               " -w / --write-data\t\t\t: Write Data to ModBus\n\n"
              );
        err = false;
    } else if (argc == 2) {
        QString arg1(argv[1]);
        if (arg1 == QLatin1String("-h") ||
            arg1 == QLatin1String("--help")) {
            printf(" -dc / --data-custom DATA_CUSTOM\t: ModBus Data Custom\n\n"
                   " -f / --function-code FUNCTION_CODE\t: ModBus Function Code\n"
                   "  Function Code List :\n"
                   "  1   : Read Coils\n"
                   "       ex : -tcp -ip 127.0.0.1 -p 502 -s 1 -f 1 -str 1000 -noc 20\n"
                   "  2   : Read Discrete Inputs\n"
                   "       ex : -tcp -ip 127.0.0.1 -p 502 -s 1 -f 2 -str 1000 -noc 20\n"
                   "  3   : Read Holding Registers\n"
                   "       ex : -tcp -ip 127.0.0.1 -p 502 -s 1 -f 3 -str 1000 -noc 20 -now 2 -tc FLOAT\n"
                   "       ex : -tcp -ip 127.0.0.1 -p 502 -s 1 -f 3 -str 1000 -noc 20 -now 1 -tc DEC\n"
                   "  4   : Read Input Registers\n"
                   "       ex : -tcp -ip 127.0.0.1 -p 502 -s 1 -f 4 -str 1000 -noc 20\n"
                   "  5   : Write Single Coil\n"
                   "       ex : -tcp -ip 127.0.0.1 -p 502 -s 1 -f 5 -str 1000 -w 1\n"
                   "  6   : Write Single Register\n"
                   "       ex : -tcp -ip 127.0.0.1 -p 502 -s 1 -f 6 -str 1000 -w 1234\n"
                   "  15  : Write Multiple Coils\n"
                   "       ex : -tcp -ip 127.0.0.1 -p 502 -s 1 -f 15 -str 1000 -noc 4 -w 1#0#1#0\n"
                   "  16  : Write Multiple Registers\n"
                   "       ex : -tcp -ip 127.0.0.1 -p 502 -s 1 -f 16 -str 1000 -noc 4 -w 1234#2341#3412#4123\n"
                   "  100 : User Define (Write Multiple Register)\n"
                   "       ex : -tcp -ip 127.0.0.1 -p 502 -s 1 -f 100 -dc 05#10#08#28#00#02#04#00#00#00#01\n"
                   "  65 : User Define (Read Register)\n"
                   "       ex : -tcp -ip 127.0.0.1 -p 502 -s 1 -f 65 -dc 05#01#08#28#00#02\n\n"
                   " -h / --help\t\t\t\t: Print this help info\n\n"
                   " -ip / --ip-address IP_ADDRESS\t\t: IP Address for ModBus TCP\n\n"
                   " -noc / --num-of-coils NUM_OF_COILS\t: Num of Coils for ModBus Protocol (DEFAULT 1)\n\n"
                   " -now / --num-of-words NUM_OF_BYTES\t: Num of Bytes for ModBus Protocol Conversion (MAX 4) (DEFAULT 1)\n\n"
                   " -p / --port PORT\t\t\t: Port for ModBus TCP (DEFAULT 502)\n\n"
                   " -rtu\t\t\t\t\t: ModBus RTU Mode (Not yet)\n\n"
                   " -s / --slave-id SLAVE_ID\t\t: Slave Id from ModBus Device\n\n"
                   " -str / --start-address START_ADDRESS\t: Start Address for ModBus Protocol\n\n"
                   " -t / --timeout TIMEOUT(ms)\t\t: Timeout for end process (DEFAULT 5000ms)\n\n"
                   " -tc / --type-conversion TYPE\t\t: Type will be BIN/HEX/DEC/FLOAT (DEFAULT BIN)\n\n"
                   " -tcp / --modbus-tcp\t\t\t: ModBus TCP Mode\n\n"
                   " -v / --version\t\t\t\t: Print version of plugins\n\n"
                   " -w / --write-data\t\t\t: Write Data to ModBus\n\n"
                  );
            err = false;
        } else if (arg1 == QLatin1String("-v") ||
                 arg1 == QLatin1String("--version")) {
//            printf(" ModBus Plugins Version: %s \nPT. DaunBiru Engiinering\nwww.daunbiru.com\n\n", VERSION);
            QString compilationTime = QString("%1 %2").arg(__DATE__).arg(__TIME__);
            QString version = VERSION;
            printf("Modbus Plugin Version:  %s\nPT. DaunBiru Engineering\nwww.daunbiru.com\n\n"
                   "build on: %s (UTC+7)\n",
                   version.toUtf8().data(),
                   compilationTime.toUtf8().data());
            err = false;
        }
    } else  if (argc > 2) {
        QString ip = "", type_conversion = "", mode = "", data = "", data_custom = "";
        int port = 0, slave_id = 0, function = 0, start_address = 0, num_of_coils = 0, num_of_bytes = 0;
        for (int i = 1; i < argc; i++) {
            QString arg1(argv[i]);
            if (arg1 == QLatin1String("-dc") ||
                arg1 == QLatin1String("--data-custom")) {
                data_custom = argv[i+1];
            } else if (arg1 == QLatin1String("-f") ||
                       arg1 == QLatin1String("--function-code")) {
                function = atoi(argv[i+1]);
//                if (function != 3) function = 0;
            } else if (arg1 == QLatin1String("-ip") ||
                       arg1 == QLatin1String("--ip-address")) {
                ip = argv[i+1];
                if (ip.split(".").length() < 4) ip = "";
            } else if (arg1 == QLatin1String("-noc") ||
                       arg1 == QLatin1String("--num-of-coils")) {
                num_of_coils = atoi(argv[i+1]);
                if (num_of_coils < 0) num_of_coils = 0;
            } else if (arg1 == QLatin1String("-now") ||
                       arg1 == QLatin1String("--num-of-words")) {
                num_of_bytes = QString::fromLocal8Bit(argv[i+1]).toInt();
                if (num_of_bytes < 0 || num_of_bytes > 4) num_of_bytes = 0;
            } else if (arg1 == QLatin1String("-p") ||
                       arg1 == QLatin1String("--port")) {
                port = atoi(argv[i+1]);
                if (port < 0) port = 0;
            } else if (arg1 == QLatin1String("-rtu") ||
                      arg1 == QLatin1String("--mode-rtu")) {
                mode = "RTU";
            } else if (arg1 == QLatin1String("-s") ||
                       arg1 == QLatin1String("--slave-id")) {
                slave_id = atoi(argv[i+1]);
                if (slave_id < 0) slave_id = 0;
            } else if (arg1 == QLatin1String("-str") ||
                       arg1 == QLatin1String("--start-address")) {
                start_address = atoi(argv[i+1]);
                if (start_address < 0) start_address = 0;
            } else if (arg1 == QLatin1String("-tc") ||
                       arg1 == QLatin1String("--type-conversion")) {
                type_conversion = argv[i+1];
            } else if (arg1 == QLatin1String("-tcp") ||
                       arg1 == QLatin1String("--mode-tcp")) {
                mode = "TCP";
            } else if (arg1 == QLatin1String("-w") ||
                       arg1 == QLatin1String("--write-data")) {
                data = argv[i+1];
            }
        }

//        printf("Mode            : %s\n", mode.toLatin1().data());
//        printf("IP Address      : %s\n", ip.toLatin1().data());
//        printf("Slave ID        : %d\n", slave_id);
//        printf("Function        : %d\n", function);
//        printf("Start Address   : %d\n", start_address);
//        printf("Num of Coils    : %d\n", num_of_coils);
//        printf("Num of Bytes    : %d\n", num_of_bytes);
//        printf("Type Convertion : %s\n", type_conversion.toLatin1().data());
//        printf("Data            : %s\n", data.toLatin1().data());

        if (mode == "TCP") {
            if (!port) port = 502;
            if (!num_of_bytes) num_of_bytes = 1;
            if (!num_of_coils) num_of_coils = 1;
            if (type_conversion.isEmpty()) type_conversion = "BIN";
            if (!ip.isEmpty() && port && slave_id) {
                if (start_address >= 0) {
                    if (function == 1 || function == 2) { // Read Coils, Read Discrete Input, Read Input Register
                        w.print_result(w.request_modbus(ip, port, slave_id, function, start_address, num_of_coils, 1, "BIN", ""));
                    } else if (function == 3 || function == 4) { // Read Holding Register
                        w.print_result(w.request_modbus(ip, port, slave_id, function, start_address, num_of_coils, num_of_bytes, type_conversion, ""));
                    } else if (function == 5 || function == 6 || function == 15 || function == 16) { // Write Single Coil, Write Single Register, Write Multiple Coils, Write Multiple Registers
//                        w.print_result(w.request_modbus(ip, port, slave_id, function, start_address, num_of_coils, 1, "BIN", data));
                        w.print_result(w.request_modbus(ip, port, slave_id, function, start_address, num_of_coils, 1, "BIN", data));
                    } else if ((function == 100 || function == 65) && !data_custom.isEmpty()) { // Data Custom
                        w.print_result(w.custom_request(ip, port, slave_id, function, type_conversion, data_custom));
                    }
                    err = false;
                } else {
                    err = true;
                }
            } else {
                err = true;
            }
        } else {
            err = true;
        }
    }
    if (err) {
        printf("{\"ERR\": \"Wrong Pluggins Commands\"}\n\n");
    }
//    plugins/ModBus ./ModBus -tcp -ip 192.168.3.250 -p 502 -s 10 -f 3 -str 1024 -noc 8 -now 2 -tc HEX
//    QStringList result = w.request_modbus("192.168.3.250", 502, 10, 3, 1024, 8, 2, "FLOAT", "");
//    qDebug() << "";
//    QStringList result = w.request_modbus("192.168.3.242", 502, 3, 3, 3203, 20, 4, "DEC", "");
//    QStringList result = w.request_modbus("192.168.3.242", 502, 3, 3, 3027, 6, 2, "FLOAT", "");
//    QStringList result = w.request_modbus("192.168.3.11", 502, 21, 3, 1034, 2, 2, "FLOAT", "");

//    QStringList result = w.request_modbus("127.0.0.1", 502, 1, 1, 1000, 8, 1, "DEC", "");
//    QStringList result = w.request_modbus("127.0.0.1", 502, 1, 15, 1000, 10, 1, "DEC", "1#0#1#0");
//    w.print_result(result);

//    QStringList result = w.custom_request("127.0.0.1", 502, 1, 100, "DEC", "05#10#08#28#00#02#04#00#0A#01#02");
//    w.print_result(result);

//    return 0;
}

int timeOut(int argc, char **argv) {
    int tOut = 0;

    if (argc > 2) {
        for (int i = 1; i < argc; i++) {
            QString arg1(argv[i]);
            if (arg1 == QLatin1String("-t") || arg1 == QLatin1String("--timeout")) {
                tOut = atoi(argv[i+1]);
            }
        }
    }

//    qDebug() << "timeout = " + QString::number(tOut);

    return tOut;
}

int main(int argc, char **argv)
{
    QCoreApplication a(argc, argv);

    QTimer::singleShot(timeOut(argc, argv), &a, SLOT(quit()));
    processArgs(argc, argv);
    a.quit();

    return a.exec();
//    return result;
}
