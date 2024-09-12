#include <iostream>
#include <mariadb/conncpp.hpp>
#include <string>
#include <string.h>
#include <vector>
#include <algorithm>
#include <fstream>
#include <stdlib.h>
#include <ctime>

const char *file = "history.txt";
const char *memofile = "memo.txt";
class Search
{
private:
  std::string housekind;
  std::string district;
  std::string dongname;
  std::string searchname;

public:
  void showPARK(std::unique_ptr<sql::Connection> &conn)
  {
    try
    {
      // createStaemet 객체 생성
      std::unique_ptr<sql::Statement> stmnt(conn->createStatement());
      int i = 1;
      sql::ResultSet *res = stmnt->executeQuery("select* from PARK");
      std::cout << "[ 지역구: " << district;
      std::cout << "/ 법정동: " << dongname << " ]" << std::endl;
      while (res->next())
      {
        if (district.compare(res->getString(2)) == 0 && dongname.compare(res->getString(3)) == 0)
        {
          std::cout << i++ << " : " << res->getString(4);
          std::cout << "\n";
        }
        else
          continue;
      }
      std::cout << "========================================================================================" << std::endl;
      std::cout << "총 공원의 수: " << i - 1 << "개" << std::endl;
      std::cout << std::endl;
    }
    catch (sql::SQLException &e)
    {
      std::cerr << "Error selecting tasks: " << e.what() << std::endl;
    }
  }

  void showBUDONGSAN(std::unique_ptr<sql::Connection> &conn)
  {
    try
    {
      int i = 1;
      std::cout << "[ 지역구: " << district;
      std::cout << "/ 법정동: " << dongname << " ]" << std::endl;
      std::unique_ptr<sql::Statement> stmnt(conn->createStatement());
      sql::ResultSet *res = stmnt->executeQuery("select* from BUDONGSAN");
      while (res->next())
      {
        if (district.compare(res->getString(2)) == 0 && dongname.compare(res->getString(3)) == 0)
        {
          std::cout << i++ << " : " << res->getString(7);
          std::cout << "\n";
        }
        else
          continue;
      }
      std::cout << "========================================================================================" << std::endl;
      std::cout << "총 부동산 수: " << i - 1 << "개" << std::endl;
      std::cout << std::endl;
    }
    catch (sql::SQLException &e)
    {
      std::cerr << "Error selecting tasks: " << e.what() << std::endl;
    }
  }

  void showMART(std::unique_ptr<sql::Connection> &conn)
  {
    try
    {
      int i = 1;

      std::cout << "[ 지역구: " << district;
      std::cout << "/ 법정동: " << dongname << " ]" << std::endl;
      std::unique_ptr<sql::Statement> stmnt(conn->createStatement());
      sql::ResultSet *res = stmnt->executeQuery("select* from MART");
      while (res->next())
      {

        if (district.compare(res->getString(2)) == 0 && dongname.compare(res->getString(3)) == 0)
        {
          std::cout << i++;
          std::cout << " : " << res->getString(4);
          std::cout << " ( " << res->getString(5) << " )";
          std::cout << "\n";
        }
        else
          continue;
      }
      std::cout << "========================================================================================" << std::endl;
      std::cout << "총 마트의 수: " << i - 1 << "개" << std::endl;
      std::cout << std::endl;
    }
    catch (sql::SQLException &e)
    {
      std::cerr << "Error selecting tasks: " << e.what() << std::endl;
    }
  }

