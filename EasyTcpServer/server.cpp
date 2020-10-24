#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <windows.h>
#include <WinSock2.h>
#include <stdio.h>

//#pragma comment(lib, "ws2_32.lib")  // ָ����Ҫʹ�õĶ�̬���ӿ⣬ֻ��vc++֧������д��, Ҳ���������������ӿ�����������

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
	short m_dataLength;  // ���ݳ���
	short m_cmd;   // ����

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

	while (true)
	{
		DataHeader header = {};
		// 5. ���տͻ�������
		int nLen = recv(_cSock, (char*)&header, sizeof(DataHeader), 0);
		if (nLen <= 0)
		{
			printf("�ͻ������˳�, �������\n");
			break;
		}
		// 6. ��������
		
		switch (header.m_cmd)
		{
		case CMD_LOGIN:
		{
			Login login = {};
			recv(_cSock, (char*)&login + sizeof(DataHeader), sizeof(Login) - sizeof(DataHeader), 0);
			printf("���յ�������: %d, ���ݳ��ȣ�%d, userName: %s, password: %s\n", header.m_cmd, header.m_dataLength, login.userName, login.password);
			LoginResult ret;
			send(_cSock, (char*)&ret, sizeof(LoginResult), 0);
			break;
		}
		case CMD_LOGOUT:
		{
			Logout logout = {};
			recv(_cSock, (char*)&logout + sizeof(DataHeader), sizeof(Logout) - sizeof(DataHeader), 0);
			printf("���յ�������: %d, ���ݳ��ȣ�%d, userName: %s\n", header.m_cmd, header.m_dataLength, logout.userName);
			LogoutResult logoutRet;
			send(_cSock, (char*)&logoutRet, sizeof(LogoutResult), 0);
			break;
		}
		default:
		{
			printf("���յ�������: %d, ���ݳ��ȣ�%d\n", header.m_cmd, header.m_dataLength);
			header.m_cmd = CMD_ERROR;
			header.m_dataLength = 0;
			send(_cSock, (char*)&header, sizeof(DataHeader), 0);
			break;
		}
		}
	}
	// 8. �ر��׽��� closesocket
	closesocket(_sock);

	WSACleanup();  // ���windows socket����

	printf("�˳�, �������\n");

	getchar();
	return 0;
}