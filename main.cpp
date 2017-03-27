//#include <QCoreApplication>

//#include "qtservice.h"

#include "controller/worker.h"

#define VERSION "0.1 beta"

worker w;

int processArgs(int argc, char **argv)
{
    bool err = true;
    if (argc < 2) {
        printf(" -f / --function-code FUNCTION_CODE\t: ModBus Function Code\n"
               "  Function Code List :\n"
               "  1  : Read Coils (Not yet)\n"
               "  2  : Read Discrete Inputs (Not yet)\n"
               "  3  : Read Holding Registers\n"
               "  ex : -tcp -ip 192.168.3.100 -p 502 -s 1 -f 3 -str 1000 -noc 20 -nob 2 -t FLOAT\n"
               "  4  : Read Input Registers (Not yet)\n"
               "  5  : Write Single Coil (Not yet)\n"
               "  6  : Write Single Register (Not yet)\n"
               "  15 : Write Multiple Coils (Not yet)\n"
               "  16 : Write Multiple Registers (Not yet)\n\n"
               " -h / --help\t\t\t\t: Print this help info\n\n"
               " -ip / --ip-address IP_ADDRESS\t\t: IP Address for ModBus TCP\n\n"
               " -nob / --num-of-bytes NUM_OF_BYTES\t: Num of Bytes for ModBus Protocol Conversion (MAX 4) (DEFAULT 2)\n\n"
               " -noc / --num-of-coils NUM_OF_COILS\t: Num of Coils for ModBus Protocol\n\n"
               " -p / --port PORT\t\t\t: Port for ModBus TCP (DEFAULT 502)\n\n"
               " -rtu\t\t\t\t\t: ModBus RTU Mode (Not yet)\n\n"
               " -s / --slave-id SLAVE_ID\t\t: Slave Id from ModBus Device\n\n"
               " -str / --start-address START_ADDRESS\t: Start Address for ModBus Protocol\n\n"
               " -t / --type-conversion TYPE\t\t: Type will be BIN/HEX/DEC/FLOAT (DEFAULT BIN)\n\n"
               " -tcp\t\t\t\t\t: ModBus TCP Mode\n\n"
               " -v / --version\t\t\t\t: Print version of plugins\n\n"
              );
        err = false;
    } else if (argc == 2) {
        QString arg1(argv[1]);
        if (arg1 == QLatin1String("-h") ||
            arg1 == QLatin1String("--help")) {
            printf(" -f / --function-code FUNCTION_CODE\t: ModBus Function Code\n"
                   "  Function Code List :\n"
                   "  1  : Read Coils (Not yet)\n"
                   "  2  : Read Discrete Inputs (Not yet)\n"
                   "  3  : Read Holding Registers\n"
                   "  ex : -tcp -ip 192.168.3.100 -p 502 -s 1 -f 3 -str 1000 -noc 20 -nob 2 -t FLOAT\n"
                   "  4  : Read Input Registers (Not yet)\n"
                   "  5  : Write Single Coil (Not yet)\n"
                   "  6  : Write Single Register (Not yet)\n"
                   "  15 : Write Multiple Coils (Not yet)\n"
                   "  16 : Write Multiple Registers (Not yet)\n\n"
                   " -h / --help\t\t\t\t: Print this help info\n\n"
                   " -ip / --ip-address IP_ADDRESS\t\t: IP Address for ModBus TCP\n\n"
                   " -nob / --num-of-bytes NUM_OF_BYTES\t: Num of Bytes for ModBus Protocol Conversion (MAX 4) (DEFAULT 2)\n\n"
                   " -noc / --num-of-coils NUM_OF_COILS\t: Num of Coils for ModBus Protocol\n\n"
                   " -p / --port PORT\t\t\t: Port for ModBus TCP (DEFAULT 502)\n\n"
                   " -rtu\t\t\t\t\t: ModBus RTU Mode (Not yet)\n\n"
                   " -s / --slave-id SLAVE_ID\t\t: Slave Id from ModBus Device\n\n"
                   " -str / --start-address START_ADDRESS\t: Start Address for ModBus Protocol\n\n"
                   " -t / --type-conversion TYPE\t\t: Type will be BIN/HEX/DEC/FLOAT (DEFAULT BIN)\n\n"
                   " -tcp\t\t\t\t\t: ModBus TCP Mode\n\n"
                   " -v / --version\t\t\t\t: Print version of plugins\n\n"
                  );
            err = false;
        } else if (arg1 == QLatin1String("-v") ||
                 arg1 == QLatin1String("--version")) {
            printf(" ModBus Plugins Version: %s \nPT. DaunBiru Engiinering\nwww.daunbiru.com\n\n", VERSION);
            err = false;
        }
    } else  if (argc > 2) {
        QString ip = "", type_conversion = "", mode = "";
        int port = 0, slave_id = 0, function = 0, start_address = 0, num_of_coils = 0, num_of_bytes = 0;
        for (int i = 1; i < argc; i++) {
            QString arg1(argv[i]);
            if (arg1 == QLatin1String("-nob") ||
                arg1 == QLatin1String("--num-of-bytes")) {
                num_of_bytes = QString::fromLocal8Bit(argv[i+1]).toInt();
                if (num_of_bytes < 0 || num_of_bytes > 4) num_of_bytes = 0;
            } else if (arg1 == QLatin1String("-f") ||
                       arg1 == QLatin1String("--function-code")) {
                function = QString::fromLocal8Bit(argv[i+1]).toInt();
                if (function != 3) function = 0;
            } else if (arg1 == QLatin1String("-ip") ||
                       arg1 == QLatin1String("--ip-address")) {
                ip = argv[i+1];
                if (ip.split(".").length() < 4) ip = "";
            } else if (arg1 == QLatin1String("-noc") ||
                       arg1 == QLatin1String("--num-of-coils")) {
                num_of_coils = atoi(argv[i+1]);
                if (num_of_coils < 0) num_of_coils = 0;
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
            } else if (arg1 == QLatin1String("-t") ||
                       arg1 == QLatin1String("--type-conversion")) {
                type_conversion = argv[i+1];
            } else if (arg1 == QLatin1String("-tcp") ||
                       arg1 == QLatin1String("--mode-tcp")) {
                mode = "TCP";
            }
        }
        if (mode == "TCP") {
            if (!port) port = 502;
            if (!num_of_bytes) num_of_bytes = 2;
            if (type_conversion.isEmpty()) type_conversion = "BIN";
            if (!ip.isEmpty() && port && slave_id) {
                if (function == 3) {
                    if (start_address >= 0 && num_of_coils > 0) {
                        QStringList result = w.request_modbus(ip, port, slave_id, function, start_address, num_of_coils, num_of_bytes, type_conversion);
                        w.print_result(result);
                        err = false;
                    } else {
                        err = true;
                    }
                }
            } else {
                err = true;
            }
        } else {
            err = true;
        }
    }
    if (err) {
        printf("{\"ERR\": \"Wrong Pluggins Commands\"}\n");
    }
//    plugins/ModBus -tcp --ip-address 192.168.3.11 --port 502 --slave-id 21 --function-code 3 --start-address 1034 --num-of-coils 2 --num-of-bytes 2 --type-conversion FLOAT
//    QStringList result = w.request_modbus("192.168.3.242", 502, 20, 3, 3203, 20, 4, "DEC");
//    QStringList result = w.request_modbus("192.168.3.11", 502, 21, 3, 1034, 2, 2, "FLOAT");
//    w.print_result(result);

    return 0;
}

int main(int argc, char **argv)
{
//    QCoreApplication a(argc, argv);
    int result = processArgs(argc, argv);

//    return a.exec();
    return result;
}
