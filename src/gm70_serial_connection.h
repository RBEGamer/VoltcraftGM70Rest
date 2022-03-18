//
// Created by prodevmo on 14.03.22.
//

#ifndef SRC_GM70REST_GM70_SERIAL_CONNECTION_H
#define SRC_GM70REST_GM70_SERIAL_CONNECTION_H
#include <string>
#include <iomanip>
#include <filesystem>

#include "loguru/loguru.hpp"
#include "serialib/lib/serialib.h"
#include "gm70_message.h"

#define LOG_RAW_MSG
#define GM70_MSG_BYTES 16
#define APPLY_NORTH_SOUTH_TO_VALUE // SOUTH => -value NORTH => +value
class gm70_serial_connection {
public:
    gm70_serial_connection();
    bool connect(std::string _port, int _baud);
    bool close();
    gm70_message::GM70MESSAGE receieve_loop();
    bool check_baud_rate(int _baudrate_to_check);


private:
    serialib serial_connection;


};


#endif //SRC_GM70REST_GM70_SERIAL_CONNECTION_H
