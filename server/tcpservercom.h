/*************************************************************************
	> File Name: tcpservercom.h
	> Author: 
	> Mail: 
	> Created Time: 2019年08月20日 星期二 16时29分42秒
 ************************************************************************/

#ifndef _TCPSERVERCOM_H
#define _TCPSERVERCOM_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <iostream>
#include <vector>
#include <string>
#include <thread>

const int32_t LISEN_PORT = 56321;
const int32_t MAX_CONNECT = 10;
const int32_t NAME_LEN = 48;
const int32_t BUFF_LEN = 1024;
const char ID_ARRAY[] = {'A', 'B', 'C', 'D', 'E', 
                        'F', 'G', 'H', 'I', 'J', 'K', '\0'};

typedef struct
{
    char clientId;
    int32_t clientFd;
    struct sockaddr_in clientAddr;   
}CLIENT_INFO_S;

typedef enum
{
    FRAME_HEAD,
    FRAME_COMMAND,
    FRAME_DATA,
}FRAME;

class TcpServerCom
{
public:
    TcpServerCom();
    ~TcpServerCom();

    void initServerCom();
    void closeServerCom();
    void serverLoop();
    void analysisData(const CLIENT_INFO_S clientInfo, const uint8_t *recv_buf, const int32_t recv_len);

protected:
    void clientProcess(const CLIENT_INFO_S clientInfo);

private:
    int32_t m_listenPort;
    int32_t m_sockfd;
    std::vector<CLIENT_INFO_S> m_clients;
    std::vector<char> m_id;
};

#endif
