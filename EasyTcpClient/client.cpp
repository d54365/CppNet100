#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <windows.h>
#include <WinSock2.h>
#include <stdio.h>

//#pragma comment(lib, "ws2_32.lib")  // ָ����Ҫʹ�õĶ�̬���ӿ⣬ֻ��vc++֧������д��, Ҳ���������������ӿ�����������

enum CMD
{
	CMD_LOGIN,
	CMD_LOGOUT,
	CMD_ERROR,
};

struct DataHeader
{
	short dataLength;  // ���ݳ���
	short cmd;   // ����
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
	WSAStartup(ver, &dat);  // ����windows socket 2.x���绷��

	// ����һ��tcp�ͻ���
	// 1. ����һ��socket
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == _sock) {
		printf("ERROR: ����socketʧ��\n");
		return 0;
	}
	else 
	{
		printf("����socket�ɹ�\n");
	}
	// 2. ���ӷ���� connect
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);  
	_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if (SOCKET_ERROR == connect(_sock, (sockaddr*)&_sin, sizeof(_sin))) {
		printf("ERROR: ���ӷ���������\n");
		return 0;
	} 
	else
	{
		printf("���ӷ������ɹ�\n");
	}
	
	while (true)
	{
		// 3. ������������
		char cmdBuf[128] = {0};
		scanf_s("%s", cmdBuf, sizeof(cmdBuf));
		// 4. ��������
		if (0 == strcmp(cmdBuf, "exit"))
		{
			break;
		} 
		else if (0 == strcmp(cmdBuf, "login"))
		{
			Login login = {"drw", "123456"};
			DataHeader dh = { sizeof(login), CMD_LOGIN};
			// 5. ���������������
			send(_sock, (const char*)&dh, sizeof(dh), 0);
			send(_sock, (const char*)&login, sizeof(login), 0);
			// ���շ�������������
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
			// 5. ���������������
			send(_sock, (const char*)&dh, sizeof(dh), 0);
			send(_sock, (const char*)&logout, sizeof(logout), 0);
			// ���շ�������������
			DataHeader retHeader = {};
			LogoutResult logoutRet = {};
			recv(_sock, (char*)&retHeader, sizeof(DataHeader), 0);
			recv(_sock, (char*)&logoutRet, sizeof(LogoutResult), 0);
			printf("LogoutResult: %d\n", logoutRet.result);
		} 
		else {
			printf("��֧�ֵ��������������\n");
		}
	}
	
	// 7. �ر��׽��� closesocket
	closesocket(_sock);

	WSACleanup();  // ���windows socket����
	
	printf("�˳�, �������\n");

	getchar();

	return 0;
}