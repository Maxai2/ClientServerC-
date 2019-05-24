#include "pch.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <map>
#include <list>
//#include <thread>

#pragma comment(lib, "ws2_32.lib")
using namespace std;

#define BUFSIZE     1024 
#define SERVER_PORT 54000

list<string> clientList;

map<string, string> sbw;

void connectMail(string mail)
{   
    clientList.push_back(mail);
}

string onlineListShow(string selfMail)
{
    string onlineMails = "";

    for (const auto &m : clientList)
    {
        if (m != selfMail)
        {
            onlineMails += m + '|';
        }
    }

    return onlineMails;
}

int main(void)
{
    WSAData data;
    int wsResult = WSAStartup(514, &data);
    if (wsResult != 0)
    {
        cerr << "WSAStartup failed, Err #" << wsResult << endl;
        cin.get();
        return 0;
    }

    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == INVALID_SOCKET)
    {
        cerr << "Can't create a socket, Err #" << WSAGetLastError() << endl;
        cin.get();
        WSACleanup();
        return 0;
    }

    int OptVal = 1;
    int OptLen = sizeof(int);
    int rc = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&OptVal, OptLen);
    if (rc == SOCKET_ERROR)
    {
        cerr << "Can't set a sockopt, Err #" << WSAGetLastError() << endl;
        cin.get();
        WSACleanup();
        return 0;
    }
    else
        cout << "Socket option SO_REUSEADDR is ON" << endl;

    struct sockaddr_in server_addr;
    int server_len = sizeof(server_addr);
    ZeroMemory(&server_addr, server_len);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.S_un.S_addr = INADDR_ANY;

    
    rc = bind(sock, (struct sockaddr*)&server_addr, server_len);
    if (rc == SOCKET_ERROR)
    {
        cerr << "Can't bind a socket, Err #" << WSAGetLastError() << endl;
        cin.get();
        WSACleanup();
        return 0;
    }

    struct sockaddr_in client_addr;
    int client_len = sizeof(client_addr);
    ZeroMemory(&client_addr, client_len);

    char buf[BUFSIZE];

    while (true)
    {
        ZeroMemory(buf, BUFSIZE);

        int bytesRecieved = recvfrom(sock, buf, BUFSIZE, 0, (struct sockaddr*)&client_addr, &client_len);
        if (bytesRecieved == SOCKET_ERROR)
        {
            cerr << "Error recieving from client" << WSAGetLastError() << endl;
            continue;
        }
        
        string bufByString = string(buf);
        cout << bufByString << endl;

        string com = bufByString.substr(0, bufByString.find('|'));

        string param = bufByString.substr(bufByString.find('|') + 1);

        if (com == "connect")
        {
            connectMail(param);

            string senm = "Connect";
            sendto(sock, senm.c_str(), BUFSIZE, 0, (sockaddr *)&client_addr, client_len);
        }
        else if (com == "show")
        {
            string senm = onlineListShow(param);
            if (senm == "")
                senm = "empty";
            sendto(sock, senm.c_str(), BUFSIZE, 0, (sockaddr *)&client_addr, client_len);
        }
        else if (com == "send")
        {
            string mail = param.substr(0, param.find(':'));

            string from = param.substr(0, param.find('~')).substr(param.find(':'));

            string msg = mail + '|' + from + '~' + param.substr(param.find('~') + 1);

            sbw[mail] += msg.c_str();
        }
        else if (com == "wait")
        {
            string message = sbw[param];
            sbw[param] = "";
            sendto(sock, message.c_str(), BUFSIZE, 0, (struct sockaddr*)&client_addr, client_len);
        }
    }

    cin.get();
    closesocket(sock);
    WSACleanup();
    return 0;
}