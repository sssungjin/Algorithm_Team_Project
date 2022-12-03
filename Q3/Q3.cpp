#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

using namespace std;

#define max_snum 3 // 입력으로 쓸 학생 수
#define max_subject 24 // 수강 가능한 강의의 수
#define max_time 30 // 강의 교시 수(n = (1 + 0.5*n)교시) (n >= 0인 30보다 작은 정수)
#define max_day 5
#define max_scheduleNum 3 // 가능한 강의 시간 분할 수(일반물리학및실험1의 경우 강의가 3개의 시간으로 분할됨)

enum Day { mon, tue, wed, thu, fri }; // 요일에 따른 수를 열거한 열거형
typedef struct schedule {
    enum Day day; // 요일
    int start_time; // 해당 요일에서 강의 시작시간
    int end_time; // 해당 요일에서 강의 종료시간
} Schedule; // 요일에 따른 강의 시간 정보를 담고 있는 구조체

typedef struct subject {
    string subject_name; // 강의 학수번호
    Schedule* time_table; // 요일별 강의시간
    int student_num; // 해당 강의를 듣고 있는 학생의 수
    int schedule_num; // 강의시간 분할 수
    struct subject* next;
} Subject; // 강의의 정보를 담는 구조체


class subject_table // 강의의 학수번호를 key, Subject를 value로 가지는 해시 테이블
{
    Subject** table;
    int size;

public:
    subject_table(int n)
    {
        size = n;
        table = new Subject * [size]
        { NULL };
    }

    //해시값을 반환할 함수
    int hash_value(string subject_name)
    {
       int hash_value = 0, a = 3;

        for (unsigned int i = 0; i < subject_name.length(); i++)
        {
            hash_value += hash_value * a + subject_name[i];
        }

        return hash_value % size;
    }

    // table에 과목정보 배열 삽입할 함수
    void insert(Subject* subject)
    {
        int hv = hash_value(subject->subject_name);

        if (table[hv] == NULL)
        {
            subject->next=NULL;
            table[hv] = subject;
        }
        else
        {
            subject->next = table[hv];
            table[hv] = subject;
        }
    }

    // table에서 operation에 해당하는 code를 가져오는 함수
    Subject* get_value(string subject_name)
    {
        Subject* ptr;
        int hv = hash_value(subject_name);

        ptr = table[hv];

        if (ptr != NULL)
        {
            if (ptr->subject_name == subject_name)
            {
                return ptr;
            }
            else
            {
                ptr = ptr->next;
                while (ptr != NULL)
                {
                    if (ptr->subject_name == subject_name)
                        return ptr;
                    ptr = ptr->next;
                }
            }
        }
        return NULL;
    }
};

// --- 재형 코드 ----


int student_cnt; // 데이터로 받아온 학생 수
int* student_subjectNum;    //학생당 과목 수 배열
Subject** student_sub;
string** student;
string sb_union;   //중복 없는 학수번호 하나의 문자열로 저장
int total_sub = 0;  //전체 과목 수
subject_table st(10009);
Subject* sorted;
void setData() {
    ifstream fp("C:\\Users\\nicol\\Desktop\\알골팀플\\Algorithm_Team_Project\\Q3\\totalData.txt");
    if (!fp.is_open()) { cout << "not open" << endl; return; }
    string fpstr; // 파일로부터 읽어올 한 문장을 저장할 변수
    string subject_code, time[3]; // 학수번호, 부분별로 나눠진 시간표(한 과목당 최대 3번으로 나눠져 수업 진행됨)
    int cnt_idx, cnt_len;           // student_cnt를 가져오기 위해 필요한 변수

    // 학생수만큼 동적할당하는 과정
    getline(fp, fpstr);
    cnt_idx = 12; // student_cnt:(숫자)의 숫자값만 가져오기 위함
    cnt_len = fpstr.length();
    fpstr = fpstr.substr(cnt_idx, cnt_len - cnt_idx);
    student_cnt = stoi(fpstr);
    student = new string * [student_cnt]; // 학생수만큼의 포인터 배열 할당
    student_sub = new Subject * [student_cnt]; // 학생수만큼의 포인터 배열 크기 할당
    student_subjectNum = new int[student_cnt]; // 학생수만큼 포인터 배열 크기 할당

    // student 이차원 배열 데이터 설정 과정
    for (int pCnt = 0; pCnt < student_cnt; pCnt++) {
        getline(fp, fpstr); // 과목 수가 적혀있는 라인        
        int subject_cnt = stoi(fpstr);

        student[pCnt] = new string[subject_cnt];
        student_sub[pCnt] = new Subject[subject_cnt];
        student_subjectNum[pCnt] = subject_cnt;

        for (int sCnt = 0; sCnt < subject_cnt; sCnt++) {
            int scheduleNum = 0;
            getline(fp, fpstr);
            stringstream split(fpstr);
            for (int i = 0; i < 3; i++) time[i].clear();
            split >> subject_code >> time[0] >> time[1] >> time[2];
            student[pCnt][sCnt] = subject_code;

            // 학생별 Subject 구조체 멤버 초기화 진행
            student_sub[pCnt][sCnt].subject_name = subject_code;
            student_sub[pCnt][sCnt].student_num = 1;
            int partCnt = 0;          // 한과목이 분할된 갯수
            for (int i = 0; i < 3; i++)
                if (!time[i].empty()) partCnt++;
            student_sub[pCnt][sCnt].schedule_num = partCnt;
            student_sub[pCnt][sCnt].time_table = new schedule[partCnt];

            // 학생별 schedule 초기화 진행
            for (int tidx = 0; tidx < partCnt; tidx++) {
                int split = time[tidx].find('-');
                string date = time[tidx].substr(0, 3);
                string startT_str = time[tidx].substr(3, 3);
                float startT = stof(startT_str); startT = (startT - 1) * 2;
                string endT_str = time[tidx].substr(split + 1, 3);
                float endT = stof(endT_str); endT = (endT - 1) * 2;

                if (date == "mon") student_sub[pCnt][sCnt].time_table[tidx].day = mon;
                else if (date == "tue") student_sub[pCnt][sCnt].time_table[tidx].day = tue;
                else if (date == "wed") student_sub[pCnt][sCnt].time_table[tidx].day = wed;
                else if (date == "thu") student_sub[pCnt][sCnt].time_table[tidx].day = thu;
                else if (date == "fri") student_sub[pCnt][sCnt].time_table[tidx].day = fri;
                student_sub[pCnt][sCnt].time_table[tidx].start_time = (int)startT;
                student_sub[pCnt][sCnt].time_table[tidx].end_time = (int)endT;
            }
        }
    }
}

