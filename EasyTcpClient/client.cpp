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

	// 建立一个tcp客户端
	// 1. 建立一个socket
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == _sock) {
		printf("ERROR: 建立socket失败\n");
		return 0;
	}
	else 
	{
		printf("建立socket成功\n");
	}
	// 2. 连接服务端 connect
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);  
	_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if (SOCKET_ERROR == connect(_sock, (sockaddr*)&_sin, sizeof(_sin))) {
		printf("ERROR: 连接服务器错误\n");
		return 0;
	} 
	else
	{
		printf("连接服务器成功\n");
	}
	
	while (true)
	{
		// 3. 输入请求命令
		char cmdBuf[128] = {0};
		scanf_s("%s", cmdBuf, sizeof(cmdBuf));
		// 4. 处理请求
		if (0 == strcmp(cmdBuf, "exit"))
		{
			break;
		} 
		else if (0 == strcmp(cmdBuf, "login"))
		{
			Login login = {"drw", "123456"};
			DataHeader dh = { sizeof(login), CMD_LOGIN};
			// 5. 向服务器发送请求
			send(_sock, (const char*)&dh, sizeof(dh), 0);
			send(_sock, (const char*)&login, sizeof(login), 0);
			// 接收服务器返回数据
			DataHeader retHeader = {};
			LoginResult loginRet = {};
			recv(_sock, (char*)&retHeader, sizeof(DataHeader), 0);
			recv(_sock, (char*)&loginRet, sizeof(LoginResult), 0);
			printf("LoginResult: %d", loginRet.result);
		}
		else if (0 == strcmp(cmdBuf, "logout"))
		{
			Logout logout = { "drw" };
			DataHeader dh = { sizeof(logout), CMD_LOGOUT };
			// 5. 向服务器发送请求
			send(_sock, (const char*)&dh, sizeof(dh), 0);
			send(_sock, (const char*)&logout, sizeof(logout), 0);
			// 接收服务器返回数据
			DataHeader retHeader = {};
			LogoutResult logoutRet = {};
			recv(_sock, (char*)&retHeader, sizeof(DataHeader), 0);
			recv(_sock, (char*)&logoutRet, sizeof(LogoutResult), 0);
			printf("LogoutResult: %d\n", logoutRet.result);
		} 
		else {
			printf("不支持的命令，请重新输入\n");
		}
	}
	
	// 7. 关闭套接字 closesocket
	closesocket(_sock);

	WSACleanup();  // 清除windows socket环境
	
	printf("退出, 任务结束\n");

	getchar();

	return 0;
}