  void ShowBUILDINGS(std::unique_ptr<sql::Connection> &conn)
  {
    int i = 1;
    int min = 1000000;
    int max = 0;
    int min_floor = 0;
    int max_floor = 0;
    char min_house[30];
    char max_house[30];
    try
    {
      std::vector<std::string> check;
      std::vector<std::string> check2;
      std::vector<std::string> check3;

      std::cout << "검색 가능한 주거 형태는 다음과 같습니다." << std::endl;
      std::unique_ptr<sql::Statement> stmnt(conn->createStatement());
      sql::ResultSet *res = stmnt->executeQuery("select * from BUILDINGS");
      while (res->next())
      {
        if (find(check.begin(), check.end(), res->getString(2)) == check.end())
          check.emplace_back(res->getString(2));
      }

      std::cout << "[ 조회 원하시는 것을 입력 부탁드립니다 ]" << std::endl;
      std::cout << "========================================================================================" << std::endl;
      for (int i = 0; i < check.size(); i++)
      {
        std::cout << check.at(i) << ' ';
        if (i % 10 == 0)
        {
          std::cout << std::endl;
        }
      }
      std::cout << std::endl
                // << std::endl
                << "========================================================================================" << std::endl;
      std::cout << " 입력 : ";
      std::cin >> housekind;
      res = stmnt->executeQuery("select * from BUILDINGS");
      while (res->next())
      {
        if (res->getString(2) == housekind)
        {
          if (find(check2.begin(), check2.end(), res->getString(5)) == check2.end())
            check2.emplace_back(res->getString(5));
        }
      }
      std::cout << std::endl;
      std::cout << "[ 조회 원하시는 것을 입력 부탁드립니다 ]" << std::endl;
      std::cout << "========================================================================================" << std::endl;
      for (int i = 0; i < check2.size(); i++)
      {
        std::cout << check2.at(i) << ' ';
        if ((i + 1) % 10 == 0)
        {
          std::cout << std::endl;
        }
      }
      std::cout << std::endl
                << "========================================================================================" << std::endl;
      std::cout << " 입력 : ";
      std::cin >> district;
      res = stmnt->executeQuery("select * from BUILDINGS");
      while (res->next())
      {
        if (res->getString(2) == housekind && res->getString(5) == district)
        {
          if (find(check3.begin(), check3.end(), res->getString(6)) == check3.end())
            check3.emplace_back(res->getString(6));
        }
      }
      std::cout << std::endl;
      std::cout << "[ 조회 원하시는 것을 입력 부탁드립니다 ]" << std::endl;
      std::cout << "========================================================================================" << std::endl;
      for (int i = 0; i < check3.size(); i++)
      {
        std::cout << check3.at(i) << ' ';
        if ((i + 1) % 10 == 0)
        {
          std::cout << std::endl;
        }
      }
      std::cout << std::endl
                << "========================================================================================" << std::endl;
      std::cout << " 입력 : ";
      std::cin >> dongname;
      res = stmnt->executeQuery("select * from BUILDINGS");
      std::cout << "입력하신 지역의 (" << housekind << ") 최근 거래 정보입니다." << std::endl;
      std::cout << "[ 지역구: " << district;
      std::cout << "/ 법정동: " << dongname << " ]" << std::endl;
      std::cout << "========================================================================================" << std::endl;
      while (res->next())
      {
        if (housekind.compare(res->getString(2)) == 0 && district.compare(res->getString(5)) == 0 && dongname.compare(res->getString(6)) == 0)
        {
          std::cout << i++;
          std::cout << " : " << res->getString(9) << " " << res->getInt(13) << " 층";
          std::cout << " [ " << res->getInt(11) << " 만원 / " << res->getInt(14) << "년 준공 ]";
          std::cout << "\n";
          if (min > res->getInt(11))
          {
            min = res->getInt(11);
            strcpy(min_house, res->getString(9));
            min_floor = res->getInt(13);
          }
          if (max < res->getInt(11))
          {
            max = res->getInt(11);
            strcpy(max_house, res->getString(9));
            max_floor = res->getInt(13);
          }
        }
      }
      std::cout << "========================================================================================" << std::endl;
      std::cout << "총 매물 :" << i - 1 << "개" << std::endl;
      std::cout << "최저가 : " << min_house << " " << min_floor << "층 " << min << "만원" << std::endl;
      std::cout << "최고가 : " << max_house << " " << max_floor << "층 " << max << "만원" << std::endl;
      std::cout << "========================================================================================" << std::endl;
      std::cout << std::endl;
    }
    catch (sql::SQLException &e)
    {
      std::cerr << "Error selecting tasks: " << e.what() << std::endl;
    }
  }
  void showSUB(std::unique_ptr<sql::Connection> &conn)
  {
    try
    {
      int i = 1;
      std::unique_ptr<sql::Statement> stmnt(conn->createStatement());
      sql::ResultSet *res = stmnt->executeQuery("select * from SUB");
      while (res->next())
      {
        if (district.compare(res->getString(6)) == 0 && dongname.compare(res->getString(8)) == 0)
        {
          std::cout << i++;
          std::cout << " : " << res->getInt(3) << "호선 [" << res->getString(4) << "역]";
          std::cout << "\n";
        }
        else
          continue;
      }
      std::cout << "총 역의 수: " << i - 1 << "개" << std::endl;
    }

    catch (sql::SQLException &e)
    {
      std::cerr << "Error selecting tasks: " << e.what() << std::endl;
    }
  }
  void memo_r()
  {
    char ch;
    std::ifstream fin;
    fin.open(memofile);

    if (fin.is_open())
    {
      std::cout << "현재 저장된 메모 입니다: " << std::endl;
      while (fin.get(ch))
      {
        std::cout << ch;
      }
      std::cout << std::endl;
    }
    fin.close();
  }
  void memo_w()
  {
    int cnt = 1;
    std::string memo;
    std::ofstream fout(memofile, std::ios_base::app);
    time_t timer;
    struct tm *t;
    timer = time(NULL);
    t = localtime(&timer);
    std::cout << "메모를 입력해 주세요 : ";
    std::cin >> memo;
    while (cnt != 0 && memo.size() > 0)
    {
      cnt = 0;
      fout << t->tm_year + 1900 << "년 ";
      fout << t->tm_mon + 1 << "월 ";
      fout << t->tm_mday << "일 ";
      fout << t->tm_hour << "시 ";
      fout << t->tm_min << "분 ";
      fout << memo << std::endl;
    }
    fout.close();
  }
  void history_w()
  {
    int cnt = 1;
    std::ofstream fout(file, std::ios_base::app);
    time_t timer;
    struct tm *t;
    timer = time(NULL);
    t = localtime(&timer);
    while (cnt != 0 && housekind.size() > 0)
    {
      cnt = 0;
      fout << t->tm_year + 1900 << "년 ";
      fout << t->tm_mon + 1 << "월 ";
      fout << t->tm_mday << "일 ";
      fout << t->tm_hour << "시 ";
      fout << t->tm_min << "분 ";
      fout << "," << housekind << "," << district << "," << dongname << "," << searchname << "," << std::endl;
    }
    fout.close();
  }
  void history_search(std::unique_ptr<sql::Connection> &conn)
  {
    char ch;
    std::string str;
    int search_num = 0;
    std::vector<std::vector<std::string>> v1(15);
    std::vector<std::string> v2;
    std::ifstream fin;
    fin.open(file);

    if (fin.is_open())
    {
      std::cout << "현재 저장된 검색 히스토리 내역 입니다: " << std::endl;
      std::cout << std::endl;
      while (fin.get(ch))
      {
        std::cout << ch;
      }
    }
    fin.close();
    std::cout << std::endl;
    while (getline(fin, str, ','))
    {
      v2.push_back(str);
    }
    fin.close();

    for (int i = 0; i < 6; i++)
    {
      for (int j = 0; j < v2.size(); j++)
      {
        v1[i].push_back(v2[i]);
      }
    }

    
    std::cout << "검색할 히스토리를 골라주세요 : ";
    std::cin >> search_num;
    std::cout << std::endl;
    for (int i = 0; i < v2.size(); i++)
    {
      if (search_num == i+1)
      {

        int i = 1;
        int min = 1000000;
        int max = 0;
        int min_floor = 0;
        int max_floor = 0;
        char min_house[30];
        char max_house[30];
        try
        {
          std::vector<std::string> check;
          std::vector<std::string> check2;
          std::vector<std::string> check3;

          // std::cout << "검색 가능한 주거 형태는 다음과 같습니다." << std::endl;
          std::unique_ptr<sql::Statement> stmnt(conn->createStatement());
          sql::ResultSet *res = stmnt->executeQuery("select * from BUILDINGS");
          // while (res->next())
          // {
          //   if (find(check.begin(), check.end(), res->getString(2)) == check.end())
          //     check.emplace_back(res->getString(2));
          // }

          // std::cout << "[ 조회 원하시는 것을 입력 부탁드립니다 ]" << std::endl;
          std::cout << "========================================================================================" << std::endl;
          for (int i = 0; i < check.size(); i++)
          {
            std::cout << check.at(i) << ' ';
            if (i % 10 == 0)
            {
              std::cout << std::endl;
            }
          }
          std::cout << std::endl
                    // << std::endl
                    << "========================================================================================" << std::endl;
          // std::cout << " 입력 : ";
          housekind = v1[i][1];
          res = stmnt->executeQuery("select * from BUILDINGS");
          while (res->next())
          {
            if (res->getString(2) == housekind)
            {
              if (find(check2.begin(), check2.end(), res->getString(5)) == check2.end())
                check2.emplace_back(res->getString(5));
            }
          }
          std::cout << std::endl;
          // std::cout << "[ 조회 원하시는 것을 입력 부탁드립니다 ]" << std::endl;
          std::cout << "========================================================================================" << std::endl;
          for (int i = 0; i < check2.size(); i++)
          {
            std::cout << check2.at(i) << ' ';
            if ((i + 1) % 10 == 0)
            {
              std::cout << std::endl;
            }
          }
          std::cout << std::endl
                    << "========================================================================================" << std::endl;
          // std::cout << " 입력 : ";
          district = v1[i][2];
          res = stmnt->executeQuery("select * from BUILDINGS");
          while (res->next())
          {
            if (res->getString(2) == housekind && res->getString(5) == district)
            {
              if (find(check3.begin(), check3.end(), res->getString(6)) == check3.end())
                check3.emplace_back(res->getString(6));
            }
          }
          std::cout << std::endl;
          // std::cout << "[ 조회 원하시는 것을 입력 부탁드립니다 ]" << std::endl;
          std::cout << "========================================================================================" << std::endl;
          for (int i = 0; i < check3.size(); i++)
          {
            std::cout << check3.at(i) << ' ';
            if ((i + 1) % 10 == 0)
            {
              std::cout << std::endl;
            }
          }
          std::cout << std::endl
                    << "========================================================================================" << std::endl;
          // std::cout << " 입력 : ";
          dongname = v1[i][3];
          res = stmnt->executeQuery("select * from BUILDINGS");
          std::cout << "입력하신 지역의 (" << housekind << ") 최근 거래 정보입니다." << std::endl;
          std::cout << "[ 지역구: " << district;
          std::cout << "/ 법정동: " << dongname << " ]" << std::endl;
          std::cout << "========================================================================================" << std::endl;
          while (res->next())
          {
            if (housekind.compare(res->getString(2)) == 0 && district.compare(res->getString(5)) == 0 && dongname.compare(res->getString(6)) == 0)
            {
              std::cout << i++;
              std::cout << " : " << res->getString(9) << " " << res->getInt(13) << " 층";
              std::cout << " [ " << res->getInt(11) << " 만원 / " << res->getInt(14) << "년 준공 ]";
              std::cout << "\n";
              if (min > res->getInt(11))
              {
                min = res->getInt(11);
                strcpy(min_house, res->getString(9));
                min_floor = res->getInt(13);
              }
              if (max < res->getInt(11))
              {
                max = res->getInt(11);
                strcpy(max_house, res->getString(9));
                max_floor = res->getInt(13);
              }
            }
          }
          std::cout << "========================================================================================" << std::endl;
          std::cout << "총 매물 :" << i - 1 << "개" << std::endl;
          std::cout << "최저가 : " << min_house << " " << min_floor << "층 " << min << "만원" << std::endl;
          std::cout << "최고가 : " << max_house << " " << max_floor << "층 " << max << "만원" << std::endl;
          std::cout << "========================================================================================" << std::endl;
          std::cout << std::endl;
        }
        catch (sql::SQLException &e)
        {
          std::cerr << "Error selecting tasks: " << e.what() << std::endl;
        }
      }
    }
  }
  void Search_start(std::unique_ptr<sql::Connection> &conn)
  {
    int count = 0;
    char sel;
    std::cout << "검색을 시작 하시겠습니까?" << std::endl
              << "원하는 검색을 선택해 주세요." << std::endl;
    std::cout << "0 : 처음으로\n1 : 매물 검색\n2 : 히스토리 검색\n3 : 메모장\n4 : 메모장 보기\nQ : 종료" << std::endl;
    std::cin >> sel;
    while (count < 1)
    {
      switch (sel)
      {
      case '0':
        count++;
        break;

      case '1':
        system("clear");
        Start(conn);
        count++;
        break;

      case '2':
        system("clear");
        history_search(conn);
        count++;
        break;

      case '3':
        system("clear");
        memo_w();
        count++;
        break;

      case '4':
        system("clear");
        memo_r();
        count++;
        break;

      case 'q':
      case 'Q':
        std::cout << "<system : 프로그램을 종료하겠습니다>";
        exit(1);
        break;

        // default:
        //   std::cout << "올바른 번호를 선택해 주세요.";
        //   continue;
      }
    }
  }

