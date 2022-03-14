//
// Created by prodevmo on 14.03.22.
//

#include <cmath>
#include <chrono>
#include "gm70_serial_connection.h"

gm70_serial_connection::gm70_serial_connection() {

}

bool gm70_serial_connection::check_baud_rate(const int _baudrate_to_check) {
#ifdef __MACH__
    return true;
#else
    switch (_baudrate_to_check) {
        case 9600:
            return B9600;
        case 19200:
            return B19200;
        case 38400:
            return B38400;
        case 57600:
            return B57600;
        case 115200:
            return B115200;
        case 230400:
            return B230400;
        case 460800:
            return B460800;
        case 500000:
            return B500000;
        case 576000:
            return B576000;
        case 921600:
            return B921600;
        case 1000000:
            return B1000000;
        case 1152000:
            return B1152000;
        case 1500000:
            return B1500000;
        case 2000000:
            return B2000000;
        case 2500000:
            return B2500000;
        case 3000000:
            return B3000000;
        case 3500000:
            return B3500000;
        case 4000000:
            return B4000000;
        default:
            return 0;
    }
#endif
}

bool gm70_serial_connection::connect(const std::string _port, const int _baud) {
    //CLOSE DDEVICE IF OPEN
    if (serial_connection.isDeviceOpen()) {
        serial_connection.closeDevice();
    }

    if (_port.empty()) {
        LOG_F(ERROR, "gm70_serial_connection::connect PORT INVALID");
        return false;
    }

    if (_baud <= 0 || !check_baud_rate(_baud)) {
        LOG_F(ERROR, "gm70_serial_connection::connect BAUD-RATE invalid %i", _baud);
        return false;
    }

    const char errorOpening = serial_connection.openDevice(_port.c_str(), _baud);

    if (errorOpening != 1) {
        LOG_F(ERROR, "gm70_serial_connection::connect CANT OPEN DEVICE");
        return false;
    };
    // serial_connection.
    serial_connection.flushReceiver();

    return true;
}

bool gm70_serial_connection::close() {

}

gm70_message::GM70MESSAGE gm70_serial_connection::receieve_loop() {
    gm70_message::GM70MESSAGE msg;
    msg.valid = false;

    if (serial_connection.isDeviceOpen() && serial_connection.available()) {

        char charr[GM70_MSG_BYTES] = {0};
        // const int chars_read = serial_connection.readString(charr, '\n', serial_connection.available(), 500);
        const int r = serial_connection.readBytes(charr, 2 * GM70_MSG_BYTES, 20, 100);

        if (r == GM70_MSG_BYTES) {
            //CHECK FOR START AND STOP BYTE
            if (charr[0] == 2 && charr[GM70_MSG_BYTES - 1] == '\r') {
#ifdef LOG_RAW_MSG
                LOG_F(INFO, "%s", charr);
#endif
                msg.valid = true;
                msg.timestamp = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
                //CONSTRUCT VALUE INTO FLOAT
                /// FIRST ADD UP THE 8 SEPERATE DIGITS INTO ONE INT
                int tmp = 0;
                int mult = 1;
                for (int i = 14; i > 7; i--) {
                    tmp += ((int) (charr[i] - '0')) * mult;
                    mult *= 10;
                }
                //LOG_F(INFO, "%i", tmp);

                // ADD THE DECIMAL POINT
               // msg.value = tmp * (std::log((int)(charr[6] - '0')) /std::log(10.0));

                if(charr[6] == '1'){
                    msg.value = tmp * 0.1f;
                    msg.dp_offset = gm70_message::GM70_DP::OFFSET_1;
                }else if(charr[6] == '2') {
                    msg.value = tmp * 0.01f;
                    msg.dp_offset = gm70_message::GM70_DP::OFFSET_2;
                }else if(charr[6] == '3'){
                    msg.value = tmp * 0.001f;
                    msg.dp_offset = gm70_message::GM70_DP::OFFSET_3;
                }else{
                    msg.value = tmp * 1.0f;
                    msg.dp_offset = gm70_message::GM70_DP::OFFSET_0;
                }

                
                
                //READ UNIT
                if(charr[3] == 'B' && charr[4] == '5') {
                    msg.unit = gm70_message::GM70_UNIT::GAUSS;
                }else if(charr[3] == 'E' && charr[4] == '3'){
                        msg.unit = gm70_message::GM70_UNIT::MILLI_TESLA;
                }else{
                    msg.unit = gm70_message::GM70_UNIT::UNKNOWN;
                }


                //READ NORTH SOUTH ORIENTATION
                if(charr[5] == '0'){
                    msg.polarity = gm70_message::GM70_POLARITY::NORTH;
                }else if(charr[5] == '1') {
                    msg.polarity = gm70_message::GM70_POLARITY::SOUTH;
                }else{
                    msg.polarity = gm70_message::GM70_POLARITY::UNKNOWN;
                }


                //IF POARITY IS SOUTH => THE VALUE IS NEGATIV
#ifdef APPLY_NORTH_SOUTH_TO_VALUE
                if(msg.polarity == gm70_message::GM70_POLARITY::SOUTH){
                    msg.value = msg.value * -1.0f;
                }
#endif
            }
        } else {
            serial_connection.flushReceiver();
        }

    }


    return msg;
}