//
// Created by prodevmo on 14.03.22.
//

#include "msg_queue.h"


msg_queue::msg_queue() {

}

void msg_queue::set_queue_size(unsigned int _queue_size) {

    if(_queue_size < 0){
        queue_sz = 0;
    }
    lock.lock();
    queue_sz = _queue_size;
    lock.unlock();
}

bool msg_queue::enqueue(const gm70_message::GM70MESSAGE _msg) {

    if(queue_sz <= 0){
        return false;
    }

    lock.lock();
    if (message_queue.size() > queue_sz) {
        for (unsigned int i = 0; i < message_queue.size() > queue_sz; i++) {
            message_queue.erase(message_queue.begin());
        }
    }

    message_queue.push_back(_msg);
    last_push = _msg;
    lock.unlock();
    return true;
}

void msg_queue::set_avg_len(const unsigned int _len) {
    lock.lock();
    if (_len < 1) {
        avg_len = 1;
    } else if (_len > queue_sz) {
        avg_len = queue_sz;
        return;
    } else {
        avg_len = _len;
    }
    lock.unlock();
}

gm70_message::GM70MESSAGE msg_queue::dequeue() {
    gm70_message::GM70MESSAGE tmp;
    lock.lock();
    if(message_queue.size() > 0){
        tmp = message_queue.front();
        message_queue.erase(message_queue.begin());
    }
    lock.unlock();
    return tmp;
}

gm70_message::GM70MESSAGE msg_queue::get_last() {
    lock.lock();
    gm70_message::GM70MESSAGE tmp = last_push;
    lock.unlock();
    return tmp;
}

gm70_message::GM70MESSAGE msg_queue::get_average() {
    gm70_message::GM70MESSAGE tmp;
    float val = 0.0f;
    lock.lock();
    unsigned int sz  =message_queue.size();
    if(sz > avg_len){
        sz = avg_len;
    }

    for(unsigned int i = 0; i < sz; i++){
        val += message_queue[i].value;
    }

    if(sz > 0){
        tmp.valid = false;
        val = val / (sz*1.0f);
    }
    lock.unlock();
    tmp.value = val;

    return tmp;
}
