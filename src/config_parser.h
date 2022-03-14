#pragma once
#ifndef __CONFIG_PARSER_H__
#define __CONFIG_PARSER_H__

#include <thread>
#include <mutex>
#include <iostream>
#include <fstream>
#include <map>
#include <utility>

///INCLUDE 3rd PARTY
#include "loguru/loguru.hpp"
#include "inih/INIReader.h"
#include "magic_enum/include/magic_enum.hpp"
#include "json11/json11.hpp"




#define USE_STD_LOG
#define INI_SETTINGS_CATEGORY_TOKEN "SETTINGS"
//THIS MESSAGE WILL BE PRINTED ONTOP OF THE DEFAULT CONFIG FILE
//#define INIFILE_COMMENTS "# CONFIG \n\n"

class config_parser
        {
        public:
            ///PLEASE NOTE BY EDIT/ADDING ENTRIES PLEASE EDIT ConfigParser::createConfigFile, loadDefaults FUNCTIONS TOO
            enum class CFG_ENTRY
                    {


                REST_WEBSERVER_ENABLE,
                REST_WEBSERVER_BIND_PORT,
                REST_WEBSERVER_BIND_HOST,


                GM70_SERIAL_INTERFACE,
                PACKET_QUEUE_LEN,
                READ_VALUE_AVG_LEN
                    };

            static config_parser* getInstance();

            bool loadConfigFile(const std::string& _file);
            bool createConfigFile(const std::string& _file, bool _load_directly); //GENERATES A CONFIG FILE WITH DEFAULTS

            bool configLoaded();



            std::string get(config_parser::CFG_ENTRY _entry);
            void set(config_parser::CFG_ENTRY _entry, const std::string& _value, const std::string& _conf_file_path); //SETS A CONFIG VALUE FOR ENTRY IF _conf_file_path IS NOT EMPTY THE NEW CONFIG WILL BE WRITTEN TO FILE
            void setInt(config_parser::CFG_ENTRY _entry, int _value, const std::string& _conf_file_path);  //SETS A CONFIG VALUE FOR ENTRY IF _conf_file_path IS NOT EMPTY THE NEW CONFIG WILL BE WRITTEN TO FILE
            bool writeConfigFile(const std::string& _file);

            bool getInt(config_parser::CFG_ENTRY _entry, int& _ret);
            int getInt_nocheck(config_parser::CFG_ENTRY _entry); //SAME AS getInt BUT WITH NO SUCCESS CHECK IF ENTRY FOUND; RETURN 0 IF ENTRY WAS NOT FOUND
            bool getBool(config_parser::CFG_ENTRY _entry, bool& _ret);


            void loadDefaults();

            bool getBool_nocheck(config_parser::CFG_ENTRY _entry);//SAME AS getBool BUT WITH NO SUCCESS CHECK IF ENTRY FOUND; RETURN FALSE IF ENTRY WAS NOT FOUND
            std::string toJson();
            bool loadFromJson(const std::string& _jsonstr, bool load_only_user_data); //loads config from json  load_only_user_data is TRUE = updates only the user data section with the new data from json
            bool loadFromJson(json11::Json::object _jsobj, bool load_only_user_data);



            std::map<std::string, std::string> get_raw_config_map();
        private:
            //USED IN toJson/loadFromJson
            struct KV_PAIR{
                std::string key;
                std::string value;
                KV_PAIR(const std::string _k,const std::string _v){
                    key = std::move(_k);
                    value = std::move(_v);
                }
                KV_PAIR()= default;
            };

            static config_parser* instance;
            static std::mutex acces_lock_mutex;

            bool cfg_loaded_success = false;
            config_parser();
            ~config_parser();
            //STORES THE ACUTAL CONFIG FILE
            std::map<config_parser::CFG_ENTRY, std::string> config_store;
            INIReader reader;
        };
#endif
