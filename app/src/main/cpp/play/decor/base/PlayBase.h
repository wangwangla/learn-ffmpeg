//
// Created by Admin on 2021/4/24.
//

#ifndef LEARNFFMPEG_PLAYBASE_H
#define LEARNFFMPEG_PLAYBASE_H

#include <jni.h>
#include <PlayBase.h>

#endif //LEARNFFMPEG_PLAYBASE_H

class  PlayBase{
public:
   PlayBase();
   void init();

protected:
    char *m_url;

};
