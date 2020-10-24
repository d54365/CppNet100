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

	char _recvBuf[128] = { 0 };

	while (true)
	{
		// 5. 接收客户端数据
		int nLen = recv(_cSock, _recvBuf, sizeof(_recvBuf), 0);
		if (nLen <= 0)
		{
			printf("客户端已退出, 任务结束\n");
			break;
		}
		// 6. 处理请求
		printf("接收: %s\n", _recvBuf);
		if (0 == strcmp(_recvBuf, "getInfo"))
		{
			DataPackage dp = {30, "张三"};
			// 7. send 向客户端返回一条数据
			send(_cSock, (const char*)&dp, sizeof(DataPackage), 0);
		}
		else
		{
			char msgBuf[] = "Hello, I'm Sever.";
			// 7. send 向客户端返回一条数据
			send(_cSock, msgBuf, strlen(msgBuf) + 1, 0);
		}
	}
	// 8. 关闭套接字 closesocket
	closesocket(_sock);

	WSACleanup();  // 清除windows socket环境

	printf("退出, 任务结束\n");

	getchar();
	return 0;
}