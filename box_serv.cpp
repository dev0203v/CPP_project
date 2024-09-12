#include <mariadb/conncpp.hpp>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <string>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sstream>
#include <iomanip>
#define IMAGE_BUFFER_SIZE 1024
#define BUF_SIZE 100
#define MAX_CLNT 256
#define NAME_SIZE 20
using namespace std;
void *handle_clnt(void *arg);
void error_handling(const char *msg);

int clnt_cnt = 0;		  // 소켓 디스크립터의 인덱스 번호 역할
int clnt_socks[MAX_CLNT]; // 소켓 디스크립터 담을 256개
pthread_mutex_t mutx;	  // 뮤텍스 변수

class DB
{
public:
	sql::Connection *conn;
	DB() {}
	void connect()
	{
		try
		{
			sql::Driver *driver = sql::mariadb::get_driver_instance();
			sql::SQLString url = ("jdbc:mariadb://10.10.21.111:3306/Package");
			sql::Properties properties({{"user", "6team"}, {"password", "1234"}});
			conn = driver->connect(url, properties);
		}
		catch (sql::SQLException &e)
		{
			std::cerr << "Error Connecting to MariaDB Platform: " << e.what() << std::endl;
		}
	}

	~DB() { delete conn; }
};
//////////////////
std::string read(int clnt_sock, DB &data);

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
		cout << "clnt_sock : " << clnt_sock << endl;

		pthread_mutex_lock(&mutx);
		clnt_socks[clnt_cnt++] = clnt_sock;
		pthread_mutex_unlock(&mutx);

		pthread_create(&t_id, NULL, handle_clnt, (void *)&clnt_sock);
		pthread_detach(t_id);
		cout << ("Connected client IP: %s \n", inet_ntoa(clnt_adr.sin_addr));
	}
	close(serv_sock);
	return 0;
}

void *handle_clnt(void *arg)
{
	int clnt_sock = *((int *)arg);
	char msg[BUF_SIZE];
	string recv_msg;
	string box_type;
	int box_size;
	DB db;
	db.connect();
	int flag = 0;

	recv_msg = read(clnt_sock, db);

	if (recv_msg == "클라이언트 접속")
	{
		flag = 1;
	}
	while (flag == 1)
	{
		recv_msg = read(clnt_sock, db);
		box_size = stoi(recv_msg);

		cout << "box_size : " << box_size << endl;
		if (box_size > 0)
		{
			try
			{
				if (box_size >= 160)
					box_type = "특대형";
				else if (box_size >= 140)
					box_type = "대형";
				else if (box_size >= 120)
					box_type = "중형";
				else if (box_size >= 100)
					box_type = "소형";
				else if (box_size <= 80)
					box_type = "극소형";

				std::unique_ptr<sql::PreparedStatement> stmnt(db.conn->prepareStatement("INSERT INTO Package_size (box_num,size,type,date)VALUES(default,?,?,default)"));
				stmnt->setInt(1, box_size);
				stmnt->setString(2, box_type);
				stmnt->executeQuery();
			}
			catch (sql::SQLException &e)
			{
				std::cerr << "Error Connecting to MariaDB Platform: " << e.what() << std::endl;
			}
		}
	}

	return NULL;
}

void error_handling(const char *msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}

string read(int clnt_sock, DB &data)
{
	string str_msg;
	char msg[BUF_SIZE];
	memset(msg, 0, sizeof(msg));
	if (read(clnt_sock, msg, sizeof(msg)) > 0)
	{
		str_msg = msg;
		return str_msg;
	}
	else
	{
		return "연결종료";
	}
}