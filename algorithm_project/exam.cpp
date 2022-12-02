#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

#define max_snum 3 // 입력으로 쓸 학생 수
#define max_subject 24 // 수강 가능한 강의의 수
#define max_time 30 // 강의 교시 수(n = (1 + 0.5*n)교시) (n >= 0인 30보다 작은 정수)
#define max_day 5
#define max_scheduleNum 3 // 가능한 강의 시간 분할 수(일반물리학및실험1의 경우 강의가 3개의 시간으로 분할됨)

enum Day {mon, tue, wed, thu, fri}; // 요일에 따른 수를 열거한 열거형

typedef struct schedule{
	enum Day day; // 요일
	int start_time; // 해당 요일에서 강의 시작교시
	int end_time; // 해당 요일에서 강의 종료교시
} Schedule; // 요일에 따른 강의 시간 정보를 담고 있는 구조체

typedef struct subject {
	string subject_name; // 강의 학수번호
	Schedule *time_table; // 요일별 강의시간
	int schedule_num; // 강의시간 분할 수
	int student_num; // 해당 강의를 듣고 있는 학생의 수
	struct subject* next;
} Subject; // 강의의 정보를 담는 구조체

string **student; // 학생의 수강강의 학수번호를 담는 이중포인터(ex : 1번 학생은 CSE2022-01, EGC2022-02... 을 듣고 있다면 student[1][0] = "CSE2022-01" student[1][1] = "EGC2022-02")

int student_cnt; // 데이터로 받아온 학생 수
int *student_subjectNum; // 학생별 수강과목 수를 저장할 포인터
Subject **student_sub; // 학생들의 학수번호를 토대로 Subject 구조체로 변환

