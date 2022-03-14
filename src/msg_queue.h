//
// Created by prodevmo on 14.03.22.
//

#ifndef SRC_GM70REST_MSG_QUEUE_H
#define SRC_GM70REST_MSG_QUEUE_H
#include <queue>
#include <vector>
#include <mutex>

#include "gm70_message.h"
class msg_queue {
public:
    msg_queue();
    void set_queue_size(unsigned int _queue_size);
    bool enqueue(gm70_message::GM70MESSAGE _msg);
    void set_avg_len( unsigned int _len);
    gm70_message::GM70MESSAGE dequeue();

    gm70_message::GM70MESSAGE get_last();
    gm70_message::GM70MESSAGE get_average();
private:

    std::mutex lock;
    unsigned int avg_len;
    unsigned int queue_sz;
    std::vector<gm70_message::GM70MESSAGE> message_queue;
    gm70_message::GM70MESSAGE last_push;
};


#endif //SRC_GM70REST_MSG_QUEUE_H