  void Start(std::unique_ptr<sql::Connection> &conn)
  {
    ShowBUILDINGS(conn);
    int count = 0;
    char sel;
    std::cout << "매물 인근의 편의 시설을 검색할 수 있습니다." << std::endl
              << "원하는 편의 시설을 선택해 주세요." << std::endl;
    std::cout << "0 : 처음으로\n1 : 부동산\n2 : 지하철\n3 : 마트\n4 : 공원\nQ : 종료" << std::endl;
    std::cin >> sel;
    while (count < 1)
    {
      switch (sel)
      {
      case '0':
        count++;
        break;

      case '1':
        system("clear");
        showBUDONGSAN(conn);
        searchname = "BUDONGSAN";
        count++;
        break;

      case '2':
        system("clear");
        showSUB(conn);
        searchname = "SUB";
        count++;
        break;

      case '3':
        system("clear");
        showMART(conn);
        searchname = "MART";
        count++;
        break;

      case '4':
        system("clear");
        showPARK(conn);
        searchname = "PARK";
        count++;
        break;

      case 'q':
      case 'Q':
        std::cout << "<system : 프로그램을 종료하겠습니다>";
        exit(1);
        break;

      default:
        std::cout << "올바른 번호를 선택해 주세요.";
        continue;
      }
    }
    history_w();
  }
};

int main()
{
  try
  {
    Search Searching;
    sql::Driver *driver = sql::mariadb::get_driver_instance();
    sql::SQLString url("jdbc:mariadb://10.10.21.111/HOME");
    sql::Properties properties({{"user", "OPERATOR"}, {"password", "1234"}});
    std::unique_ptr<sql::Connection> conn(driver->connect(url, properties));

    while (1)
    {
      Searching.Search_start(conn);
    }
  }
  catch (sql::SQLException &e)
  {
    std::cerr << "Error Connecting to MariaDB Platform: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
