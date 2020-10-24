#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <windows.h>
#include <WinSock2.h>
#include <stdio.h>

//#pragma comment(lib, "ws2_32.lib")  // 指明需要使用的动态链接库，只有vc++支持这种写法, 也可以在属性里链接库输入里配置

struct DataPackage
{
	int age;
	char name[32];
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
		else {
			// 5. 向服务器发送请求
			send(_sock, cmdBuf, sizeof(cmdBuf), 0);
		}

		// 6. 接收服务器信息 recv
		char recvBuf[128] = {0};
		int nLen = recv(_sock, recvBuf, sizeof(recvBuf), 0);
		if (nLen > 0)
		{
			DataPackage* dp = (DataPackage*)recvBuf;
			printf("接收到数据：年龄%d 姓名%s\n", dp->age, dp->name);
		}
	}
	
	// 7. 关闭套接字 closesocket
	closesocket(_sock);

	WSACleanup();  // 清除windows socket环境
	
	printf("退出, 任务结束\n");

	getchar();

	return 0;
}