void swap(Subject* s1, Subject* s2) {
    Subject tmp;
    tmp = *s1;
    *s1 = *s2;
    *s2 = tmp;
}


class Q3
{
private:
    int size;
    Subject** resultSubject;
    Subject** subject; //[0][0]이면 월요일 0(1교시) [3][5]면 목요일 5(3.5교시)
public:
    Q3(){  //size는 total_sub
        resultSubject = new Subject *[5];
        subject = new Subject*[5];
        for (int i = 0; i < 5; i++) {
            subject[i] = new Subject[max_time];
            resultSubject[i] = new Subject[max_time];
        }
    }
    void setSize(int size) {
        this->size = size;
    }
    int getSize() {
        return this->size;
    }
    void initData() {   //해시테이블에 과목 구조체 삽입
        sb_union = "";
        for (int i = 0; i < student_cnt; i++) {
            for(int j = 0; j < student_subjectNum[i]; j++)
            if (st.get_value(student_sub[i][j].subject_name)) {  //해시테이블에 존재하면 가중치만 ++
                st.get_value(student_sub[i][j].subject_name)->student_num++;
            }
            else {  //해시테이블에 없으면 가중치 1로 설정, 해시테이블에 insert
                sb_union += student_sub[i][j].subject_name + " ";   //합연산으로 학수번호 중복 없이 저장
                total_sub++;
                st.insert(&student_sub[i][j]);
            }
        }
        
    }
    //문자열에 공백으로 구분된 학수번호 합집합을 문자열 배열에 저장
    string* splitUnion() {
        string* sub_unionArr = new string[total_sub];
        stringstream ss(sb_union);
        ss.str();
        string id;
        for (int i = 0; i < total_sub; i++) {
            ss >> id;
            sub_unionArr[i] = id;
        }
        return sub_unionArr;
    }

    void printTable() {     //해시테이블에 저장된 정보 출력 student_cnt 는 학생 수
        for (int i = 0; i < student_cnt; i++) {
            for (int j = 0; j < student_subjectNum[i]; j++) {   //student_subjectNum은 학생이 듣는 강의 수
                Subject* ptr = st.get_value(student[i][j]);
                cout << "학수번호 : " << ptr->subject_name;
                for (int k = 0; k < ptr->schedule_num; k++) {
                    cout << " 날짜 : " << ptr->time_table[k].day;
                    cout << " 시작시간 : " << ptr->time_table[k].start_time;
                    cout << " 종료시간 : " << ptr->time_table[k].end_time;
                }
                cout << " 가중치 : " << ptr->student_num << endl;
            }
        }
    }
    //해시테이블에 있는 정보를 복사한 배열 반환
    Subject* copyTable() {
        string* subArr = splitUnion();
        Subject* result = new Subject[total_sub];
        for (int i = 0; i < total_sub; i++) {
            Subject* ptr = st.get_value(subArr[i]);
            result[i].subject_name = ptr->subject_name;
            result[i].student_num = ptr->student_num;
            result[i].time_table = ptr->time_table;
            result[i].schedule_num = ptr->schedule_num;
            result[i].next = ptr->next;
        }
        return result;
    }


