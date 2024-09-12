#include <iostream>
#include <cstring>
#include <time.h>
#include <unistd.h>
using namespace std;
using std::ios_base;
struct user
{
    char name[30];
    int level;
    double exp;
}user;
void victory(int* turn);
void badending(int* turn);
void knockdown(int* turn);

int main(){
    char name;
    char gamechoice;
    int j=0;
    int count = 0;
    int turn = 1;
    int resureection = 0;
    int res_num = 0;
    double up_exp = 0;
    double random_exp = 0;
    double event_rand = 0;
    double turn_exp = 0;
    double LV_exp_count[100]={0,30000};
    float total_exp = 0;
    float levelup_rand = 0;
    
    user.level = 1;
    
    ios_base::fmtflags orig =
        cout.setf(ios_base::fixed, ios_base::floatfield);
    std::streamsize prec = cout.precision(3);

    srand(time(NULL));

    cout << "닉네임을 지정해주세요: \n";
    cin >> user.name;
    cout << "\n========== 게임을 시작 합니다 ==========\n";

    while(1){
        system("clear");
        if (gamechoice == 'n'){
            cout << "게임을 종료합니다.\n";
            break;
        }
        else if( turn == 1 || (gamechoice == 'y' && user.level < 100) && user.level >= 1){
            cout << "           나 혼자만 레벨 업              \n";
            cout << "========================================\n";
            cout << turn << "턴 째 진행중" << endl;

            for(int i = 0; i < 5; i++){
                random_exp = rand()%7000-3500;
                turn_exp += random_exp;
                cout << random_exp << "경험치 획득!"<< endl;
            }
            event_rand = 1.0+(double)rand()/RAND_MAX*(5.0-1.0);
            cout.precision(2);
            cout << "*****이벤트 발생 경험치 " << event_rand << "배 획득*****" << endl;
            total_exp = turn_exp*event_rand;
            cout << "이번 턴 경험치: " << total_exp  << "\n";
            turn++;

            user.exp += total_exp;
            up_exp=1.8+(double)rand()/RAND_MAX*(2.5-1.8);

            if (user.level > 1){
                LV_exp_count[user.level] = LV_exp_count[user.level-1]*up_exp; // 300*2.5
                if (res_num == 1)
                    LV_exp_count[user.level] *= 0.5;
            }
            cout << user.name << "용사님 LV: " << user.level << " exp: " << user.exp  << '\n';
            cout << "다음 레벨까지 필요 경험치: " << LV_exp_count[user.level] << endl;

            if (LV_exp_count[user.level] < user.exp && user.level >= 1){
                user.level += 1;
                cout << "축하합니다! LEVEL UP!!\n";
                count = 1;
                user.exp -= LV_exp_count[user.level];
            }
            else if (user.exp <= 0 && user.level >= 1){
                user.level -= 1;
                user.exp = LV_exp_count[user.level]+user.exp;
                cout << "LEVEL DOWN....\n";
            }
            cout << "========================================\n";
            cout << "      게임을 더 진행하시겠습니까? \n       yes : y / no : n\n";
            cout << "========================================\n";
            
            cin >> gamechoice;

            

        }
        if(user.level < 0)
            user.level = 0;

        if (user.level == 0 && count == 1 ){
            system("clear");
            knockdown(&turn);
            break;
        }
        else if (user.level == 0){
            resureection = rand()%10+1;
            system("clear");
            badending(&turn);
            if(resureection < 4){
                system("clear");
                cout << "                  !!!!!\n";
                cout << "========================================\n";
                sleep(1);
                cout << " 신의 축복으로 부활합니다!!\n"; 
                sleep(1);
                cout << " 부활 후엔 필요경험치가 절반으로 감소합니다.\n"; 
                res_num = 1;
                user.level = 1;
                LV_exp_count[1] = {15000};
                sleep(2);
                continue;
                
            }
            else{
                break;
            }
        }
        else if (user.level == 100){
            system("clear");
            victory(&turn);
            break;
        }

    }
    
    
    return 0;
}

void victory(int* turn){
    cout << "========================================\n";
    cout << "         LV." << user.level << "  Exp" << user.exp << "\n";
    cout << *turn << "턴째 종료.\n";
    cout << "\n\n============== Victory ===============\n";
    cout << "============== happy ending ==============\n\n";
    cout << "\n\n\n";
}
void badending(int* turn){
    user.exp = 0;
    cout << "========================================\n";
    cout << "         LV." << user.level << "  Exp" << user.exp << "\n";
    cout << *turn << "턴째 종료.\n";
    cout << "========================================\n";
    cout << "\n\n============== Game Over ===============\n";
    cout << "============== Bad Ending ==============\n\n";
    cout << "\n\n\n";
}
void knockdown(int* turn){
    user.exp = 0;
    cout << "========================================\n";
    cout << "         LV." << user.level << "  Exp" << user.exp << "\n";
    cout << *turn << "턴째 종료.\n";
    cout << "========================================\n";
    cout << "\n\n============== Game Over ===============\n";
    cout << "============== Knock Down ==============\n\n";
    cout << "\n\n\n";
}