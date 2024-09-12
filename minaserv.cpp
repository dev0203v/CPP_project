#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <mariadb/conncpp.hpp>

#define BUF_SIZE 100
#define MAX_CLNT 256
using namespace std;

void *handle_clnt(void *arg);
void rand_msg(const char *msg, int len);
void error_handling(const char *msg);
void addUser(std::string NAME, std::string NICNAME, std::string ID, std::string PW);
void Userlogin(string user_id, string user_pw, int clnt_sock);

int clnt_cnt = 0;
int clnt_socks[MAX_CLNT];
pthread_mutex_t mutx;

class DB
{
protected:
    sql::Connection *conn;

public:
    sql::PreparedStatement *prepareStatement(const string &query) // ? 쿼리문 삽입
    {
        sql::PreparedStatement *stmt(conn->prepareStatement(query));
        return stmt;
    }

    sql::Statement *createStatement() // 쿼리문 즉시 실행
    {
        sql::Statement *stm(conn->createStatement());
        return stm;
    }

    void connect()
    {
        try
        {

            sql::Driver *driver = sql::mariadb::get_driver_instance();
            sql::SQLString url("jdbc:mariadb://10.10.21.111:3306/CHAT");
            sql::Properties properties({{"user", "CHAT"}, {"password", "1"}});
            conn = driver->connect(url, properties);
            cout << "db호출 성공" << endl;

            // if (Reset() == 0)
            // {
            //     std::unique_ptr<sql::PreparedStatement> stmnt(conn->prepareStatement("UPDATE USER SET STATUS = '0', SOKET_NUM = NULL WHERE STATUS = '1' AND SOKET_NUM IS NOT NULL;"));

            //     stmnt->executeQuery(); // 서버 재부팅시 유저 미접속, 소켓번호 초기화
            // }
        }
        catch (sql::SQLException &e)
        {
            std::cerr << "Error Connecting to MariaDB Platform: " << e.what() << std::endl;
        }
    }

    void addUser(std::string NAME, std::string NICNAME, std::string ID, std::string PW)
    {
        try
        {
            std::unique_ptr<sql::PreparedStatement> stmnt(conn->prepareStatement("insert into USER values (default,?, ?, ?, ?, 0, 0, NULL)"));
            stmnt->setString(1, NAME);
            stmnt->setString(2, NICNAME);
            stmnt->setString(3, ID);
            stmnt->setString(4, PW);
            stmnt->executeQuery();
        }
        catch (sql::SQLException &e)
        {
            std::cerr << "Error inserting new task: " << e.what() << std::endl;
        }
    }

    void Userlogin(string user_id, int clnt_sock)
    {
        // int count = login_attempts[user_id]; // 현재 사용자의 로그인 시도 횟수를 가져옵니다.
        try
        {
            cout << "로그인 접근" << endl;
            cout << user_id << endl;

            string clnt_sock_no = to_string(clnt_sock);
            cout << "소켓 번호" << clnt_sock << endl;

            std::unique_ptr<sql::PreparedStatement> stmnt9(conn->prepareStatement("UPDATE USER SET STATUS = '0', SOKET_NUM = NULL  WHERE STATUS = '1' AND USER_ID = '" + user_id + "';"));
            stmnt9->executeQuery(); // 유저 접속시 접속중으로 바꿈

            std::unique_ptr<sql::PreparedStatement> stmnt(conn->prepareStatement("UPDATE USER SET STATUS = '1', SOKET_NUM = '" + clnt_sock_no + "' WHERE STATUS = '0' AND USER_ID = '" + user_id + "' AND SOKET_NUM IS NULL"));
            stmnt->executeQuery(); // 유저 접속시 접속중으로 바꿈
            return;
        }
        catch (sql::SQLException &e)
        {
            std::cerr << "Error inserting new task: " << e.what() << std::endl;
        }
    }

    bool attemptLogin(string user_id, string user_pw, int clnt_sock)
    {
        try
        {
            string userno2, set2;
            std::unique_ptr<sql::Statement> stmnt(conn->createStatement());
            sql::ResultSet *res = stmnt->executeQuery("SELECT * FROM USER");

            while (res->next())
            {
                if (res->getString(4) == user_id && res->getString(5) == user_pw)
                {
                    cout << "어탬프" << endl;
                    userno2 = res->getString(1);
                    set2 = res->getString(3);

                    string message = "로그인 성공";
                    write(clnt_sock, message.c_str(), message.size());
                    // sleep(1);
                    usleep(100);
                    write(clnt_sock, userno2.c_str(), userno2.size());
                    std::cout << userno2 << " 고유번호 전송." << endl;
                    // sleep(1);
                    usleep(100);
                    write(clnt_sock, set2.c_str(), set2.size());
                    std::cout << set2 << " 님이 접속하셨습니다." << endl;

                    return true; // 1
                }
            }
        }
        catch (sql::SQLException &e)
        {
            std::cerr << "Error during login attempt: " << e.what() << std::endl;
        }

        return false; // 0
    }