    void weightSort(Subject* a, int size){ // 내림차순 정렬
        Subject temp;
        for(int i=1;i<size;i++){
            temp=a[i]; 
            int j=i-1;
            while (j >= 0 && temp.student_num > a[j].student_num){
                a[j + 1] = a[j];
                j--;
            }
            a[j+1]=temp;
        }
    }

    void timeSort(Subject* a, int size){ // 오름차순 정렬
        Subject temp;
        for(int i=1;i<size;i++){
            temp=a[i]; 
            int j=i-1;
            while (j >= 0 && temp.time_table[0].start_time < a[j].time_table[0].start_time){
                a[j + 1] = a[j];
                j--;
            }
            a[j+1]=temp;
        }
    }

       void daySort(Subject* a, int size){ // 오름차순 정렬
        Subject temp;
        for(int i=1;i<size;i++){
            temp=a[i]; 
            int j=i-1;
            while (j >= 0 && temp.time_table[0].day < a[j].time_table[0].day){
                a[j + 1] = a[j];
                j--;
            }
            a[j+1]=temp;
        }
    }

        void scheduleSort(Subject* a, int size){ // 오름차순 정렬
        Subject temp;
        for(int i=1;i<size;i++){
            temp=a[i]; 
            int j=i-1;
            while (j >= 0 && temp.schedule_num < a[j].schedule_num){
                a[j + 1] = a[j];
                j--;
            }
            a[j+1]=temp;
        }
    }

    int minExamDay(Schedule* sc, Subject** sb, Subject* sub) {
        int i, j, min = 0, minSum = 0, sum = 0; // min은 시험이 가장 덜 배정되어있는 요일(1번째 요일을 기본적인 최소로 설정), minSum은 min요일에 해당하는 가중치
        int scheduleNum = sub->schedule_num;
        int userStartTime, userEndTime;
        Subject* ptr; // 요일에 배정된 시험을 가리킬 포인터
        userStartTime = sub->time_table[0].start_time, userEndTime = sub->time_table[0].end_time;
        for (i = 0; i < max_time; i++) { // 1교시부터 15.5교시까지 모든 시험들의 가중치를 검토
            if (sb[sc[0].day][i].subject_name != "") { // 해당 교시에 시험이 배정되어있다면
                ptr = &sb[sc[0].day][i]; // 연결리스트 전체 확인
                while (ptr != NULL) {
                   if ((ptr->subject_name.substr(0, 7) != sub->subject_name.substr(0, 7)) &&
                        !((ptr->time_table->start_time <= userStartTime && ptr->time_table->end_time > userStartTime) || (
                            ptr->time_table->start_time < userEndTime && ptr->time_table->end_time >= userEndTime)))
                        minSum += ptr->student_num;
                    ptr = ptr->next; // 시간이 중복되는 시험이 존재하는지 아래 조건문을 통해 확인
                }
            }
        }

        for (i = 1; i < scheduleNum; i++) { // 나머지 요일들을 검토하여 minSum보다 작은 요일이 발견될경우 해당 요일을 min으로 할당
            userStartTime = sub->time_table[i].start_time, userEndTime = sub->time_table[i].end_time;
            for (j = 0; j < max_time; j++) {
                if (sb[sc[i].day][j].subject_name != "") {
                    ptr = &sb[sc[i].day][j];
                    while (ptr != NULL) {
                       if (ptr->subject_name.substr(0, 7) != sub->subject_name.substr(0, 7) &&
                            !((ptr->time_table->start_time <= userStartTime && ptr->time_table->end_time > userStartTime) || (
                                ptr->time_table->start_time < userEndTime && ptr->time_table->end_time >= userEndTime)))
                            sum += ptr->student_num;
                        ptr = ptr->next;
                    }
                }
            }
            if (minSum > sum) {
                minSum = sum;
                min = i;
            }
            sum = 0;
        }

        return min;
    }
    void allocateExam(Subject *sub) { 
        Schedule* sc;
        Subject* ptr;

        for (int i = 0; i < total_sub; i++) {  // 학생의 모든 수강강의들에 대해 진행
            sc = sub[i].time_table;
            int examDay = minExamDay(sc, this->subject, &sub[i]); // 가중치가 최소인 요일 확인
            ptr = &(this->subject[sub[i].time_table[examDay].day][sub[i].time_table[examDay].start_time]);
            if (ptr->subject_name == "") { // 해당 시간대에 시험이 존재하지 않는다면
                *ptr = sub[i]; // 시험 배정
                ptr->time_table = &sub[i].time_table[examDay];
            }
            else { // 다른 시험이 존재한다면 연결리스트로 연결
                while (ptr->next != NULL) {
                    ptr = ptr->next;
                }
                ptr->next = &sub[i];
                ptr->next->time_table = &sub[i].time_table[examDay];
                ptr = ptr->next;
            }
            sub[i].time_table = ptr->time_table;
        }
    }

