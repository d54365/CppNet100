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
	// ����һ��tcp�����
	// 1. ����һ��socket
	// AF_INET: IPV4
	// SOCK_STREAM: ������
	// IPPROTO_TCP�� TCP
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == _sock) {
		printf("ERROR: ����socketʧ��\n");
		return 0;
	}
	else
	{
		printf("����socket�ɹ�\n");
	}
	// 2. bind �����ڽ��ܿͻ������ӵ�����˿�
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);  // �����������ϵĶ˿ڲ�һ������Ҫת��
	_sin.sin_addr.S_un.S_addr = INADDR_ANY;  // inet_addr("127.0.0.1")  INADDR_ANY�����������ַ������
	if (bind(_sock, (sockaddr*)&_sin, sizeof(_sin)) == SOCKET_ERROR) 
	{
		// ��ʧ��
		printf("ERROR��������˿�ʧ��\n");
		return 0;
	}
	else 
	{
		printf("�󶨶˿ڳɹ�\n");
	}
	// 3. listen ��������˿�
	if (SOCKET_ERROR == listen(_sock, 5))
	{
		printf("ERROR����������˿�ʧ��\n");
		return 0;
	}
	else 
	{
		printf("��������˿ڳɹ�\n");
	}
	// 4. accept �ȴ����ܿͻ�������
	sockaddr_in clientAddr = {};
	int nAddrLen = sizeof(sockaddr_in);
	SOCKET _cSock = INVALID_SOCKET;
	
	_cSock = accept(_sock, (sockaddr*)&clientAddr, &nAddrLen);
	if (INVALID_SOCKET == _cSock)
	{
		printf("ERROR�����ܵ���Ч�ͻ���SOCKET\n");
	}
	printf("�¿ͻ��˼���: IP = %s\n", inet_ntoa(clientAddr.sin_addr));

	char _recvBuf[128] = { 0 };

	while (true)
	{
		// 5. ���տͻ�������
		int nLen = recv(_cSock, _recvBuf, sizeof(_recvBuf), 0);
		if (nLen <= 0)
		{
			printf("�ͻ������˳�, �������\n");
			break;
		}
		// 6. ��������
		printf("����: %s\n", _recvBuf);
		if (0 == strcmp(_recvBuf, "getInfo"))
		{
			DataPackage dp = {30, "����"};
			// 7. send ��ͻ��˷���һ������
			send(_cSock, (const char*)&dp, sizeof(DataPackage), 0);
		}
		else
		{
			char msgBuf[] = "Hello, I'm Sever.";
			// 7. send ��ͻ��˷���һ������
			send(_cSock, msgBuf, strlen(msgBuf) + 1, 0);
		}
	}
	// 8. �ر��׽��� closesocket
	closesocket(_sock);

	WSACleanup();  // ���windows socket����

	printf("�˳�, �������\n");

	getchar();
	return 0;
}