    bool Reset()
    {
        try
        {
            std::unique_ptr<sql::Statement> stmnt(conn->createStatement());
            sql::ResultSet *res = stmnt->executeQuery("SELECT * FROM USER");
            while (res->next())
            {
                if (res->getString(6) == "1")
                {
                    return true; // 1
                }
            }
        }
        catch (sql::SQLException &e)
        {
            std::cerr << "Error during login attempt: " << e.what() << std::endl;
        }

        return false; // 0
    }

    ~DB() { delete conn; }
};

class USER
{
private:
    DB &USERDB;
    string user_id, user_pw;

public:
    USER(DB &db) : USERDB(db) {}

    void longin(int clnt_sock)
    {
        int str_len = 0;
        char msg[BUF_SIZE];
        try
        {
            while ((str_len = read(clnt_sock, msg, sizeof(msg))) != 0)
            {
                // std::cout << msg;
                if (!strcmp(msg, "1\n"))
                {
                    int count = 0;
                    while (1)
                    {
                        ++count;
                        cout << "로그 루프" << count << endl;
                        memset(msg, 0, BUF_SIZE);
                        read(clnt_sock, msg, sizeof(msg)); // 아이디 READ
                        user_id = std::string(msg);
                        memset(msg, 0, BUF_SIZE);
                        read(clnt_sock, msg, sizeof(msg)); // 패스워드 READ
                        user_pw = std::string(msg);
                        memset(msg, 0, BUF_SIZE);
                        int logbool = USERDB.attemptLogin(user_id, user_pw, clnt_sock);
                        cout << "로그 불" << logbool << endl;
                        if (logbool == true)
                        {
                            cout << "로그인 성공" << endl;
                            USERDB.Userlogin(user_id, clnt_sock);
                            cout << "로그인 이후 기능" << endl;
                            while (1)
                            {
                                // string mge = "3번 : 랜덤채팅";
                                // write(clnt_sock, mge.c_str(), mge.size()); // 선택지 보여줌
                                memset(msg, 0, BUF_SIZE);
                                read(clnt_sock, msg, sizeof(msg)); // 고른 번호 입력받기
                                // int lenn = read(clnt_sock, msg, sizeof(msg));
                                // std::string client_response(msg, lenn);

                                if (!strcmp(msg, "1\n"))
                                {
                                }
                                else if (!strcmp(msg, "2\n"))
                                {
                                }
                                else if (!strcmp(msg, "3\n"))
                                {
                                    cout << "간다간다" << endl;
                                    Randchat(clnt_sock);
                                }
                                else if (!strcmp(msg, "4\n"))
                                {
                                    cout << "그룹챗그룹챗" << endl;
                                    group_chat(clnt_sock);
                                }
                                else
                                {
                                    break;
                                }
                            }
                            break;
                        }
                        else if (count < 5)
                        {
                            cout << "로그인 실패" << count << endl;
                            string message = "로그인 실패. 재시도하세요.";
                            write(clnt_sock, message.c_str(), message.size());
                        }
                        else
                        {
                            cout << "로그인 실패, 고유번호" << endl;
                            string message = "5회 실패. 고유번호를 입력하세요.";
                            write(clnt_sock, message.c_str(), message.size());
                        }
                    }
                }

                // if (!strcmp(msg, "2\n")) /// 랜덤채팅시 문제로 잠깐 바꿈
                if (!strcmp(msg, "9\n"))
                {
                    cout << "2번 선택" << endl;
                    string id, pw, NAME, NICNAME;
                    for (int i = 0; i < 4; i++)
                    {
                        str_len = read(clnt_sock, msg, sizeof(msg));
                        msg[str_len] = '\0';

                        if (i == 0)
                        {
                            id = string(msg);
                        }
                        else if (i == 1)
                        {
                            pw = string(msg);
                        }
                        else if (i == 2)
                        {
                            NAME = string(msg);
                        }
                        else if (i == 3)
                        {
                            NICNAME = string(msg);
                        }
                    }

                    std::unique_ptr<sql::Statement> stmnt(USERDB.createStatement());
                    sql::ResultSet *res = stmnt->executeQuery("select * from USER");

                    while (res->next())
                    {
                        if (id == res->getString(4))
                        {
                            string check = "1";
                            write(clnt_sock, check.c_str(), check.size());

                            for (int i = 0; i < 4; i++)
                            {
                                str_len = read(clnt_sock, msg, sizeof(msg));
                                msg[str_len] = '\0';

                                if (i == 0)
                                {
                                    id = string(msg);
                                }
                                else if (i == 1)
                                {
                                    pw = string(msg);
                                }
                                else if (i == 2)
                                {
                                    NAME = string(msg);
                                }
                                else if (i == 3)
                                {
                                    NICNAME = string(msg);
                                }
                            }
                            break;
                        }
                    }

                    // string check = "0";
                    // write(clnt_sock, check.c_str(), check.size());
                    USERDB.addUser(NAME, NICNAME, id, pw);
                }
            }
        }
        catch (sql::SQLException &e)
        {
            std::cerr << "Error Connecting to MariaDB Platform: " << e.what() << std::endl;
        }
    }

