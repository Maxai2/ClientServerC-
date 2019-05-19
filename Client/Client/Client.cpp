#include "pch.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <list>

#pragma comment(lib, "ws2_32.lib")
using namespace std;

#define BUFSIZE     1024 
#define SERVER_PORT 54000

list<string> splitString(string tempString)
{
    list<string> onlineList;

    for (int i = 0; i < tempString.length(); i++)
    {
        string str = "";

        for (int j = i; j < tempString.length(); j++, i++)
        {
            if (tempString[j] == '|')
            {   
                onlineList.push_back(str);
                break;
            }
            str += tempString[j];
        }
    }

    return onlineList;
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

    cout << "IP adress of server : ";
    string ipAdress;
    getline(cin, ipAdress);

    sockaddr_in server_addr, client_addr;

    int server_len = sizeof(server_addr);
    ZeroMemory(&server_addr, server_len);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, ipAdress.c_str(), &server_addr.sin_addr);

    int client_len = sizeof(client_addr);
    ZeroMemory(&client_addr, client_len);
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(0);
    client_addr.sin_addr.S_un.S_addr = INADDR_ANY;

    int rc = bind(sock, (struct sockaddr*)&client_addr, client_len);
    if (rc == SOCKET_ERROR)
    {
        cerr << "Can't bind a socket, Err #" << WSAGetLastError() << endl;
        cin.get();
        WSACleanup();
        return 0;
    }

    char buf[BUFSIZE];
    string userInput, message;
    
    cout << "Input mail : ";
    string mail;
    getline(cin, mail);

    message = "connect|" + mail;

    sendto(sock, message.c_str(), BUFSIZE, 0, (sockaddr*)&server_addr, server_len);

    recvfrom(sock, buf, BUFSIZE, 0, (struct sockaddr*)&server_addr, &server_len);

    cout << buf << endl << endl << "------------------------------------";

    while (true)
    {
        cout << endl << "1) show online" << endl;
        cout << "2) send mail" << endl;
        cout << "3) recieve" << endl;
        cout << "Input choice: ";
        short selNum;
        cin >> selNum;

        switch (selNum)
        {
            case 1:
            {
                message = "show|" + mail;

                sendto(sock, message.c_str(), BUFSIZE, 0, (sockaddr*)&server_addr, server_len);

                recvfrom(sock, buf, BUFSIZE, 0, (struct sockaddr*)&server_addr, &server_len);

                if ((string)buf == "empty")
                {
                    cout << endl << "No user(s) online" << endl;
                    continue;
                }

                list<string> userList = splitString(buf);

                cout << endl << "Online users: " << endl;
                for (string u : userList)
                {
                    cout << u << endl;
                }

                cout << "------------------------------------" << endl;
            }
                break;
            case 2:
            {
                message = "show|" + mail;

                sendto(sock, message.c_str(), BUFSIZE, 0, (sockaddr*)&server_addr, server_len);

                recvfrom(sock, buf, BUFSIZE, 0, (struct sockaddr*)&server_addr, &server_len);

                if ((string)buf == "empty")
                {
                    cout << endl << "No user(s) online" << endl;
                    continue;
                }

                list<string> userList = splitString(buf);

                cout << endl << "Online user(s): " << endl;
                for (string u : userList)
                {
                    cout << u << endl;
                }

                cout << "------------------------------------" << endl << endl;

                cin.ignore();
                cout << "Write user mail to send: ";
                string chatUser;
                getline(cin, chatUser);

                cout << "Text message: ";
                string msg;
                getline(cin, msg);

                message = "send|" + chatUser + '~' + msg;

                sendto(sock, message.c_str(), BUFSIZE, 0, (sockaddr*)&server_addr, server_len);

                recvfrom(sock, buf, BUFSIZE, 0, (struct sockaddr*)&server_addr, &server_len);

                cout << (string)buf << endl;
            }
                break;
            case 3:
            {
                cout << endl << "Wait to recieve message > ";
                recvfrom(sock, buf, BUFSIZE, 0, (struct sockaddr*)&server_addr, &server_len);

                string bufstr = string(buf);

                string from = bufstr.substr(0, bufstr.find('|'));
                string msg = bufstr.substr(bufstr.find('|') + 1);

                cout << endl << "From: " << from << endl;
                cout << "Message: " << msg << endl;
            }
                break;
        }
    }
          
    do
    {
        cout << "> ";
        getline(cin, userInput);

        if (userInput.size() > 0)
        {
            int sendResult = sendto(sock, userInput.c_str(), BUFSIZE, 0, (sockaddr*)&server_addr, server_len);
            if (sendResult == SOCKET_ERROR)
            {
                cerr << "Can't send msg, Err #" << WSAGetLastError() << endl;
                cin.get();
                return 0;
            }
        }
    } while (userInput.size() > 0);

    cin.get();
    closesocket(sock);
    WSACleanup();
    return 0;
}