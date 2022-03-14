//
// Created by prodevmo on 14.03.22.
//

#ifndef SRC_GM70REST_GM70_MESSAGE_H
#define SRC_GM70REST_GM70_MESSAGE_H
#include <string>

#include "json11/json11.hpp"
#include "magic_enum/include/magic_enum.hpp"

class gm70_message {



public:
        enum class GM70_POLARITY{
            UNKNOWN = -1,
            NORTH = 0,
            SOUTH = 1
        };

        enum class GM70_UNIT{
            UNKNOWN = 0,
            GAUSS = 'G',
            MILLI_TESLA = 'E',
        };

        enum class GM70_DP{
            OFFSET_0 = 0,
            OFFSET_1 = 1,
            OFFSET_2 = 2,
            OFFSET_3 = 3
        };

        struct GM70MESSAGE{
        public:
            GM70_POLARITY polarity = GM70_POLARITY::UNKNOWN;
            GM70_UNIT unit = GM70_UNIT::UNKNOWN;
            GM70_DP dp_offset = GM70_DP::OFFSET_0;
            float value;
            bool valid = false;
            unsigned int timestamp;
        };


        static std::string toJson(GM70MESSAGE _msg, bool _val_only);

};


#endif //SRC_GM70REST_GM70_MESSAGE_H
