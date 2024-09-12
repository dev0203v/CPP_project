#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define BUF_SIZE 100
#define NAME_SIZE 20
using namespace std;

void *send_msg(void *arg);
void *recv_msg(void *arg);
void error_handling(const char *msg);

char name[NAME_SIZE] = "[DEFAULT]";
char msg[BUF_SIZE];

int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in serv_addr;
    pthread_t snd_thread, rcv_thread;
    void *thread_return;
    if (argc != 3)
    {
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    sprintf(name, "[%s]", argv[3]);
    sock = socket(PF_INET, SOCK_STREAM, 0);

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("connect() error");

    pthread_create(&snd_thread, NULL, send_msg, (void *)&sock);
    pthread_create(&rcv_thread, NULL, recv_msg, (void *)&sock);
    pthread_join(snd_thread, &thread_return);
    pthread_join(rcv_thread, &thread_return);
    close(sock);
    return 0;
}

void *send_msg(void *arg)
{
    int flag = 0;
    int sock = *((int *)arg);
    char name_msg[NAME_SIZE + BUF_SIZE];
    string ligin[3] = {"ID 입력", "PW 입력", "고유번호 입력"};
    string join[4] = {"아이디를 입력하세요.", "비밀번호를 입력하세요.", "이름을 입력하세요.", "닉네임을 입력하세요."};

    while (1)
    {
        cout << "1번 : 로그인, 2번 : 회원가입 ";
        fgets(msg, BUF_SIZE, stdin);
        if (!strcmp(msg, "q\n") || !strcmp(msg, "Q\n"))
        {
            close(sock);
            exit(0);
        }
        write(sock, msg, strlen(msg)); // 로그인 회원가입 번호 write

        if (!strcmp(msg, "1\n"))
        {
            cout << "로그인 화면입니다" << endl;
            for (int i = 0; i < 2; i++)
            {
                memset(msg, 0, BUF_SIZE);
                cout << ligin[i] << ": ";
                cin.getline(msg, BUF_SIZE);

                if (write(sock, msg, strlen(msg)) <= 0)
                {
                    cout << "Error" << endl;
                    break;
                }
            }

            while (1)
            {
                memset(msg, 0, sizeof(msg));
                int len = read(sock, msg, sizeof(msg));
                std::string server_response(msg, len);

                if (server_response == "로그인 실패. 재시도하세요.")
                {
                    cout << server_response << endl;
                    for (int i = 0; i < 2; i++)
                    {
                        memset(msg, 0, BUF_SIZE);
                        cout << ligin[i] << ": ";
                        cin.getline(msg, BUF_SIZE);

                        if (write(sock, msg, strlen(msg)) <= 0)
                        {
                            cout << "Error" << endl;
                            break;
                        }
                    }
                }
                else if (server_response == "5회 실패. 고유번호를 입력하세요.")
                {
                    cout << server_response << endl;
                    for (int i = 0; i < 3; i++)
                    {
                        memset(msg, 0, BUF_SIZE);
                        cout << ligin[i] << ": ";
                        cin.getline(msg, BUF_SIZE);

                        if (write(sock, msg, strlen(msg)) <= 0)
                        {
                            cout << "Error" << endl;
                            break;
                        }
                    }
                }
                else if (server_response.find("로그인 성공") != std::string::npos)
                {
                    cout << "로그인 성공" << endl;
                    memset(msg, 0, sizeof(msg));
                    int uno = read(sock, msg, sizeof(msg));
                    std::string userno(msg);
                    memset(msg, 0, sizeof(msg));
                    int well = read(sock, msg, sizeof(msg));
                    std::string wcome(msg);
                    std::cout << "고유번호는 " << userno << "번 입니다." << std::endl;

                    // memset(msg, 0, sizeof(msg));
                    // int well = read(sock, msg, sizeof(msg));
                    // std::string wcome(msg);
                    std::cout << "환영합니다 " << wcome << " 님" << std::endl;
                    flag = 1;
                    break;
                }
                else
                {
                    cout << server_response << endl;
                    // break;
                }
            }
            cout << "while문 나옴" << endl;
        }
        else if (!strcmp(msg, "2\n"))
        {
            cout << "회원가입 창입니다." << endl;
            for (int i = 0; i < 4; i++)
            {
                memset(msg, 0, BUF_SIZE);
                cout << join[i] << ": ";
                cin.getline(msg, BUF_SIZE);

                if (write(sock, msg, strlen(msg)) <= 0)
                {
                    cout << "Error" << endl;
                    break;
                }
            }
            memset(msg, 0, sizeof(msg));
            int len = read(sock, msg, sizeof(msg));
            if (msg[0] == '1')
            {
                cout << "중복 아이디입니다 다시 입력해주세요" << endl;
                for (int i = 0; i < 3; i++)
                {
                    memset(msg, 0, BUF_SIZE);
                    cout << join[i] << ": ";
                    cin.getline(msg, BUF_SIZE);

                    if (write(sock, msg, strlen(msg)) <= 0)
                    {
                        cout << "Error" << endl;
                        break;
                    }
                }
            }
        }
        if (flag == 1) // 로그인 성공시 진입
        {
            cout << "들어옴" << endl;
            //   memset(msg, 0, BUF_SIZE);
            //   int len = read(sock, msg, sizeof(msg));
            //   string server_response(msg, len);
            //   cout << server_response << endl;
            cout << "1번 : 3번 : 랜덤채팅 4번 : 그룹채팅" << endl;
            while (1)
            {
                memset(msg, 0, BUF_SIZE);
                sleep(1);
                fgets(msg, BUF_SIZE, stdin);
                if (!strcmp(msg, "p\n") || !strcmp(msg, "P\n"))
                {
                    close(sock);
                    exit(0);
                }
                // sprintf(name_msg, "%s %s", name, msg);
                write(sock, msg, strlen(msg));
            }
        }
    }

    return NULL;
}

void *recv_msg(void *arg)
{
    int sock = *((int *)arg);
    // char name_msg[NAME_SIZE + BUF_SIZE];
    int str_len;
    sleep(5);
    while (1)
    {
        memset(msg, 0, BUF_SIZE);
        str_len = read(sock, msg, sizeof(msg));
        if (str_len == -1)
            return (void *)-1;
        msg[str_len] = 0;
        fputs(msg, stdout);
    }
    return NULL;
}

void error_handling(const char *msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}

////////////////////////////////
// void *recv_msg(void *arg)
// {
//     int sock = *((int *)arg);
//     int str_len;
//     while (1)
//     {
//         memset(msg, 0, BUF_SIZE);
//         str_len = read(sock, msg, sizeof(msg));
//         if (str_len == -1)
//         {
//             // 클라이언트가 연결을 끊었을 때의 처리
//             cout << "서버와의 연결이 종료되었습니다." << endl;
//             close(sock);
//             exit(0);
//         }
//         else if (str_len == 0) // 클라이언트가 정상적으로 종료됐을 때의 처리
//         {
//             cout << "서버와의 연결이 정상적으로 종료되었습니다." << endl;
//             close(sock);
//             exit(0);
//         }
//         msg[str_len] = 0;
//         fputs(msg, stdout);
//     }
//     return NULL;
// }