/*************************************************************************
	> File Name: tcpservercom.cpp
	> Author: 
	> Mail: 
	> Created Time: 2019年08月20日 星期二 16时29分26秒
 ************************************************************************/

#include "tcpservercom.h"

TcpServerCom::TcpServerCom()
{
	struct sockaddr_in addr;
	int32_t ret = 0;

    m_listenPort = LISEN_PORT;
    m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (m_sockfd < 0)
	{
		std::cout << __FILE__ << "," << __LINE__ << "socket error" << std::endl;
	}
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(m_listenPort);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	ret = bind(m_sockfd, (struct sockaddr *)&addr, sizeof(addr));
	if (0 != ret)
	{
		std::cout << __FILE__ << "," << __LINE__ << "bind error" << std::endl;
		close(m_sockfd);
	}

	ret = listen(m_sockfd, MAX_CONNECT);
	if (0 != ret)
	{
		std::cout << __FILE__ << "," << __LINE__ << "listen error" << std::endl;
		close(m_sockfd);
	}
}

TcpServerCom::~TcpServerCom()
{
}

void TcpServerCom::initServerCom()
{
    
}
