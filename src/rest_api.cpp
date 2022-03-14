//
// Created by Marcel Ochsendorf on 21.02.22.
//

#include "rest_api.h"


rest_api::rest_api(msg_queue *_queue) {
    if(_queue == nullptr ){
        LOG_F(ERROR, "rest_api::rest_api _queue is null");
    }
    queue = _queue;
}


void rest_api::set_bind_settings(const std::string _bind_host, const int _port){
    if(_port <= 0 || _port > 65536){
        LOG_F(ERROR, "rest_api::set_bind_settings port invalid %i", _port);
        port = 4242;
    }else{
        port = _port;
    }

    if(_bind_host.empty()){
        LOG_F(ERROR, "rest_api::set_bind_settings _bind_host is empty");
        host = "127.0.0.0";
    }else{
        host = _bind_host;
    }

}

void rest_api::enable_service(bool _en) {
    if(_en){
        if(thread_running){
            LOG_F(ERROR,"rest_api::enable_service => THREAD ALREADY RUNNING");
            return;
        }
        std::thread t1(recieve_thread_function, this);
        t1.detach();
        update_thread = &t1;
        thread_running = true;

        LOG_F(INFO,"rest_api start thread");
    }else{
        if (update_thread)
        {
            thread_running = false;
            update_thread->join();
            LOG_F(INFO,"rest_api stop thread");
        }
    }
}

void rest_api::recieve_thread_function(rest_api *_this) {
    using namespace httplib;
    httplib::Server svr;
    //REGISTER WEBSERVER EVENTS




    svr.Get("/", [_this](const Request& req, Response& res) {
        res.status = 200;
        const json11::Json t = json11::Json::object{
            {"version", VERSION},
            {"version", "/version"},
            {"current_value", "/current"},
            {"average", "/avg"},
            {"config", "/config"}
        };

        res.set_content(t.dump(), "application/json");
    });

    svr.Get("/version", [_this](const Request& req, Response& res) {
        res.status = 200;
        const json11::Json t = json11::Json::object{{"version", VERSION}};
        res.set_content(t.dump(), "application/json");
    });

    svr.Get("/config", [_this](const Request& req, Response& res) {
        res.status = 200;
        res.set_content(config_parser::getInstance()->toJson(), "application/json");
    });



    //GET CURRENT
    svr.Get("/avg", [_this](const Request& req, Response& res) {
        if(_this->queue){
            res.status = 200;
            gm70_message::GM70MESSAGE msg = _this->queue->get_average();
            res.set_content(gm70_message::toJson(msg,true),"application/json");
        }else{
            res.status = 500;
            const json11::Json t = json11::Json::object{{"error", "queue->get_last() null or error"}};
            res.set_content(t.dump(), "application/json");
            return;
        }
    });

    svr.Get("/current", [_this](const Request& req, Response& res) {
        if(_this->queue){
            res.status = 200;
            gm70_message::GM70MESSAGE msg = _this->queue->get_last();
            res.set_content(gm70_message::toJson(msg, false),"application/json");
        }else{
            res.status = 500;
            const json11::Json t = json11::Json::object{{"error", "queue->get_last() null or error"}};
            res.set_content(t.dump(), "application/json");
            return;
        }
    });

    svr.set_error_handler([](const auto& req, auto& res) {
        res.status = 404;
        const json11::Json t = json11::Json::object{{"error", res.status}};
        res.set_content(t.dump(), "application/json");

    });



    svr.set_exception_handler([](const auto& req, auto& res, std::exception &e) {
        res.status = 500;
        const json11::Json t = json11::Json::object{{"exception", res.status}};
        res.set_content(t.dump(), "application/json");
    });

    //START WEBSERVER
    if(!svr.listen(_this->host.c_str(), _this->port)){
        LOG_F(ERROR,"WEBSERVER BIND FAILED");
        _this->enable_service(false); //STOP SERVER
        return;
    }
    while (_this->thread_running) {

    }
    svr.stop();
}

