//
// Created by Lijingxin on 2023/3/13.
//

#include "Platform.h"
#include "iostream"

NetworkInitializer::NetworkInitializer() {
    WORD wVersionRequested;
    WSADATA wsaData;
    /* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
    wVersionRequested = MAKEWORD(2,2);
    int err = ::WSAStartup(wVersionRequested, &wsaData);

    if (err != 0) {
        std::cout << ("WSAStartup failed with error: ") << std::endl;
    }

    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
        std::cout << ("Could not find a usable version of Winsock.dll") << std::endl;
    }
}

NetworkInitializer::~NetworkInitializer() {
    ::WSACleanup();
}