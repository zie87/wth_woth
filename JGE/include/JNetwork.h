#ifndef _JNETWORK_H_
#define _JNETWORK_H_

// Network support for PSP
//#define NETWORK_SUPPORT

#include "JGE.h"
#include <string>
#include <map>
using namespace std;
class JSocket;
#include <iostream>
#include <sstream>

#include <wge/thread.hpp>

typedef void (*processCmd)(istream&, ostream&);

class JNetwork {
private:
    int connected_to_ap;
    JSocket* socket;
    wge::mutex sendMutex;
    wge::mutex receiveMutex;
    stringstream received;
    stringstream toSend;
    static map<string, processCmd> sCommandMap;

public:
    JNetwork();
    ~JNetwork();
    string serverIP;

    int connect(string serverIP = "");
    bool isConnected();
    static void ThreadProc(void* param);
#if !defined(WIN32) && !defined(LINUX)
    static int connect_to_apctl(int config);
#endif
    bool sendCommand(string command);
    static void registerCommand(string command, processCmd processCommand, processCmd processResponse);

private:
    wge::thread* mpWorkerThread;
};

#endif