    void Randchat(int clnt_sock)
    {
        cout << "랜챗 진입2" << endl;
        // char msg[BUF_SIZE];
        int chat_socks[2];
        char msg[BUF_SIZE];
        string message;
        try
        {
            // string rand_message = "랜덤채팅입니다.";
            // write(clnt_sock, rand_message.c_str(), rand_message.size());

            string str1 = to_string(clnt_sock);
            std::unique_ptr<sql::Statement> stmnt(USERDB.createStatement());
            sql::ResultSet *res = stmnt->executeQuery("SELECT USER_NO, USER_NAME, USER_ID, SOKET_NUM FROM USER WHERE RAND_CHAT = 1 AND SOKET_NUM != '" + str1 + "' ORDER BY RAND();");
            cout << "랜덤 쿼리" << endl;
            // string rensponseT = "1번 :    2번 : 다   3번 : 대";
            // write(clnt_sock, rensponseT.c_str(), rensponseT.size());

            pthread_mutex_lock(&mutx);
            chat_socks[0] = clnt_sock;
            pthread_mutex_unlock(&mutx);

            string mge = "1번 : 랜덤 상대 연결   2번 : 다음 상대 찾기   3번 : 대화 종료";
            write(clnt_sock, mge.c_str(), mge.size());

            while (res->next())
            {
                cout << "랜챗 진입1" << endl;
                string rensponse = "1번 : 랜덤 상대 연결   2번 : 다음 상대 찾기   3번 : 대화 종료";
                write(clnt_sock, rensponse.c_str(), rensponse.size());
                memset(msg, 0, BUF_SIZE);

                // int lenn = read(clnt_sock, msg, sizeof(msg));
                // std::string client_response(msg, lenn);
                read(clnt_sock, msg, sizeof(msg));

                int connect_sock = res->getInt(4);
                pthread_mutex_lock(&mutx);
                chat_socks[1] = connect_sock;
                pthread_mutex_unlock(&mutx);

                // if (connect_sock != clnt_sock)
                // {
                if (!strcmp(msg, "1\n"))
                {
                    int str_len = 0;
                    message = "랜덤 채팅 상대를 찾았습니다. (종료하려면 q or Q)";
                    write(clnt_sock, message.c_str(), message.size());
                    write(connect_sock, message.c_str(), message.size());
                    while (1)
                    {
                        int len = read(clnt_sock, msg, BUF_SIZE);
                        string ranchat_response(msg, len);
                        if (ranchat_response == "q" || ranchat_response == "Q")
                            break;
                        write(connect_sock, msg, len);

                        int len2 = read(connect_sock, msg, BUF_SIZE);
                        string ranchat_response2(msg, len2);
                        if (ranchat_response2 == "q" || ranchat_response2 == "Q")
                            break;
                        write(clnt_sock, msg, len2);
                    }

                    // while ((str_len = read(clnt_sock, msg, sizeof(msg))) != 0)
                    // {
                    //     rand_msg(msg, str_len);
                    // }
                    break;
                }
                else if (!strcmp(msg, "2\n"))
                {
                    cout << "다음 상대를 찾습니다" << endl;
                    continue;
                }
                else if (!strcmp(msg, "3\n"))
                {
                    message = "대화를 종료합니다.";
                    write(clnt_sock, message.c_str(), message.size());
                    break;
                }
                // }
            }
        }
        catch (sql::SQLException &e)
        {
            std::cerr << "Error during random chat: " << e.what() << std::endl;
        }
    }

