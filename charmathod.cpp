void group_chat(int clnt_sock)
{
    vector<string> buddy(MAX_CLNT);
    vector<int> buddy_socks(MAX_CLNT);
    string user_no;
    char msg[BUF_SIZE];
    int buddy_cnt = 0;
    int buddy_sock_cnt = 0;
    int buddy_sock;
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
                cout << "대화 가능한 친구 " << buddy_cnt << " : " << res1->getString(5) << " (" << res1->getString(6) << ")" << endl;
                string ok_buddy = to_string(buddy_cnt) + colon + buddy_name + left + buddy_nickname + right + "\n";
                write(clnt_sock, ok_buddy.c_str(), ok_buddy.size());

                buddy.insert(buddy.begin() + buddy_cnt, buddy_nickname);
            }
        }
        group_chat_msg = "초대할 친구를 골라주세요\n";
        write(clnt_sock, group_chat_msg.c_str(), group_chat_msg.size());

        std::unique_ptr<sql::Statement> aaaa(USERDB.createStatement());
        sql::ResultSet *res3 = aaaa->executeQuery("SELECT SOKET_NUM FROM USER WHERE  USER_NICKNAME = '" + buddy_nickname + "'");

        while (buddy_cnt != 0)
        {
            read(clnt_sock, msg, sizeof(msg));
            cout << "msg check : " << msg << endl;
            if (msg != 0)
            {
                cout << "이프문 진입" << endl;
                buddy_nickname = buddy.at(stoi(msg)); // 되는걸까 마는걸까 // 되네
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
                while (1)
                {
                    cout << "그룹챗 와일문 들어옴" << endl;
                    memset(msg, 0, BUF_SIZE);
                    int len = read(clnt_sock, msg, BUF_SIZE);
                    if (len <= 0)
                        break;
                    write(buddy_sock, msg, len);

                    memset(msg, 0, BUF_SIZE);
                    len = read(buddy_sock, msg, BUF_SIZE);
                    if (len <= 0)
                        break;
                    write(clnt_sock, msg, len);
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
