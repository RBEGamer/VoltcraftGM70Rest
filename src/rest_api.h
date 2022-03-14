//
// Created by Marcel Ochsendorf on 21.02.22.
//
#pragma once
#ifndef CAN2REST_REST_API_H
#define CAN2REST_REST_API_H

#include <string>
#include <list>
#include <vector>
//THREAD STUFF
#include <queue>
#include <mutex>
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>


#include "cpp-httplib/httplib.h"
#include "json11/json11.hpp"
#include "loguru/loguru.hpp"



#include "config_parser.h"
#include "gm70_message.h"
#include "msg_queue.h"

#ifndef VERSION
#define VERSION "42.42.42"
#endif


class rest_api {

public:

    rest_api(msg_queue* _queue);

    void set_bind_settings(std::string _bind_host = "0.0.0.0", int _port = 4242);
    void enable_service(bool _en); //START STOP WEBSERVER THREAD


private:

    bool thread_running = false;

    int port = 4242;
    std::string host =  "0.0.0.0";

    std::thread* update_thread = nullptr;

    msg_queue* queue;
    static void recieve_thread_function(rest_api* _this);

};


#endif //CAN2REST_REST_API_H