    void printExamResultSchedule() {
        Subject* ptr;
        int num = 0;    //요일별 사람 수 저장할 변수
        for (int i = 0; i < 5; i++) {
            cout << i << " : ";
            for (int j = 0; j < max_time; j++) {
                if (subject[i][j].subject_name != "") {
                    ptr = &subject[i][j];
                    cout << ptr->subject_name << " " << ptr->student_num << " ";
                    num += ptr->student_num;
                    while (ptr->next != NULL) {
                        ptr = ptr->next;
                        cout << ptr->subject_name << " " << ptr->student_num << " ";
                        num += ptr->student_num;
                    }
                }
            }
            cout << "요일 사람 수 : " << num << endl;
            num = 0;
            cout << endl;
        }

        cout << endl;
    }

    Subject** getSubject() {
        return this->subject;
    }
};

// n은 학생들의 각자 듣는 과목들의 종류(ex : 1번 학생이 CSE2022-01, EGC2022-02 2번 CSE2022-01, EGC2021-01을 듣는다면 종류는 CSE2022-01, EGC2022-02, EGC2021-01로 총 3개)

int main()
{    
     setData();  
    Q3 q;
    q.initData();   //해시테이블 삽입
    //q.printTable();  //해시테이블 출력

    string* arr = q.splitUnion();
    Subject *result = q.copyTable();

    cout << "정렬 전" << endl;
    for (int i = 0; i < total_sub; i++) {
        cout << "학수번호 : " << result[i].subject_name;
            for (int k = 0; k < result[i].schedule_num; k++) {
                cout << " 날짜 : " << result[i].time_table[k].day;
                cout << " 시작시간 : " << result[i].time_table[k].start_time;
                cout << " 종료시간 : " << result[i].time_table[k].end_time;
            }
            cout << " 가중치 : " << result[i].student_num << endl;
    }
    cout << "---------timesort-----------" << endl;
    q.timeSort(result, total_sub);
    for (int i = 0; i < total_sub; i++) {
        cout << "학수번호 : " << result[i].subject_name;
        for (int k = 0; k < result[i].schedule_num; k++) {
            cout << " 날짜 : " << result[i].time_table[k].day;
            cout << " 시작시간 : " << result[i].time_table[k].start_time;
            cout << " 종료시간 : " << result[i].time_table[k].end_time;
        }
        cout << " 가중치 : " << result[i].student_num << endl;
    }
    cout << "----------daysort-----------\n";
    q.daySort(result, total_sub);
    for (int i = 0; i < total_sub; i++) {
        cout << "학수번호 : " << result[i].subject_name;
        for (int k = 0; k < result[i].schedule_num; k++) {
            cout << " 날짜 : " << result[i].time_table[k].day;
            cout << " 시작시간 : " << result[i].time_table[k].start_time;
            cout << " 종료시간 : " << result[i].time_table[k].end_time;
        }
        cout << " 가중치 : " << result[i].student_num << endl;
    }
    cout << "----------weightsort-----------\n";
    q.weightSort(result, total_sub);
    for (int i = 0; i < total_sub; i++) {
        cout << "학수번호 : " << result[i].subject_name;
        for (int k = 0; k < result[i].schedule_num; k++) {
            cout << " 날짜 : " << result[i].time_table[k].day;
            cout << " 시작시간 : " << result[i].time_table[k].start_time;
            cout << " 종료시간 : " << result[i].time_table[k].end_time;
        }
        cout << " 가중치 : " << result[i].student_num << endl;
    }

   

    cout << "----------schedulesort-----------\n";
    q.scheduleSort(result, total_sub);
    for (int i = 0; i < total_sub; i++) {
        cout << "학수번호 : " << result[i].subject_name;
        for (int k = 0; k < result[i].schedule_num; k++) {
            cout << " 날짜 : " << result[i].time_table[k].day;
            cout << " 시작시간 : " << result[i].time_table[k].start_time;
            cout << " 종료시간 : " << result[i].time_table[k].end_time;
        }
        cout << " 가중치 : " << result[i].student_num << endl;
    }

    cout << endl;
    //q.timeMergesort(result, 0, total_sub - 1);
    //q.dayMergesort(result, 0, total_sub - 1);
    //q.weightMergesort(result, 0, total_sub - 1);
    //q.scheduleMergesort(result, 0, total_sub - 1);
    //q.reverseArr();
    q.allocateExam(result);
    q.printExamResultSchedule();
}