    // void rand_msg(const char *msg, int len)
    // {
    //     int i;
    //     pthread_mutex_lock(&mutx);
    //     for (i = 0; i < sizeof(clnt_socks) / sizeof(*clnt_socks); i++)
    //         write(clnt_socks[i], msg, len);
    //     pthread_mutex_unlock(&mutx);
    // }
    void group_chat(int clnt_sock)
    {
        vector<string> buddy(MAX_CLNT);
        vector<int> buddy_socks(MAX_CLNT);
        string user_no;
        char msg[BUF_SIZE];
        char group_msg[BUF_SIZE];
        int buddy_cnt = 0;
        int buddy_sock_cnt = 0;
        int buddy_sock;
        int buddy_choice = 0;
        string buddy_name;
        string buddy_nickname;
        string user_nickname;
        string left = " (";
        string right = ") ";
        string colon = " : ";
        try
        {
            std::unique_ptr<sql::Statement> stmnt(USERDB.createStatement());
            sql::ResultSet *res = stmnt->executeQuery("SELECT * FROM USER");

            string group_chat_msg = "그룹채팅입니다.\n";
            write(clnt_sock, group_chat_msg.c_str(), group_chat_msg.size());

            while (res->next())
            {
                if (res->getString(4) == user_id)
                {
                    user_no = res->getString(1);
                    user_nickname = res->getString(3);
                }
            }
            std::unique_ptr<sql::Statement> stmnt2(USERDB.createStatement());
            sql::ResultSet *res1 = stmnt2->executeQuery("SELECT * FROM BUDDY WHERE USER_NUM =" + user_no);

            group_chat_msg = "대화가능한 친구 목록 입니다\n";
            write(clnt_sock, group_chat_msg.c_str(), group_chat_msg.size());
            while (res1->next())
            {

                if (res1->getBoolean(7) == true && res1->getInt(8) == 1)
                {

                    buddy_name = res1->getString(5);
                    buddy_nickname = res1->getString(6);
                    buddy_cnt++;
                    // cout << "대화 가능한 친구 " << buddy_cnt << " : " << res1->getString(5) << " (" << res1->getString(6) << ")" << endl;
                    string ok_buddy = to_string(buddy_cnt) + colon + buddy_name + left + buddy_nickname + right + "\n";
                    write(clnt_sock, ok_buddy.c_str(), ok_buddy.size());

                    buddy.insert(buddy.begin() + buddy_cnt, buddy_nickname);
                }
            }
            while (1)
            {
                group_chat_msg = "초대할 친구를 골라주세요. 대화를 시작하려면 t\n";
                write(clnt_sock, group_chat_msg.c_str(), group_chat_msg.size());
                memset(group_msg, 0, BUF_SIZE);
                read(clnt_sock, group_msg, sizeof(group_msg));
                cout << "group_msg check : " << group_msg << endl;
                buddy_choice++;
                if(!strcmp(group_msg, "t\n"))
                    break;
            }
            std::unique_ptr<sql::Statement> aaaa(USERDB.createStatement());
            sql::ResultSet *res3 = aaaa->executeQuery("SELECT SOKET_NUM FROM USER WHERE  USER_NICKNAME = '" + buddy_nickname + "'");

            while (buddy_cnt != 0)
            {
                cout << "buddy_choice check : " << buddy_choice <<endl;
                // read(clnt_sock, msg, sizeof(msg));
                // cout << "msg check : " << msg << endl;
                if (buddy_choice != 0)
                {
                    cout << "이프문 진입" << endl;
                    buddy_nickname = buddy.at(buddy_choice); // 되는걸까 마는걸까 // 되네
                    cout << "buddy nickname check : " << buddy_nickname << endl;
                    if (res3->next())
                    {
                        buddy_sock_cnt++;
                        buddy_sock = res3->getInt(1);
                        cout << "buddy_sock check : " << buddy_sock << endl;
                        buddy_socks.push_back(buddy_sock);
                        cout << "buddy_socks check : " << buddy_socks.at(buddy_sock_cnt) << endl;
                    }
                    group_chat_msg = "그룹채팅을 시작합니다.";
                    write(clnt_sock, group_chat_msg.c_str(), group_chat_msg.size());
                    write(buddy_sock, group_chat_msg.c_str(), group_chat_msg.size());
                    cout << "그룹챗 들어간다" << endl;
                    // while (1)
                    // {
                    //     cout << "그룹챗 와일문 들어옴" << endl;
                    //     memset(msg, 0, BUF_SIZE);
                    //     int len = read(clnt_sock, msg, BUF_SIZE);
                    //     if (len <= 0)
                    //         break;
                    //     write(buddy_sock, msg, len);

                    //     memset(msg, 0, BUF_SIZE);
                    //     len = read(buddy_sock, msg, BUF_SIZE);
                    //     if (len <= 0)
                    //         break;
                    //     write(clnt_sock, msg, len);
                    // }
                    memset(group_msg, 0, BUF_SIZE);
                    while (1)
                    {
                        // cout << "그룹챗 와일문 들어옴" << endl;
                        for (int i = 0; i <= buddy_cnt; i++)
                        {
                            read(buddy_sock, group_msg, sizeof(group_msg));
                            read(clnt_sock, group_msg, sizeof(group_msg));
                            write(buddy_sock, group_msg, sizeof(group_msg));
                            write(clnt_sock, group_msg, sizeof(group_msg));
                        }
                        // write(clnt_sock, group_msg, sizeof(group_msg));
                    }
                }
                // while (res3->next())
                // {
                //     buddy_sock_cnt++;
                //     int buddy_sock = res3->getInt(1);
                //     cout << "buddy_sock check : " << buddy_sock << endl;
                //     // buddy_socks.push_back(buddy_sock);
                //     // cout << "buddy_socks check : " << buddy_socks.at(buddy_sock_cnt) << endl;
                // }
            }
        }
        catch (sql::SQLException &e)
        {
            std::cerr << "Error inserting new task: " << e.what() << std::endl;
        }
    }
};

