
#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <master.h>
#include <unix_serial_proto_io.h>

aquabox::proto::UNIXSerialIO* io;
aquabox::proto::Master* master;

int main(int argc, char**argv)
{
    std::string tty = "/dev/ttyUSB0";
    for(int i = 1; i < argc -1; ++i)
    {
        if (strcmp(argv[i], "-tty") == 0)
            tty = argv[i+1];
    }
    io = new aquabox::proto::UNIXSerialIO(tty.c_str(), B115200);
    master = new aquabox::proto::Master(io);

    std::string command;
    do
    {
        if (command.empty())
            continue;

        if (command == "q" || command == "Q" || command == "quit")
            break;

        if (command == "c" || command == "C" || command == "count")
        {
            std::cout << "Requesting properties count" << std::endl;
            int count = master->getPropertiesCount();
            if (count < 0)
                std::cout << "\tClient has not properties" << std::endl;
            else
                std::cout << "\tSlave has " << count << " properties" << std::endl;
        }

        if (command == "n" || command == "N" || command == "name")
        {
            int idx = 0;
            std::cout << "? Property index:" << std::endl;
            std::cin >> idx;
            std::cout << "! Requesting property " << idx << std::endl;

            char name[256];
            memset(name, '\0', 256);

            if (master->getPropertyName(idx, name))
                std::cout << "\tProperty " << idx << " named as: '" << name << "'" << std::endl;
            else
                std::cerr << "\tRequesting property name failed";
        }
    } while (std::getline(std::cin, command));

    delete master;
    delete io;
    
    return 0;
}