void setData() {
	ifstream fp("totalData.txt");
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
	student = new string*[student_cnt]; // 학생수만큼의 포인터 배열 할당
	student_sub = new Subject*[student_cnt]; // 학생수만큼의 포인터 배열 크기 할당
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
			student_sub[pCnt][sCnt].next = NULL;

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

class Q1 {
	Subject **subject; // [0][0]이면 월요일 0(1교시), [3][5]이면 목요일 5(3.5교시)
	Subject **resultSubject;
	int size; // 수강강의의 개수를 저장할 변수

public:
	Q1() {
		this->subject = new Subject*[5]; // 5개의 요일 배열 생성(월, 화, 수, 목, 금)
		this->resultSubject = new Subject*[5];
		for (int i = 0; i < 5; i++) {
			this->subject[i] = new Subject[max_time]; // 각각의 요일에 1~15.5교시까지의 배열 생성(시간이 겹치는 강의는 연결 리스트로 연결)
			this->resultSubject[i] = new Subject[max_time];
		}
	}

	void setSize(int size) {
		this->size = size;
	}

	Subject* timeSort(Subject* src, int n, int k) { // 빠른 시작시간 기준 계수정렬, src는 정렬할 배열, dest는 정렬이 완료된 배열, n은 src 원소의 개수, k는 교시의 범위
		Subject *dest = new Subject[size];
		int *N = new int[k]{ 0 };
		int i;
		for (i = 1; i <= n; i++)
			N[src[i-1].time_table[0].start_time] = N[src[i-1].time_table[0].start_time] + 1;
		for (i = 1; i < k; i++)
			N[i] = N[i] + N[i - 1];
		for (i = n-1; i >= 0; i--) {
			dest[N[src[i].time_table[0].start_time] - 1] = src[i];
			N[src[i].time_table[0].start_time] = N[src[i].time_table[0].start_time] - 1;
		}
		return dest;
	}

	Subject* daySort(Subject* src, int n, int k) { // 빠른 요일 기준 계수정렬, src는 정렬할 배열, dest는 정렬이 완료된 배열, n은 src 원소의 개수, k는 요일의 범위
		Subject *dest = new Subject[size];
		int *N = new int[k] {0};
		int i;
		for (i = 1; i <= n; i++)
			N[src[i - 1].time_table[0].day] = N[src[i - 1].time_table[0].day] + 1;
		for (i = 1; i <= k - 1; i++)
			N[i] = N[i] + N[i - 1];
		for (i = n; i >= 1; i--) {
			dest[N[src[i - 1].time_table[0].day] - 1] = src[i - 1];
			N[src[i - 1].time_table[0].day] = N[src[i - 1].time_table[0].day] - 1;
		}
		return dest;
	}

	Subject* scheduleNumSort(Subject* src, int n, int k) { // 작은 강의 시간 분할 수 기준 계수정렬, src는 정렬할 배열, dest는 정렬이 완료된 배열, n은 src 원소의 개수, k는 분할 가능한 수의 범위
		Subject *dest = new Subject[size];
		int *N = new int[k + 1]{ 0 };
		int i;
		for (i = 1; i <= n; i++)
			N[src[i - 1].schedule_num] = N[src[i - 1].schedule_num] + 1;
		for (i = 2; i <= k; i++)
			N[i] = N[i] + N[i - 1];
		for (i = n; i >= 1; i--) {
			dest[N[src[i - 1].schedule_num] - 1] = src[i - 1];
			N[src[i - 1].schedule_num] = N[src[i - 1].schedule_num] - 1;
		}
		return dest;
	}

	Subject* radixSort(Subject *subject) { // 요일, 시작 시간, 강의 시간 분할 수에 따른 기수정렬
		subject = timeSort(subject, size, max_time);
		subject = daySort(subject, size, max_day);
		subject = scheduleNumSort(subject, size, max_scheduleNum);
		// 학생별로 과목 정렬. 과목 정렬 순서는  1번째 적은 강의 시간 분할 수, 2번째 같은 분할 수라면 빠른 요일별, 3번째 같은 요일이라면 빠른 시간대

		return subject;
	}

	int minExamDay(Schedule *sc, Subject **sb, Subject* userSubject) { // 강의 요일 중 가장 시험이 덜 배정되어있는(가중치가 가장 작은) 요일에 해당하는 Schedule 배열의 인덱스를 반환하는 함수
		int i, j, min = 0, minSum = 0, sum = 0; // min은 시험이 가장 덜 배정되어있는 요일(1번째 요일을 기본적인 최소로 설정), minSum은 min요일에 해당하는 가중치
		int scheduleNum = userSubject->schedule_num;
		int userStartTime, userEndTime;
		Subject *ptr; // 요일에 배정된 시험을 가리킬 포인터
		userStartTime = userSubject->time_table[0].start_time, userEndTime = userSubject->time_table[0].end_time;
		for (i = 0; i < max_time; i++) { // 1교시부터 15.5교시까지 모든 시험들의 가중치를 검토
			if (sb[sc[0].day][i].subject_name != "") { // 해당 교시에 시험이 배정되어있다면
				ptr = &sb[sc[0].day][i]; // 연결리스트 전체 확인
				while (ptr != NULL) {
					if ((ptr->subject_name.substr(0, 7) != userSubject->subject_name.substr(0, 7)) &&
						!((ptr->time_table->start_time <= userStartTime && ptr->time_table->end_time > userStartTime) || (
							ptr->time_table->start_time < userEndTime && ptr->time_table->end_time >= userEndTime))) // 만약에 같은 교과목의 분반이 아니거나 시간대가 겹치는 강의가 아니라면
						minSum += ptr->student_num;
					ptr = ptr->next; // 시간이 중복되는 시험이 존재하는지 아래 조건문을 통해 확인
				}
			}
		}

		for (i = 1; i < scheduleNum; i++) { // 나머지 요일들을 검토하여 minSum보다 작은 요일이 발견될경우 해당 요일을 min으로 할당
			userStartTime = userSubject->time_table[i].start_time, userEndTime = userSubject->time_table[i].end_time;
			for (j = 0; j < max_time; j++) {
				if (sb[sc[i].day][j].subject_name != "") {
					ptr = &sb[sc[i].day][j];
					while (ptr != NULL) {
						if (ptr->subject_name.substr(0, 7) != userSubject->subject_name.substr(0, 7) &&
							!((ptr->time_table->start_time <= userStartTime && ptr->time_table->end_time > userStartTime) || (
								ptr->time_table->start_time < userEndTime && ptr->time_table->end_time >= userEndTime))) // 만약에 같은 교과목의 분반이 아니거나 시간대가 겹치는 강의가 아니라면
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

		return min; // 시험 최소 배정 요일에 해당하는 Schedule 배열의 인덱스 반환
	}

	void allocateExam(Subject* userSubject, int size) { // minExamDay를 토대로 한 사람의 시험을 배정하는 함수
		Schedule *sc;
		Subject *ptr;

		for (int i = 0; i < size; i++) {  // 학생의 모든 수강강의들에 대해 진행
			sc = userSubject[i].time_table;
			int examDay = minExamDay(sc, this->subject, &userSubject[i]); // 가중치가 최소인 요일 확인
			ptr = &(this->subject[userSubject[i].time_table[examDay].day][userSubject[i].time_table[examDay].start_time]);
			if (ptr->subject_name == "") { // 해당 시간대에 시험이 존재하지 않는다면
				*ptr = userSubject[i]; // 시험 배정
				ptr->time_table = & userSubject[i].time_table[examDay];
			}
			else { // 다른 시험이 존재한다면 연결리스트로 연결
				while (ptr->next != NULL) {
					ptr = ptr->next;
				}
				ptr->next = &userSubject[i];
				ptr->next->time_table = &userSubject[i].time_table[examDay];
				ptr = ptr->next;
			}
			userSubject[i].time_table = ptr->time_table;
		}
	}

	void allocateExamResult(Subject* userSubject, int size) { // minExamDay를 토대로 한 사람의 시험을 배정하는 함수
		Subject *ptr;
		for (int i = 0; i < size; i++) {
			ptr = &(this->resultSubject[userSubject[i].time_table->day][userSubject[i].time_table->start_time]);
			if (ptr->subject_name != "") { // 이미 배정되어 있는 시간표가 있다면
				while (ptr->next != NULL) {
					if (ptr->subject_name == userSubject[i].subject_name) { // 지금 강의의 이름과 같다면 가중치 증가
						ptr->student_num++;
						break;
					}
					ptr = ptr->next;
				}
				if (ptr->subject_name == userSubject[i].subject_name) { // 지금 강의의 이름과 같다면 가중치 증가
					ptr->student_num++;
				}
				else {
					ptr->next = new Subject;
					*ptr->next = userSubject[i];
				}
			}
			else
				*ptr = userSubject[i];
		}
	}

	void printExamSchedule() {
		Subject* ptr;
		for (int i = 0; i < 5; i++) {
			cout << i << " : ";
			for (int j = 0; j < max_time; j++) {
				if (subject[i][j].subject_name != "") {
					ptr = &subject[i][j];
					cout << ptr->subject_name << " ";
					while (ptr->next != NULL) {
						ptr = ptr->next;
						cout << ptr->subject_name << " ";
					}
				}
			}
			cout << endl;
		}
		cout << endl;
	}

	void printExamResultSchedule() {
		Subject* ptr;
		for (int i = 0; i < 5; i++) {
			cout << i << " : ";
			for (int j = 0; j < max_time; j++) {
				if (resultSubject[i][j].subject_name != "") {
					ptr = &resultSubject[i][j];
					cout << ptr->subject_name << " " << ptr->student_num << " ";
					while (ptr->next != NULL) {
						ptr = ptr->next;
						cout << ptr->subject_name << " " << ptr->student_num << " ";
					}
				}
			}
			cout << endl;
		}
		cout << endl;
	}

	void deleteExamResult(Subject* sub, int size) {
		int min, different;
		for (int i = 0; i < size; i++) {
			min = 0, different = 0;
			for (int j = 1; j < sub[i].schedule_num; j++) {
				if (resultSubject[sub[i].time_table[j].day][sub[i].time_table[j].start_time].student_num != resultSubject[sub[i].time_table[min].day][sub[i].time_table[min].start_time].student_num)
					different = 1;
				if (resultSubject[sub[i].time_table[j].day][sub[i].time_table[j].start_time].student_num > resultSubject[sub[i].time_table[min].day][sub[i].time_table[min].start_time].student_num)
					min = j;
			}
			if (different == 0) {
				min = minExamDay(sub[i].time_table, this->resultSubject, &sub[i]);
			}
			for (int j = 0; j < sub[i].schedule_num; j++) {
				if (j != min)
					resultSubject[sub[i].time_table[j].day][sub[i].time_table[j].start_time] = { "", NULL, 0, 0, NULL };
			}
		}
	}

	void examAllocate(Subject** subjectList, int student_num) {
		Subject *subject;
		for (int i = 0; i < student_num; i++) {
			subject = subjectList[i];
			setSize(student_subjectNum[i]);
			subject = radixSort(subject);
			allocateExam(subject, size);
			cout << "전체 결과 할당 전\n";
			printExamSchedule();
			allocateExamResult(subject, size);
			cout << "전체 결과 할당 후\n";
			printExamResultSchedule();
			delete this->subject;
			this->subject = new Subject*[5]; // 5개의 요일 배열 생성(월, 화, 수, 목, 금)
			for (int i = 0; i < 5; i++) {
				this->subject[i] = new Subject[max_time]; // 각각의 요일에 1~15.5교시까지의 배열 생성(시간이 겹치는 강의는 연결 리스트로 연결)
			}
		}
		for (int i = 0; i < student_num; i++) {
			setSize(student_subjectNum[i]);
			deleteExamResult(subjectList[i], size);
			cout << "삭제 후\n";
			printExamResultSchedule();
		}
	}
};

int main() {
	setData();
	Q1 q1;
	q1.examAllocate(student_sub ,student_cnt);
}