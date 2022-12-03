#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

using namespace std;

#define max_snum 3 // �Է����� �� �л� ��
#define max_subject 24 // ���� ������ ������ ��
#define max_time 30 // ���� ���� ��(n = (1 + 0.5*n)����) (n >= 0�� 30���� ���� ����)
#define max_day 5
#define max_scheduleNum 3 // ������ ���� �ð� ���� ��(�Ϲݹ����й׽���1�� ��� ���ǰ� 3���� �ð����� ���ҵ�)

enum Day { mon, tue, wed, thu, fri }; // ���Ͽ� ���� ���� ������ ������
typedef struct schedule {
    enum Day day; // ����
    int start_time; // �ش� ���Ͽ��� ���� ���۽ð�
    int end_time; // �ش� ���Ͽ��� ���� ����ð�
} Schedule; // ���Ͽ� ���� ���� �ð� ������ ��� �ִ� ����ü

typedef struct subject {
    string subject_name; // ���� �м���ȣ
    Schedule* time_table; // ���Ϻ� ���ǽð�
    int student_num; // �ش� ���Ǹ� ��� �ִ� �л��� ��
    int schedule_num; // ���ǽð� ���� ��
    struct subject* next;
} Subject; // ������ ������ ��� ����ü


class subject_table // ������ �м���ȣ�� key, Subject�� value�� ������ �ؽ� ���̺�
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

    //�ؽð��� ��ȯ�� �Լ�
    int hash_value(string subject_name)
    {
       int hash_value = 0, a = 3;

        for (unsigned int i = 0; i < subject_name.length(); i++)
        {
            hash_value += hash_value * a + subject_name[i];
        }

        return hash_value % size;
    }

    // table�� �������� �迭 ������ �Լ�
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

    // table���� operation�� �ش��ϴ� code�� �������� �Լ�
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

// --- ���� �ڵ� ----


