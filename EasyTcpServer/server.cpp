#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <windows.h>
#include <WinSock2.h>
#include <stdio.h>
#include <vector>

using namespace std;

//#pragma comment(lib, "ws2_32.lib")  // 指明需要使用的动态链接库，只有vc++支持这种写法, 也可以在属性里链接库输入里配置

enum CMD
{
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGOUT,
	CMD_LOGOUT_RESULT,
	CMD_ERROR,
};

struct DataHeader
{
	short m_dataLength;  // 数据长度
	short m_cmd;   // 命令

	DataHeader(short dataLength, short cmd) : m_dataLength(dataLength), m_cmd(cmd) {}
	DataHeader() = default;
};

struct Login : public DataHeader
{
	char userName[32];
	char password[32];

	Login() : DataHeader(sizeof(Login), CMD_LOGIN) {}
};

struct LoginResult : public DataHeader
{
	int result;

	LoginResult() : DataHeader(sizeof(LoginResult), CMD_LOGIN_RESULT), result(0) {}
};

struct Logout : public DataHeader
{
	char userName[32];

	Logout() : DataHeader(sizeof(Logout), CMD_LOGOUT) {}
};

struct LogoutResult : public DataHeader
{
	int result;

	LogoutResult() : DataHeader(sizeof(LogoutResult), CMD_LOGOUT_RESULT), result(0) {}
};

vector<SOCKET> g_clients;

int processor(SOCKET _cSock)
{
	// 缓冲区
	char szRecv[4096] = {};
	// 5. 接收客户端数据
	int nLen = recv(_cSock, szRecv, sizeof(DataHeader), 0);
	DataHeader* header = (DataHeader*)szRecv;
	if (nLen <= 0)
	{
		printf("客户端已退出, 任务结束\n");
		return -1;
	}
	// 6. 处理请求
	switch (header->m_cmd)
	{
	case CMD_LOGIN:
	{
		Login* login;
		recv(_cSock, szRecv + sizeof(DataHeader), header->m_dataLength - sizeof(DataHeader), 0);
		login = (Login*)szRecv;
		printf("接收到的命令: %d, 数据长度：%d, userName: %s, password: %s\n", header->m_cmd, header->m_dataLength, login->userName, login->password);
		LoginResult ret;
		send(_cSock, (char*)&ret, sizeof(LoginResult), 0);
	}
	break;
	case CMD_LOGOUT:
	{
		Logout* logout;
		recv(_cSock, szRecv + sizeof(DataHeader), sizeof(Logout) - sizeof(DataHeader), 0);
		logout = (Logout*)szRecv;
		printf("接收到的命令: %d, 数据长度：%d, userName: %s\n", header->m_cmd, header->m_dataLength, logout->userName);
		LogoutResult logoutRet;
		send(_cSock, (char*)&logoutRet, sizeof(LogoutResult), 0);
	}
	break;
	default:
	{
		printf("接收到的命令: %d, 数据长度：%d\n", header->m_cmd, header->m_dataLength);
		DataHeader header = { 0, CMD_ERROR };
		send(_cSock, (char*)&header, sizeof(DataHeader), 0);
	}
	break;
	}
}

int main()
{
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);  // 启动windows socket 2.x网络环境
	// 建立一个tcp服务端
	// 1. 建立一个socket
	// AF_INET: IPV4
	// SOCK_STREAM: 流类型
	// IPPROTO_TCP： TCP
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == _sock) {
		printf("ERROR: 建立socket失败\n");
		return 0;
	}
	else
	{
		printf("建立socket成功\n");
	}
	// 2. bind 绑定用于接受客户端连接的网络端口
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);  // 主机和网络上的端口不一样，需要转换
	_sin.sin_addr.S_un.S_addr = INADDR_ANY;  // inet_addr("127.0.0.1")  INADDR_ANY代表本机任意地址都可以
	if (bind(_sock, (sockaddr*)&_sin, sizeof(_sin)) == SOCKET_ERROR) 
	{
		// 绑定失败
		printf("ERROR：绑定网络端口失败\n");
		return 0;
	}
	else 
	{
		printf("绑定端口成功\n");
	}
	// 3. listen 监听网络端口
	if (SOCKET_ERROR == listen(_sock, 5))
	{
		printf("ERROR：监听网络端口失败\n");
		return 0;
	}
	else 
	{
		printf("监听网络端口成功\n");
	}

	while (true)
	{
		fd_set fdRead;
		fd_set fdWrite;
		fd_set fdExp;

		FD_ZERO(&fdRead);
		FD_ZERO(&fdWrite);
		FD_ZERO(&fdExp);

		FD_SET(_sock, &fdRead);
		FD_SET(_sock, &fdWrite);
		FD_SET(_sock, &fdExp);
		
		for (int n = (int)g_clients.size() - 1; n >= 0; n--) {
			FD_SET(g_clients[n], &fdRead);
		}

		// nfds是整数值，是指fd_set集合中所有描述符(socket)的范围，而不是数量
		// 在windows上这个参数可以写0，没意义
		timeval t = { 0, 0 };
		int ret = select(_sock + 1, &fdRead, &fdWrite, &fdExp, &t);
		if (ret < 0)
		{
			printf("select任务结束\n");
			break;
		}
		if (FD_ISSET(_sock, &fdRead))
		{
			FD_CLR(_sock, &fdRead);
			// 4. accept 等待接受客户端连接
			sockaddr_in clientAddr = {};
			int nAddrLen = sizeof(sockaddr_in);
			SOCKET _cSock = INVALID_SOCKET;

			_cSock = accept(_sock, (sockaddr*)&clientAddr, &nAddrLen);
			if (INVALID_SOCKET == _cSock)
			{
				printf("ERROR：接受到无效客户端SOCKET\n");
			}
			g_clients.push_back(_cSock);
			printf("新客户端加入: IP = %s\n", inet_ntoa(clientAddr.sin_addr));
		}

		for (int n =  0; n < fdRead.fd_count; n++) {
			if (-1 == processor(fdRead.fd_array[n]))
			{
				auto iter = find(g_clients.begin(), g_clients.end(), fdRead.fd_array[n]);
				if (iter != g_clients.end())
				{
					g_clients.erase(iter);
				}
			}
		}
	}
	// 8. 关闭套接字 closesocket
	for (int n = 0; n < g_clients.size() - 1; n++) {
		closesocket(g_clients[n]);
	}
	

	WSACleanup();  // 清除windows socket环境

	printf("退出, 任务结束\n");

	getchar();
	return 0;
}