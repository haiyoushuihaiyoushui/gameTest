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
#include <pthread.h>
#include <stdint.h>
#include <iostream>
#include <vector>
#include <string>

const int32_t LISEN_PORT = 56321;
const int32_t MAX_CONNECT = 10;

class TcpServerCom
{
public:
    TcpServerCom();
    ~TcpServerCom();
    
    void initServerCom();

private:
    int32_t m_listenPort;
    int32_t m_sockfd;
    std::vector<int32_t> m_clients;
};

#endif
