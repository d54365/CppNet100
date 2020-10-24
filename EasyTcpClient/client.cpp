#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <windows.h>
#include <WinSock2.h>
#include <stdio.h>

//#pragma comment(lib, "ws2_32.lib")  // ָ����Ҫʹ�õĶ�̬���ӿ⣬ֻ��vc++֧������д��, Ҳ���������������ӿ�����������

struct DataPackage
{
	int age;
	char name[32];
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
		else {
			// 5. ���������������
			send(_sock, cmdBuf, sizeof(cmdBuf), 0);
		}

		// 6. ���շ�������Ϣ recv
		char recvBuf[128] = {0};
		int nLen = recv(_sock, recvBuf, sizeof(recvBuf), 0);
		if (nLen > 0)
		{
			DataPackage* dp = (DataPackage*)recvBuf;
			printf("���յ����ݣ�����%d ����%s\n", dp->age, dp->name);
		}
	}
	
	// 7. �ر��׽��� closesocket
	closesocket(_sock);

	WSACleanup();  // ���windows socket����
	
	printf("�˳�, �������\n");

	getchar();

	return 0;
}