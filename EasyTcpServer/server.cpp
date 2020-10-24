#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <windows.h>
#include <WinSock2.h>
#include <stdio.h>

//#pragma comment(lib, "ws2_32.lib")  // 指明需要使用的动态链接库，只有vc++支持这种写法, 也可以在属性里链接库输入里配置

enum CMD
{
	CMD_LOGIN,
	CMD_LOGOUT,
	CMD_ERROR,
};

struct DataHeader
{
	short dataLength;  // 数据长度
	short cmd;   // 命令
};

struct Login
{
	char userName[32];
	char password[32];
};

struct LoginResult
{
	int result;
};

struct Logout
{
	char userName[32];
};

struct LogoutResult
{
	int result;
};

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
	// 4. accept 等待接受客户端连接
	sockaddr_in clientAddr = {};
	int nAddrLen = sizeof(sockaddr_in);
	SOCKET _cSock = INVALID_SOCKET;
	
	_cSock = accept(_sock, (sockaddr*)&clientAddr, &nAddrLen);
	if (INVALID_SOCKET == _cSock)
	{
		printf("ERROR：接受到无效客户端SOCKET\n");
	}
	printf("新客户端加入: IP = %s\n", inet_ntoa(clientAddr.sin_addr));

	while (true)
	{
		DataHeader header = {};
		// 5. 接收客户端数据
		int nLen = recv(_cSock, (char*)&header, sizeof(DataHeader), 0);
		if (nLen <= 0)
		{
			printf("客户端已退出, 任务结束\n");
			break;
		}
		// 6. 处理请求
		printf("接收命令: %d, 数据长度: %d\n", header.cmd, header.dataLength);
		
		switch (header.cmd)
		{
		case CMD_LOGIN:
		{
			Login login = {};
			recv(_cSock, (char*)&login, sizeof(Login), 0);
			LoginResult ret = { 1 };
			send(_cSock, (char*)&header, sizeof(DataHeader), 0);
			send(_cSock, (char*)&ret, sizeof(LoginResult), 0);
			break;
		}
		case CMD_LOGOUT:
		{
			Logout logout = {};
			recv(_cSock, (char*)&logout, sizeof(Logout), 0);
			LogoutResult logoutRet = { 1 };
			send(_cSock, (char*)&header, sizeof(DataHeader), 0);
			send(_cSock, (char*)&logoutRet, sizeof(LogoutResult), 0);
			break;
		}
		default:
		{
			header.cmd = CMD_ERROR;
			header.dataLength = 0;
			send(_cSock, (char*)&header, sizeof(DataHeader), 0);
			break;
		}
		}
	}
	// 8. 关闭套接字 closesocket
	closesocket(_sock);

	WSACleanup();  // 清除windows socket环境

	printf("退出, 任务结束\n");

	getchar();
	return 0;
}