int student_cnt; // �����ͷ� �޾ƿ� �л� ��
int* student_subjectNum;    //�л��� ���� �� �迭
Subject** student_sub;
string** student;
string sb_union;   //�ߺ� ���� �м���ȣ �ϳ��� ���ڿ��� ����
int total_sub = 0;  //��ü ���� ��
subject_table st(10009);
Subject* sorted;
void setData() {
    ifstream fp("C:\\Users\\nicol\\Desktop\\�˰�����\\Algorithm_Team_Project\\Q3\\totalData.txt");
    if (!fp.is_open()) { cout << "not open" << endl; return; }
    string fpstr; // ���Ϸκ��� �о�� �� ������ ������ ����
    string subject_code, time[3]; // �м���ȣ, �κк��� ������ �ð�ǥ(�� ����� �ִ� 3������ ������ ���� �����)
    int cnt_idx, cnt_len;           // student_cnt�� �������� ���� �ʿ��� ����

    // �л�����ŭ �����Ҵ��ϴ� ����
    getline(fp, fpstr);
    cnt_idx = 12; // student_cnt:(����)�� ���ڰ��� �������� ����
    cnt_len = fpstr.length();
    fpstr = fpstr.substr(cnt_idx, cnt_len - cnt_idx);
    student_cnt = stoi(fpstr);
    student = new string * [student_cnt]; // �л�����ŭ�� ������ �迭 �Ҵ�
    student_sub = new Subject * [student_cnt]; // �л�����ŭ�� ������ �迭 ũ�� �Ҵ�
    student_subjectNum = new int[student_cnt]; // �л�����ŭ ������ �迭 ũ�� �Ҵ�

    // student ������ �迭 ������ ���� ����
    for (int pCnt = 0; pCnt < student_cnt; pCnt++) {
        getline(fp, fpstr); // ���� ���� �����ִ� ����        
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

            // �л��� Subject ����ü ��� �ʱ�ȭ ����
            student_sub[pCnt][sCnt].subject_name = subject_code;
            student_sub[pCnt][sCnt].student_num = 1;
            int partCnt = 0;          // �Ѱ����� ���ҵ� ����
            for (int i = 0; i < 3; i++)
                if (!time[i].empty()) partCnt++;
            student_sub[pCnt][sCnt].schedule_num = partCnt;
            student_sub[pCnt][sCnt].time_table = new schedule[partCnt];

            // �л��� schedule �ʱ�ȭ ����
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
    Subject** subject; //[0][0]�̸� ������ 0(1����) [3][5]�� ����� 5(3.5����)
public:
    Q3(){  //size�� total_sub
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
    void initData() {   //�ؽ����̺� ���� ����ü ����
        sb_union = "";
        for (int i = 0; i < student_cnt; i++) {
            for(int j = 0; j < student_subjectNum[i]; j++)
            if (st.get_value(student_sub[i][j].subject_name)) {  //�ؽ����̺� �����ϸ� ����ġ�� ++
                st.get_value(student_sub[i][j].subject_name)->student_num++;
            }
            else {  //�ؽ����̺� ������ ����ġ 1�� ����, �ؽ����̺� insert
                sb_union += student_sub[i][j].subject_name + " ";   //�տ������� �м���ȣ �ߺ� ���� ����
                total_sub++;
                st.insert(&student_sub[i][j]);
            }
        }
        
    }
    //���ڿ��� �������� ���е� �м���ȣ �������� ���ڿ� �迭�� ����
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

    void printTable() {     //�ؽ����̺� ����� ���� ��� student_cnt �� �л� ��
        for (int i = 0; i < student_cnt; i++) {
            for (int j = 0; j < student_subjectNum[i]; j++) {   //student_subjectNum�� �л��� ��� ���� ��
                Subject* ptr = st.get_value(student[i][j]);
                cout << "�м���ȣ : " << ptr->subject_name;
                for (int k = 0; k < ptr->schedule_num; k++) {
                    cout << " ��¥ : " << ptr->time_table[k].day;
                    cout << " ���۽ð� : " << ptr->time_table[k].start_time;
                    cout << " ����ð� : " << ptr->time_table[k].end_time;
                }
                cout << " ����ġ : " << ptr->student_num << endl;
            }
        }
    }
    //�ؽ����̺� �ִ� ������ ������ �迭 ��ȯ
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


    void weightSort(Subject* a, int size){ // �������� ����
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

    void timeSort(Subject* a, int size){ // �������� ����
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

       void daySort(Subject* a, int size){ // �������� ����
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

        void scheduleSort(Subject* a, int size){ // �������� ����
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
        int i, j, min = 0, minSum = 0, sum = 0; // min�� ������ ���� �� �����Ǿ��ִ� ����(1��° ������ �⺻���� �ּҷ� ����), minSum�� min���Ͽ� �ش��ϴ� ����ġ
        int scheduleNum = sub->schedule_num;
        int userStartTime, userEndTime;
        Subject* ptr; // ���Ͽ� ������ ������ ����ų ������
        userStartTime = sub->time_table[0].start_time, userEndTime = sub->time_table[0].end_time;
        for (i = 0; i < max_time; i++) { // 1���ú��� 15.5���ñ��� ��� ������� ����ġ�� ����
            if (sb[sc[0].day][i].subject_name != "") { // �ش� ���ÿ� ������ �����Ǿ��ִٸ�
                ptr = &sb[sc[0].day][i]; // ���Ḯ��Ʈ ��ü Ȯ��
                while (ptr != NULL) {
                   if ((ptr->subject_name.substr(0, 7) != sub->subject_name.substr(0, 7)) &&
                        !((ptr->time_table->start_time <= userStartTime && ptr->time_table->end_time > userStartTime) || (
                            ptr->time_table->start_time < userEndTime && ptr->time_table->end_time >= userEndTime)))
                        minSum += ptr->student_num;
                    ptr = ptr->next; // �ð��� �ߺ��Ǵ� ������ �����ϴ��� �Ʒ� ���ǹ��� ���� Ȯ��
                }
            }
        }

        for (i = 1; i < scheduleNum; i++) { // ������ ���ϵ��� �����Ͽ� minSum���� ���� ������ �߰ߵɰ�� �ش� ������ min���� �Ҵ�
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

        for (int i = 0; i < total_sub; i++) {  // �л��� ��� �������ǵ鿡 ���� ����
            sc = sub[i].time_table;
            int examDay = minExamDay(sc, this->subject, &sub[i]); // ����ġ�� �ּ��� ���� Ȯ��
            ptr = &(this->subject[sub[i].time_table[examDay].day][sub[i].time_table[examDay].start_time]);
            if (ptr->subject_name == "") { // �ش� �ð��뿡 ������ �������� �ʴ´ٸ�
                *ptr = sub[i]; // ���� ����
                ptr->time_table = &sub[i].time_table[examDay];
            }
            else { // �ٸ� ������ �����Ѵٸ� ���Ḯ��Ʈ�� ����
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
        int num = 0;    //���Ϻ� ��� �� ������ ����
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
            cout << "���� ��� �� : " << num << endl;
            num = 0;
            cout << endl;
        }

        cout << endl;
    }

    Subject** getSubject() {
        return this->subject;
    }
};

// n�� �л����� ���� ��� ������� ����(ex : 1�� �л��� CSE2022-01, EGC2022-02 2�� CSE2022-01, EGC2021-01�� ��´ٸ� ������ CSE2022-01, EGC2022-02, EGC2021-01�� �� 3��)

int main()
{    
     setData();  
    Q3 q;
    q.initData();   //�ؽ����̺� ����
    //q.printTable();  //�ؽ����̺� ���

    string* arr = q.splitUnion();
    Subject *result = q.copyTable();

    cout << "���� ��" << endl;
    for (int i = 0; i < total_sub; i++) {
        cout << "�м���ȣ : " << result[i].subject_name;
            for (int k = 0; k < result[i].schedule_num; k++) {
                cout << " ��¥ : " << result[i].time_table[k].day;
                cout << " ���۽ð� : " << result[i].time_table[k].start_time;
                cout << " ����ð� : " << result[i].time_table[k].end_time;
            }
            cout << " ����ġ : " << result[i].student_num << endl;
    }
    cout << "---------timesort-----------" << endl;
    q.timeSort(result, total_sub);
    for (int i = 0; i < total_sub; i++) {
        cout << "�м���ȣ : " << result[i].subject_name;
        for (int k = 0; k < result[i].schedule_num; k++) {
            cout << " ��¥ : " << result[i].time_table[k].day;
            cout << " ���۽ð� : " << result[i].time_table[k].start_time;
            cout << " ����ð� : " << result[i].time_table[k].end_time;
        }
        cout << " ����ġ : " << result[i].student_num << endl;
    }
    cout << "----------daysort-----------\n";
    q.daySort(result, total_sub);
    for (int i = 0; i < total_sub; i++) {
        cout << "�м���ȣ : " << result[i].subject_name;
        for (int k = 0; k < result[i].schedule_num; k++) {
            cout << " ��¥ : " << result[i].time_table[k].day;
            cout << " ���۽ð� : " << result[i].time_table[k].start_time;
            cout << " ����ð� : " << result[i].time_table[k].end_time;
        }
        cout << " ����ġ : " << result[i].student_num << endl;
    }
    cout << "----------weightsort-----------\n";
    q.weightSort(result, total_sub);
    for (int i = 0; i < total_sub; i++) {
        cout << "�м���ȣ : " << result[i].subject_name;
        for (int k = 0; k < result[i].schedule_num; k++) {
            cout << " ��¥ : " << result[i].time_table[k].day;
            cout << " ���۽ð� : " << result[i].time_table[k].start_time;
            cout << " ����ð� : " << result[i].time_table[k].end_time;
        }
        cout << " ����ġ : " << result[i].student_num << endl;
    }

   

    cout << "----------schedulesort-----------\n";
    q.scheduleSort(result, total_sub);
    for (int i = 0; i < total_sub; i++) {
        cout << "�м���ȣ : " << result[i].subject_name;
        for (int k = 0; k < result[i].schedule_num; k++) {
            cout << " ��¥ : " << result[i].time_table[k].day;
            cout << " ���۽ð� : " << result[i].time_table[k].start_time;
            cout << " ����ð� : " << result[i].time_table[k].end_time;
        }
        cout << " ����ġ : " << result[i].student_num << endl;
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