//
// Created by prodevmo on 14.03.22.
//

#include "gm70_message.h"


std::string gm70_message::toJson(const gm70_message::GM70MESSAGE _msg, const bool _val_only) {

if(_val_only){
    const json11::Json t = json11::Json::object{
            {"value",_msg.value},
            {"valid",_msg.valid},
            {"unit",std::string(magic_enum::enum_name(_msg.unit))},
    };
    return t.dump();
}else{
    const json11::Json t = json11::Json::object{
            {"valid",_msg.valid},
            {"timestamp", (int)_msg.timestamp},
            {"unit",std::string(magic_enum::enum_name(_msg.unit))},
            {"decimal_offset",std::string(magic_enum::enum_name(_msg.dp_offset))},
            {"polarity",std::string(magic_enum::enum_name(_msg.polarity))},
            {"value",_msg.value}
    };
    return t.dump();
}

}
