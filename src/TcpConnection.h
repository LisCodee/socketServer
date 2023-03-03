//
// Created by Lijingxin on 2023/3/2.
//

#ifndef SOCKETSERVER_TCPCONNECTION_H
#define SOCKETSERVER_TCPCONNECTION_H

#include "memory"
#include "Channel.h"

class TcpConnection {

public:
    TcpConnection();
    ~TcpConnection();

private:
//    stateE          m_state;                          //记录当前连接状态信息
    std::shared_ptr<Channel> m_spChannel;               //引用channel对象
//    const InetAddress        m_localAddr;               //本地地址信息
//    const InetAddress        m_remoteAddr;              //对端地址信息
//
//    Buffer                   m_inputBuffer;             //接收缓存区
//    Buffer                   m_outputBuffer;            //发送缓存去
};


#endif //SOCKETSERVER_TCPCONNECTION_H
