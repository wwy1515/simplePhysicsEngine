//
// Created by Wenyou Wang on 1/21/21.
//

#ifndef SIMPLEPHYSICSENGINE_LOGGER_H
#define SIMPLEPHYSICSENGINE_LOGGER_H

#include "header.h"
#include "extern/imgui/imgui.h"



class Timer
{
public:
    typedef std::chrono::high_resolution_clock Clock;
    typedef std::chrono::milliseconds milliseconds;

    Clock::time_point t0;

    void begin()
    {
        t0 = Clock::now();
    }

    std::string getTime()
    {
        Clock::time_point t1 = Clock::now();

        milliseconds time_span = std::chrono::duration_cast<milliseconds>(t1 - t0);

        return std::to_string(time_span.count());
    }
};

static Timer timer;




enum logType
{
    info = 0,
    warn = 1,
    error = 2,
    key = 3
};

static char* categories[4] = { "info", "warn", "error", "key" };

class logMessage
{
    int filter = logType::key;

public:
    void log(logType pType, std::string pLogInfo, std::string pTime)
    {
        if(pType < filter)
            return;
        std::cout
                <<ImGui::GetFrameCount()<<" "
                <<categories[pType]<<" "
                <<pLogInfo<<" "
                <<timer.getTime()<<" ms"<<std::endl;
    }

};

static logMessage logger;





#endif //SIMPLEPHYSICSENGINE_LOGGER_H
