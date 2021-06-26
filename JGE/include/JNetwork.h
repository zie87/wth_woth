#ifndef _JNETWORK_H_
#define _JNETWORK_H_

#include "JGE.h"
#include <string>
#include <map>

class JSocket;
#include <sstream>

#include <wge/thread.hpp>

typedef void (*processCmd)(std::istream&, std::ostream&);

class JNetwork {
private:
    int connected_to_ap;
    JSocket* socket;
    wge::mutex sendMutex;
    wge::mutex receiveMutex;
    std::stringstream received;
    std::stringstream toSend;
    static std::map<std::string, processCmd> sCommandMap;

public:
    JNetwork();
    ~JNetwork();
    std::string serverIP;

    int connect(std::string serverIP = "");
    bool isConnected();
    static void ThreadProc(void* param);
#if !defined(WIN32) && !defined(LINUX)
    static int connect_to_apctl(int config);
#endif
    bool sendCommand(std::string command);
    static void registerCommand(std::string command, processCmd processCommand, processCmd processResponse);

private:
    wge::thread* mpWorkerThread;
};

#endif
