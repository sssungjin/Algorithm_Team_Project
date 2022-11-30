#include <string>
#include <iostream>

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

class subject_table // 강의의 학수번호를 key, Subject를 value로 가지는 해시 테이블
{
	Subject **table;
	int size;

public:
	subject_table(int n)
	{
		size = n;
		table = new Subject* [size]{NULL};
	}

	//해시값을 반환할 함수
	int hash_value(string subject_name)
	{
		int hash_value = 0, a = 33;

		for (int i = 0; i < subject_name.length(); i++)
		{
			hash_value += hash_value * a + subject_name[i];
		}

		return hash_value % size;
	}

	// table에 operation에 해당하는 code를 삽입할 함수
	void insert(Subject *subject)
	{
		int hv = hash_value(subject->subject_name);

		if (table[hv] == NULL)
		{
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
		Subject *ptr;
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

class Q1 {
	Subject **subject; // [0][0]이면 월요일 0(1교시), [3][5]이면 목요일 5(3.5교시)
	int size; // 수강강의의 개수를 저장할 변수

public:
	Q1(int size) {
		this->size = size;
		this->subject = new Subject*[5]; // 5개의 요일 배열 생성(월, 화, 수, 목, 금)
		for (int i = 0; i < 5; i++) {
			this->subject[i] = new Subject[max_time]; // 각각의 요일에 1~15.5교시까지의 배열 생성(시간이 겹치는 강의는 연결 리스트로 연결)
		}
	}

	Subject* timeSort(Subject* src, int n, int k) { // 빠른 시작시간 기준 계수정렬, src는 정렬할 배열, dest는 정렬이 완료된 배열, n은 src 원소의 개수, k는 교시의 범위
		Subject *dest = new Subject[size];
		int *N = new int[k+1] {0};
		int i;
		for (i = 1; i <= n; i++)
			N[src[i-1].time_table[0].start_time] = N[src[i-1].time_table[0].start_time] + 1;
		for (i = 2; i <= k; i++)
			N[i] = N[i] + N[i - 1];
		for (i = n; i >= 1; i--) {
			dest[N[src[i-1].time_table[0].start_time] - 1] = src[i-1];
			N[src[i-1].time_table[0].start_time] = N[src[i-1].time_table[0].start_time] - 1;
		}
		return dest;
	}

	Subject* daySort(Subject* src, int n, int k) { // 빠른 요일 기준 계수정렬, src는 정렬할 배열, dest는 정렬이 완료된 배열, n은 src 원소의 개수, k는 요일의 범위
		Subject *dest = new Subject[size];
		int *N = new int[k] {0};
		int i;
		for (i = 1; i <= n; i++)
			N[src[i - 1].time_table[0].day] = N[src[i - 1].time_table[0].day] + 1;
		for (i = 1; i <= k-1; i++)
			N[i] = N[i] + N[i - 1];
		for (i = n; i >= 1; i--) {
			dest[N[src[i - 1].time_table[0].day] -1] = src[i - 1];
			N[src[i - 1].time_table[0].day] = N[src[i - 1].time_table[0].day] - 1;
		}
		return dest;
	}

	Subject* scheduleNumSort(Subject* src, int n, int k) { // 작은 강의 시간 분할 수 기준 계수정렬, src는 정렬할 배열, dest는 정렬이 완료된 배열, n은 src 원소의 개수, k는 분할 가능한 수의 범위
		Subject *dest = new Subject[size];
		int *N = new int[k+1] {0};
		int i;
		for (i = 1; i <= n; i++)
			N[src[i - 1].schedule_num] = N[src[i - 1].schedule_num] + 1;
		for (i = 2; i <= k; i++)
			N[i] = N[i] + N[i - 1];
		for (i = n; i >= 1; i--) {
			dest[N[src[i - 1].schedule_num] - 1] = src[i-1];
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

	int minExamDay(Schedule *sc, Subject **sb, int scheduleNum) { // 강의 요일 중 가장 시험이 덜 배정되어있는(가중치가 가장 작은) 요일에 해당하는 Schedule 배열의 인덱스를 반환하는 함수
		int i, j, min = 0, minSum = 0, sum = 0; // min은 시험이 가장 덜 배정되어있는 요일(1번째 요일을 기본적인 최소로 설정), minSum은 min요일에 해당하는 가중치
		Subject *ptr; // 요일에 배정된 시험을 가리킬 포인터
		for (i = 0; i < max_time; i++) { // 1교시부터 15.5교시까지 모든 시험들의 가중치를 검토
			if (sb[sc[0].day][i].subject_name != "") { // 해당 교시에 시험이 배정되어있다면
				ptr = &sb[sc[0].day][i];
				minSum += ptr->student_num;
				while(ptr->next != NULL) {
					ptr = ptr->next; // 시간이 중복되는 시험이 존재하는지 아래 조건문을 통해 확인
					minSum += ptr->student_num; // 가중치 추가
				}
			}
		}

		for (i = 1; i < scheduleNum; i++) { // 나머지 요일들을 검토하여 minSum보다 작은 요일이 발견될경우 해당 요일을 min으로 할당
			for (j = 0; j < max_time; j++) {
				if (sb[sc[i].day][j].subject_name != "") {
					ptr = &sb[sc[i].day][j];
					sum += ptr->student_num;
					while(ptr->next != NULL) {
						ptr = ptr->next;
						sum += ptr->student_num;
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

	void allocateExam(Subject* userSubject, int size) { // minExamDay를 토대로 시험을 배정하는 함수
		Schedule *sc;
		Subject *ptr;

		for (int i = 0; i < size; i++) {  // 학생의 모든 수강강의들에 대해 진행
			sc = userSubject[i].time_table;
			int examDay = minExamDay(sc, this->subject, userSubject[i].schedule_num);
			if (this->subject[userSubject[i].time_table[examDay].day][userSubject[i].time_table[examDay].start_time].subject_name == "")
				this->subject[userSubject[i].time_table[examDay].day][userSubject[i].time_table[examDay].start_time] = userSubject[i];
			else {
				ptr = &(this->subject[userSubject[i].time_table[examDay].day][userSubject[i].time_table[examDay].start_time]);
				while (ptr->next != NULL) {
					ptr = ptr->next;
				}
				ptr->next = &userSubject[i];
			}
		}
	}

	void printExamSchedule() {
		Subject* ptr;
		for (int i = 0; i < 5; i++) {
			cout << i  << " : ";
			for (int j = 0; j < max_time; j++) {
				if (subject[i][j].subject_name != "") {
					ptr = &subject[i][j];
					cout << ptr->subject_name << " " << ptr->time_table[0].start_time << " ";
					while (ptr->next != NULL) {
						ptr = ptr->next;
						cout << ptr->subject_name << " " << ptr->time_table[0].start_time << " ";
					}
				}
			}
			cout << endl;
		}
	}

	void what(string **subjectList) {
		Subject* subject = new Subject[size];
		subject[0].subject_name = "EGC7026-06";
		subject[0].time_table = new Schedule;
		subject[0].time_table->day = fri;
		subject[0].time_table->start_time = 18;
		subject[0].time_table->end_time = 23;
		subject[0].schedule_num = 1;
		subject[0].student_num = 1;
		subject[0].next = NULL;
		subject[1].subject_name = "EGC9018-01";
		subject[1].time_table = new Schedule;
		subject[1].time_table->day = wed;
		subject[1].time_table->start_time = 9;
		subject[1].time_table->end_time = 14;
		subject[1].schedule_num = 1;
		subject[1].student_num = 1;
		subject[1].next = NULL;
		subject[2].subject_name = "CSE2015-03";
		subject[2].time_table = new Schedule[2];
		subject[2].time_table[0].day = mon;
		subject[2].time_table[0].start_time = 16;
		subject[2].time_table[0].end_time = 19;
		subject[2].time_table[1].day = fri;
		subject[2].time_table[1].start_time = 15;
		subject[2].time_table[1].end_time = 17;
		subject[2].schedule_num = 2;
		subject[2].student_num = 1;
		subject[2].next = NULL;
		subject[3].subject_name = "CSE2024-02";
		subject[3].time_table = new Schedule[2];
		subject[3].time_table[0].day = tue;
		subject[3].time_table[0].start_time = 16;
		subject[3].time_table[0].end_time = 18;
		subject[3].time_table[1].day = thu;
		subject[3].time_table[1].start_time = 16;
		subject[3].time_table[1].end_time = 18;
		subject[3].schedule_num = 2;
		subject[3].student_num = 1;
		subject[3].next = NULL;
		subject[4].subject_name = "CSE4029-01";
		subject[4].time_table = new Schedule[2];
		subject[4].time_table[0].day = tue;
		subject[4].time_table[0].start_time = 12;
		subject[4].time_table[0].end_time = 15;
		subject[4].time_table[1].day = thu;
		subject[4].time_table[1].start_time = 12;
		subject[4].time_table[1].end_time = 15;
		subject[4].schedule_num = 2;
		subject[4].student_num = 1;
		subject[4].next = NULL;
		subject[5].subject_name = "CSE2013-01";
		subject[5].time_table = new Schedule[2];
		subject[5].time_table[0].day = mon;
		subject[5].time_table[0].start_time = 8;
		subject[5].time_table[0].end_time = 11;
		subject[5].time_table[1].day = wed;
		subject[5].time_table[1].start_time = 2;
		subject[5].time_table[1].end_time = 5;
		subject[5].schedule_num = 2;
		subject[5].student_num = 1;
		subject[5].next = NULL;

		subject = radixSort(subject);
		allocateExam(subject, size);
		printExamSchedule();
	} 
};

// n은 학생들의 각자 듣는 과목들의 종류(ex : 1번 학생이 CSE2022-01, EGC2022-02 2번 CSE2022-01, EGC2021-01을 듣는다면 종류는 CSE2022-01, EGC2022-02, EGC2021-01로 총 3개)

int main() {
	Q1 q1(6);
	q1.what(NULL);
}