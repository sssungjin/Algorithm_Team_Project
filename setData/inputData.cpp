#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;


enum Day {mon, tue, wed, thu, fri}; // 요일에 따른 수를 열거한 열거형
typedef struct schedule{
	enum Day day; // 요일
	int start_time; // 해당 요일에서 강의 시작시간
	int end_time; // 해당 요일에서 강의 종료시간
} Schedule; // 요일에 따른 강의 시간 정보를 담고 있는 구조체

typedef struct subject {
	string subject_name; // 강의 학수번호
	Schedule *time_table; // 요일별 강의시간
	int student_num; // 해당 강의를 듣고 있는 학생의 수
	struct subject* next;
} Subject; // 강의의 정보를 담는 구조체

string **student; // 학생의 수강강의 학수번호를 담는 이중포인터(ex : 1번 학생은 CSE2022-01, EGC2022-02... 을 듣고 있다면 student[1][0] = "CSE2022-01" student[1][1] = "EGC2022-02")



// --- 재형 코드 ----


int student_cnt; // 데이터로 받아온 학생 수
Subject **student_sub;

void setData(){
    ifstream fp("C:\\Users\\nicol\\Desktop\\Algorithm_Project\\Algorithm_Team_Project\\dataSet\\totalData.txt");
    if(!fp.is_open()){cout<<"not open"<<endl; return;}
    string fpstr; 
    string subject_code, time[3]; // 학수번호, 부분별로 나눠진 시간표(한 과목당 최대 3번으로 나눠져 수업 진행됨)
    int cnt_idx, cnt_len;           // student_cnt를 가져오기 위해 필요한 변수

    // 학생수만큼 동적할당하는 과정
    getline(fp,fpstr);
    cnt_idx=fpstr.find("student_cnt:")+12; // student_cnt:(숫자)의 숫자값만 가져오기 위함
    cnt_len=fpstr.length();
    fpstr=fpstr.substr(cnt_idx, cnt_len-cnt_idx);
    student_cnt=stoi(fpstr);
    student=new string*[student_cnt]; student_sub=new Subject*[student_cnt];

    // student 이차원 배열 데이터 설정 과정
    for(int pCnt=0; pCnt<student_cnt; pCnt++){
        getline(fp,fpstr); // 과목 수가 적혀있는 라인        
        int subject_cnt=stoi(fpstr);
        
        student[pCnt]=new string[subject_cnt]; student_sub[pCnt]=new Subject[subject_cnt];

        for(int sCnt=0;sCnt<subject_cnt;sCnt++){
            getline(fp,fpstr);
            stringstream split(fpstr);
            split >> subject_code >> time[0] >> time[1] >> time[2];
            student[pCnt][sCnt]=subject_code;

            // 학생별 Subject 구조체 멤버 초기화 진행
            student_sub[pCnt][sCnt].subject_name = subject_code;
            student_sub[pCnt][sCnt].student_num=0;
            int partCnt=0;          // 한과목이 분할된 갯수
            for(int i=0;i<3;i++)
                if(!time[i].empty()) partCnt++;
            student_sub[pCnt][sCnt].time_table=new schedule[partCnt];

            // 학생별 schedule 초기화 진행
            for(int tidx=0;tidx<partCnt;tidx++){
cout<<"time: "<<time[tidx];     // 디버깅 라인 (지워도됨)
                int split=time[tidx].find('-');
                string date=time[tidx].substr(0,3);
                string startT_str=time[tidx].substr(3,3);
                float startT=stof(startT_str); startT=(startT-1)*2;
cout<<" startT: "<<startT;      // 디버깅 라인 (지워도됨)
                string endT_str=time[tidx].substr(split+1, 3);
                float endT=stof(endT_str); endT=(endT-1)*2;
cout<<" endT: "<<endT<<endl;    // 디버깅 라인 (지워도됨)
                
                if(date=="mon") student_sub[pCnt][sCnt].time_table[tidx].day=mon;
                else if(date=="tue") student_sub[pCnt][sCnt].time_table[tidx].day=tue;
                else if(date=="wed") student_sub[pCnt][sCnt].time_table[tidx].day=wed;
                else if(date=="thu") student_sub[pCnt][sCnt].time_table[tidx].day=thu;
                else if(date=="fri") student_sub[pCnt][sCnt].time_table[tidx].day=fri;
                student_sub[pCnt][sCnt].time_table[tidx].start_time=(int)startT;
                student_sub[pCnt][sCnt].time_table[tidx].end_time=(int)endT;
            }
        }
    }
}

int main(){
    setData();
    for(int i=0;i<10;i++){
            cout<<"student : "<<student[i][0]<<endl;
            cout<<student_sub[i]->subject_name<<' ';
            cout<<student_sub[i]->time_table[0].day<<" ";
            cout<<student_sub[i]->time_table[0].start_time<<" ";
            cout<<student_sub[i]->time_table[0].end_time<<endl<<endl;
    }
}