int main(int argc, char *argv[])
{
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    socklen_t clnt_adr_sz;
    pthread_t t_id;
    if (argc != 2)
    {
        std::cout << "Usage : " << argv[0] << "<port>\n";
        exit(1);
    }

    pthread_mutex_init(&mutx, NULL);
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    if (bind(serv_sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("bind() error");
    if (listen(serv_sock, 5) == -1)
        error_handling("listen() error");

    while (1)
    {
        clnt_adr_sz = sizeof(clnt_adr);
        clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_adr, &clnt_adr_sz);
        std::cout << "clnt_sock : " << clnt_sock << std::endl;

        pthread_mutex_lock(&mutx);
        clnt_socks[clnt_cnt++] = clnt_sock;
        pthread_mutex_unlock(&mutx);

        pthread_create(&t_id, NULL, handle_clnt, (void *)&clnt_sock);
        pthread_detach(t_id);
        std::cout << ("Connected client IP: %s \n", inet_ntoa(clnt_adr.sin_addr));
    }
    close(serv_sock);
    return 0;
}

void *handle_clnt(void *arg)
{
    int clnt_sock = *((int *)arg);
    char msg[BUF_SIZE];
    DB db;
    USER user(db);
    db.connect();
    user.longin(clnt_sock);

    pthread_mutex_lock(&mutx);
    for (int i = 0; i < clnt_cnt; i++)
    {
        if (clnt_sock == clnt_socks[i])
        {
            while (i++ < clnt_cnt - 1)
                clnt_socks[i] = clnt_socks[i + 1];
            break;
        }
    }
    clnt_cnt--;
    pthread_mutex_unlock(&mutx);

    close(clnt_sock);
    return NULL;
}

void error_handling(const char *msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}

//////////////////////////////////////////
// void *handle_clnt(void *arg)
// {
//     int clnt_sock = *((int *)arg);
//     char msg[BUF_SIZE];
//     DB db;
//     USER user(db);
//     db.connect();
//     user.longin(clnt_sock);

//     pthread_mutex_lock(&mutx);
//     for (int i = 0; i < clnt_cnt; i++)
//     {
//         if (clnt_sock == clnt_socks[i])
//         {
//             while (i++ < clnt_cnt - 1)
//                 clnt_socks[i] = clnt_socks[i + 1];
//             break;
//         }
//     }
//     clnt_cnt--;
//     pthread_mutex_unlock(&mutx);

//     close(clnt_sock);
//     cout << "클라이언트 소켓 종료" << endl; // 클라이언트 소켓이 종료되었음을 표시하는 메시지 추가
//     return NULL;
// }