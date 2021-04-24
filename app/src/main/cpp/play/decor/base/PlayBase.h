//
// Created by Admin on 2021/4/24.
//

#ifndef LEARNFFMPEG_PLAYBASE_H
#define LEARNFFMPEG_PLAYBASE_H

#include <jni.h>
#include "thread"

using namespace std;
class PlayBase{
public:
       PlayBase(char *url);
protected:
    void init();
    virtual void play();
    virtual void stop();
    virtual void pause();
    virtual void resume();
    void startCoding();
    virtual void codingLoop(){};
    virtual void codingReady(){};
    static void doing(PlayBase *playBase);
    protected:
        char *m_url;
        std::thread *m_thread = nullptr;
        //锁和条件变量
        std::mutex m_mutex;
        std::condition_variable  m_Cond;
        bool already;
};

#endif //LEARNFFMPEG_PLAYBASE_H