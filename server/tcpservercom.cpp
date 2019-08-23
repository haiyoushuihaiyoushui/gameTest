/*************************************************************************
	> File Name: tcpservercom.cpp
	> Author: 
	> Mail: 
	> Created Time: 2019年08月20日 星期二 16时29分26秒
 ************************************************************************/

#include "tcpservercom.h"

TcpServerCom::TcpServerCom()
{
	m_listenPort = LISEN_PORT;
	m_sockfd = -1;
	m_clients.clear();
	m_id.clear();
	for (int i = 0; i < strlen(ID_ARRAY); i++)
	{
		m_id.push_back(ID_ARRAY[i]);
	}
}

TcpServerCom::~TcpServerCom()
{
	closeServerCom();
}

void TcpServerCom::initServerCom()
{
	struct sockaddr_in addr;
	int32_t ret = 0;

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

void TcpServerCom::closeServerCom()
{
	if (-1 != m_sockfd)
	{
		close(m_sockfd);
	}
}

void TcpServerCom::serverLoop()
{
	struct sockaddr_in client_addr;
	int32_t connectFd = 0;
	socklen_t cliaddr_len = sizeof(client_addr);
	CLIENT_INFO_S client_info;

	while (1)
	{
		connectFd = accept(m_sockfd, (struct sockaddr *)&client_addr, &cliaddr_len);
		if (connectFd < 0)
		{
			std::cout << __FILE__ << "," << __LINE__ << "accept error" << std::endl;
			continue;
		}

		if (connectFd > 0)
		{
			client_info.clientId = m_id.back();
			m_id.pop_back();
			client_info.clientFd = connectFd;
			client_info.x = getRand();
			client_info.y = getRand();
			memcpy(&client_info.clientAddr, &client_addr, sizeof(struct sockaddr_in));
			m_clients.push_back(client_info);
			std::thread clientThread{[&] { clientProcess(client_info); }};
			clientThread.detach();
		}
	}
}

int32_t TcpServerCom::getRand()
{
	srand((int32_t)time(0));
	return (rand() % 100);
}

void TcpServerCom::analysisData(const CLIENT_INFO_S clientInfo, const uint8_t *recv_buf, const int32_t recv_len)
{
	uint8_t send_buf[BUFF_LEN] = "";
	int32_t send_len = 0;
	std::vector<CLIENT_INFO_S>::iterator it;

	send_buf[FRAME_HEAD] = 0xAB;
	if (0xAB == recv_buf[FRAME_HEAD])
	{
		switch (recv_buf[FRAME_COMMAND])
		{
		case 0x03:
			//遍历vector并发送位置
			for (it = m_clients.begin(); it != m_clients.end(); it++)
			{
				if (it->clientFd != clientInfo.clientFd)
				{
					send(it->clientFd, recv_buf, recv_len, 0);
				}
			}
			break;

		default:
			break;
		}
	}
}

void TcpServerCom::newConnectSend(const CLIENT_INFO_S clientInfo)
{
	FRAME_POSITION_S newSendFrame;
	FRAME_POSITION_S oldSendFrame;
	int32_t send_len = sizeof(FRAME_POSITION_S);
	std::vector<CLIENT_INFO_S>::iterator it;

	newSendFrame.head = 0xAB;
	newSendFrame.command = 0x01;
	newSendFrame.id = clientInfo.clientId;
	newSendFrame.x = clientInfo.x;
	newSendFrame.y = clientInfo.y;

	oldSendFrame.head = 0xAB;
	oldSendFrame.command = 0x01;

	for (it = m_clients.begin(); it != m_clients.end(); it++)
	{
		//告知已存在的client有新client
		send(it->clientFd, &newSendFrame, send_len, 0);
	}
	for (it = m_clients.begin(); it != m_clients.end(); it++)
	{
		//向新client发送已存在的client
		if (it->clientFd != clientInfo.clientFd)
		{
			oldSendFrame.id = it->clientId;
			oldSendFrame.x = it->x;
			oldSendFrame.y = it->y;
			send(clientInfo.clientFd, &oldSendFrame, send_len, 0);
		}
	}
}
void TcpServerCom::disConnectSend(const int32_t id)
{
	FRAME_POSITION_S sendFrame = {0};
	int32_t send_len = sizeof(FRAME_POSITION_S);
	std::vector<CLIENT_INFO_S>::iterator it;

	sendFrame.head = 0xAB;
	sendFrame.command = 0x02;
	sendFrame.id = id;

	for (it = m_clients.begin(); it != m_clients.end(); it++)
	{
		send(it->clientFd, &sendFrame, send_len, 0);
	}
}

void TcpServerCom::clientProcess(const CLIENT_INFO_S clientInfo)
{
	int32_t recv_len = 0;
	char cli_ip[INET_ADDRSTRLEN] = "";
	char pthread_name[NAME_LEN] = "";
	uint8_t recv_buf[BUFF_LEN] = ""; // 接收缓冲区
	std::vector<CLIENT_INFO_S>::iterator it;

	sprintf(pthread_name, "Port:%d", clientInfo.clientAddr.sin_port);
	std::cout << "pthread_name " << pthread_name << std::endl;
	pthread_setname_np(pthread_self(), pthread_name);
	inet_ntop(AF_INET, &clientInfo.clientAddr.sin_addr, cli_ip, INET_ADDRSTRLEN);

	std::cout << cli_ip << ":" << clientInfo.clientAddr.sin_port << " connect"
			  << " " << clientInfo.clientId << std::endl;
	newConnectSend(clientInfo);
	while ((recv_len = recv(clientInfo.clientFd, recv_buf, sizeof(recv_buf), 0)) > 0)
	{
		std::cout << "recv " << clientInfo.clientFd << " " << recv_buf << " " << recv_len << std::endl;
		analysisData(clientInfo, recv_buf, recv_len);
		memset(recv_buf, 0, BUFF_LEN);
	}
	disConnectSend(clientInfo.clientId);
	std::cout << cli_ip << ":" << clientInfo.clientAddr.sin_port << " disconnect"
			  << " " << clientInfo.clientId << std::endl;

	for (it = m_clients.begin(); it != m_clients.end(); it++)
	{
		if (it->clientFd == clientInfo.clientFd)
		{
			m_clients.erase(it);
			break;
		}
	}
	m_id.push_back(clientInfo.clientId);

	close(clientInfo.clientFd);
}