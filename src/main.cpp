#include <iostream>
#include <string>
#include <csignal>
#include <string>
#include <algorithm>
#include <map>
#include <chrono>
#include <ctime>
#include <regex>

#include "loguru/loguru.hpp"


#ifndef VERSION
#define VERSION "42.42.42"
#endif

#define LOG_FILE_PATH "./gm70rest.log"
#define LOG_FILE_PATH_ERROR "./gm70rest_error.log"
#define CONFIG_FILE_PATH "./gm70rest_config.ini"

#include "config_parser.h"
#include "rest_api.h"
#include "msg_queue.h"
#include "gm70_serial_connection.h"


volatile bool main_loop_running = false;


void signal_callback_handler(int signum) {
    main_loop_running = false;
    LOG_F(ERROR, "Caught signal %d\n", signum);
    loguru::flush();
}

bool cmdOptionExists(char **begin, char **end, const std::string &option) {
    return std::find(begin, end, option) != end;
}


int main(int argc, char *argv[]) {
    //REGISTER SIGNAL HANDLER
    signal(SIGINT, signal_callback_handler);

    //SETUP LOGGER
    loguru::init(argc, argv);
    loguru::add_file(LOG_FILE_PATH, loguru::Truncate, loguru::Verbosity_MAX);
    loguru::add_file(LOG_FILE_PATH_ERROR, loguru::Truncate, loguru::Verbosity_WARNING);

    //---- PRINT HELP MESSAGE ------------------------------------------------------ //
    if (cmdOptionExists(argv, argv + argc, "-help")) {
        std::cout << "---- HELP ----" << std::endl;
        std::cout << "-help                   | prints this message" << std::endl;
        std::cout << "-version                | print version of this tool" << std::endl;
        std::cout << "-writeconfig            | creates default config" << std::endl;
        std::cout << "---- END HELP ----" << std::endl;
        return 0;
    }

    //---- PRINT VERSION MESSAGE ------------------------------------------------------ //
    if (cmdOptionExists(argv, argv + argc, "-version")) {
        std::cout << "---- GM70REST VERSION ----" << std::endl;
        std::cout << "version:" << VERSION << std::endl;
        std::cout << "build date:" << __DATE__ << std::endl;
        std::cout << "build time:" << __TIME__ << std::endl;
        return 0;
    }



    //LOAD CONFIG
    LOG_SCOPE_F(INFO, "LOADING CONFIG FILE %s", CONFIG_FILE_PATH);

    //OVERWRITE WITH EXISTSING CONFIG FILE SETTINGS
    if (!config_parser::getInstance()->loadConfigFile(CONFIG_FILE_PATH) ||
        cmdOptionExists(argv, argv + argc, "-writeconfig")) {
        LOG_F(WARNING, "--- CREATE LOCAL CONFIG FILE -----");
        //LOAD DEFAULTS
        config_parser::getInstance()->loadDefaults();
        //WRITE CONFIG FILE TO FILESYSTEM
        config_parser::getInstance()->createConfigFile(CONFIG_FILE_PATH, true);
        LOG_F(ERROR, "WRITE NEW CONFIGFILE DUE MISSING ONE");
    }
    LOG_F(INFO, "CONFIG FILE LOADED");

    //SETUP SERIAL
    gm70_serial_connection con;
    /// CONNECT SERIAL PORT GM70 USES 9600 BAUD
    if(!con.connect(config_parser::getInstance()->get(config_parser::CFG_ENTRY::GM70_SERIAL_INTERFACE),9600)){
        LOG_F(ERROR, "CANT CONNECT TO SERIAL DEVICE");
        return -1;
    }


    //MESSAGE QUEUE
    msg_queue data_queue;
    data_queue.set_queue_size(config_parser::getInstance()->getInt_nocheck(config_parser::CFG_ENTRY::PACKET_QUEUE_LEN));
    data_queue.set_avg_len(config_parser::getInstance()->getInt_nocheck(config_parser::CFG_ENTRY::READ_VALUE_AVG_LEN));

    //SETUP REST API SERVER
    rest_api api_server(&data_queue);
    //SET BIND HOST AND PORT
    api_server.set_bind_settings(config_parser::getInstance()->get(config_parser::CFG_ENTRY::REST_WEBSERVER_BIND_HOST),config_parser::getInstance()->getInt_nocheck(config_parser::CFG_ENTRY::REST_WEBSERVER_BIND_PORT));
    //ENABLE WEBSERVER
    api_server.enable_service(config_parser::getInstance()->getBool_nocheck(config_parser::CFG_ENTRY::REST_WEBSERVER_ENABLE));





    LOG_F(INFO, "STARTING MAIN LOOP");
    main_loop_running = true;
    while (main_loop_running) {
        // MAIN LOOP IN MAIN THREAD
        /// HANDLE SERIAL CONNECTION
        gm70_message::GM70MESSAGE msg = con.receieve_loop();
        ///IF VALID MSG => QUEUE THEM SO THE WEBSERVER CAN DELIVER THE MSG
        if(msg.valid){
            data_queue.enqueue(msg);
        }
    }
    //STOP SERVER
  //  api_server.enable_service(false);
    con.close();
    //WRITE LOGS
    loguru::flush();

    return 0;
}
