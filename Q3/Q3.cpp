#include <iostream>
#include <fstream>
#include <sstream>
#include <climits>

using namespace std;

#define max_subject 24 // 수강 가능한 강의의 수, 총 24학점까지 수강가능(1학점 강의 24개가 최대)
#define max_time 30 // 강의 교시 수(배열의 인덱스 n = (1 + 0.5*n)교시) (0 <= n < 30인 정수)
#define max_day 5 // 요일의 개수(월~금)
#define max_scheduleNum 3 // 가능한 강의 시간 분할 수(일반물리학및실험1의 경우 강의가 3개의 시간으로 분할됨)

enum Day {mon, tue, wed, thu, fri}; // 요일에 따른 정수를 열거한 열거형

typedef struct schedule{
	enum Day day; // 요일
	int start_time; // 해당 요일에서 강의 시작교시
	int end_time; // 해당 요일에서 강의 종료교시
} Schedule; // 요일에 따른 강의 시간 정보를 담고 있는 구조체

typedef struct subject {
	string subject_name;	 // 강의 학수번호
	Schedule *time_table; // 요일별 강의시간
	int schedule_num; // 강의시간 분할 수
	int student_num; // 해당 강의를 듣고 있는 학생의 수, 가중치로 사용
	struct subject* next; // 시작 시간이 겹치는 강의를 이을 연결리스트 포인터로 사용
} Subject; // 강의의 정보를 담는 구조체

int student_cnt; // 데이터로 받아온 학생의 수를 저장할 변수
int *student_subjectNum; // 학생별 수강과목의 수를 저장할 포인터
Subject **student_sub; // 학생들의 학수번호를 토대로 Subject 구조체로 변환
int compare_cnt = 0;     //시간복잡도를 위한 비교횟수 저장할 변수

void setData() { // 파일로부터 강의의 정보를 받아 Subject 구조체를 형성하는 함수
	ifstream fp("C:\\Users\\nicol\\Desktop\\algorithm_project\\Algorithm_Team_Project\\Q3\\totalData.txt"); // totalData
	if (!fp.is_open()) { cout << "not open" << endl; return; }
	string fpstr; // 파일로부터 읽어올 한 문장을 저장할 변수
	string subject_code, time[max_scheduleNum]; // 학수번호를 저장한 string, 부분별로 나눠진 시간표(한 과목당 최대 3번으로 나눠져 수업 진행됨)
	int cnt_idx, cnt_len; // student_cnt를 가져오기 위해 필요한 변수

	// 학생수만큼 동적할당하는 과정, 파일의 첫 문장에는 학생들의 수가 입력되어있음
	getline(fp, fpstr);
	cnt_len = fpstr.length(); // 첫 문장의 길이를 저장
	fpstr = fpstr.substr(12, cnt_len - 12); // 학생의 수에 해당하는 문자열을 저장(totalData.txt에 student_cnt:~~ 문장에서 ~~만 추출하는 과정)
	student_cnt = stoi(fpstr); // 읽어올 학생의 수 문자열을 숫자로 전환
	student_sub = new Subject*[student_cnt]; // 학생수만큼의 Subject 구조체 포인터 배열 크기 할당
	student_subjectNum = new int[student_cnt]; // 학생수만큼 각 학생의 수강강의수를 저장할 정수형 포인터 배열 크기 할당

	// student_sub 이차원 포인터 데이터 설정 과정
	for (int pCnt = 0; pCnt < student_cnt; pCnt++) { // 읽어온 학생들의 수만큼 반복
		getline(fp, fpstr); // 수강강의 개수가 적혀있는 문장을 읽어옴
		int subject_cnt = stoi(fpstr); // 수강강의 개수를 저장

		student_sub[pCnt] = new Subject[subject_cnt]; // 수강강의 개수만큼 student_sub의 배열생성
		student_subjectNum[pCnt] = subject_cnt; // 학생별 수강강의 개수를 해당 인덱스에 저장

		for (int sCnt = 0; sCnt < subject_cnt; sCnt++) { // 각 학생의 수강강의 개수만큼 반복
			time[0] = time[1] = time[2] = "";  // 시간표를 받아올 배열 초기화
			int scheduleNum = 0; // 시간표의 분할 수 초기화
			getline(fp, fpstr); // 강의 하나의 정보를 파일로부터 읽어옴
			stringstream split(fpstr);
			split >> subject_code >> time[0] >> time[1] >> time[2]; // 공백기준으로 강의 정보를 끊어서 저장

			// 학생별 Subject 구조체 멤버 초기화 진행
			student_sub[pCnt][sCnt].subject_name = subject_code; // 학수번호 저장
			student_sub[pCnt][sCnt].student_num = 1; // 해당 강의를 듣고 있는 학생을 1로 초기화
			int partCnt = 0; // 강의시간표가 분할된 개수
			for (int i = 0; i < 3; i++)
				if (!(time[i] == "")) // 강의시간표가 존재한다면 시간표 분할 개수 증가
					partCnt++;
			student_sub[pCnt][sCnt].schedule_num = partCnt; // 강의시간 분할 개수 저장
			student_sub[pCnt][sCnt].time_table = new Schedule[partCnt]; // 강의시간 분할 수 만큼 Schedule 배열 생성
			student_sub[pCnt][sCnt].next = NULL;

			// 학생별 schedule 초기화 진행
			for (int tidx = 0; tidx < partCnt; tidx++) { // 강의시간 분할 수 만큼 진행
				int split = time[tidx].find('-'); // 강의의 시작교시와 종료교시를 분할할 인덱스
				string date = time[tidx].substr(0, 3); // 요일을 저장
				string startT_str = time[tidx].substr(3, 3); // 강의의 시작 시간을 저장
				float startT = stof(startT_str); startT = (startT - 1) * 2; // 문자열로 표현된 강의의 시작 시간을 실수형으로 변환하여 저장
				string endT_str = time[tidx].substr(split + 1, 3); // 강의 종료 시간을 저장
				float endT = stof(endT_str); endT = (endT - 1) * 2;  // 문자열로 표현된 강의의 종료 시간을 실수형으로 변환하여 저장

				if (date == "mon") student_sub[pCnt][sCnt].time_table[tidx].day = mon; // 강의 요일에 맞게 열거형 변수 day에 저장
				else if (date == "tue") student_sub[pCnt][sCnt].time_table[tidx].day = tue;
				else if (date == "wed") student_sub[pCnt][sCnt].time_table[tidx].day = wed;
				else if (date == "thu") student_sub[pCnt][sCnt].time_table[tidx].day = thu;
				else if (date == "fri") student_sub[pCnt][sCnt].time_table[tidx].day = fri;
				student_sub[pCnt][sCnt].time_table[tidx].start_time = (int)startT; // 실수형으로 표현된 시작 시간을 정수형으로 변환
				student_sub[pCnt][sCnt].time_table[tidx].end_time = (int)endT; // 실수형으로 표현된 종료 시간을 정수형으로 변환
			}
		}
	}
}

class Q3 {
	Subject **examSchedule; // [0][0]이면 월요일 0(1교시), [3][5]이면 목요일 5(3.5교시), 한 학생의 개인 시간표를 저장할 이중 포인터
	Subject **resultExamSchedule; // examSchedule와 동일, 모든 학생의 시험 시간표를 종합할 이중 포인터

public:
	Q3() {
		this->examSchedule = new Subject*[max_day]; // 5개의 요일 배열 생성(월, 화, 수, 목, 금)
		this->resultExamSchedule = new Subject*[max_day];
		for (int i = 0; i < 5; i++) {
			this->examSchedule[i] = new Subject[max_time]{ {"", NULL, 0, 0, NULL} }; // 각각의 요일에 1~15.5교시까지의 배열 생성(시작시간이 겹치는 강의는 연결 리스트로 연결)
			this->resultExamSchedule[i] = new Subject[max_time]{ {"", NULL, 0, 0, NULL} }; // examSchedule와 동일
		}
	}

	Subject* timeSort(Subject* a, int size) { // 오름차순 정렬
		Subject *dest= new Subject[size];
		for (int i = 0; i < size; i++){		// 원본 subject배열은 그대로 유지하기 위해 새로운 배열을 생성해서 내부 값들 복사.
			dest[i].subject_name = a[i].subject_name; // 강의 학수번호
			dest[i].time_table = a[i].time_table;	  // 요일별 강의시간
			dest[i].schedule_num = a[i].schedule_num; // 강의시간 분할 수
			dest[i].student_num = a[i].student_num;	  // 해당 강의를 듣고 있는 학생의 수, 가중치로 사용
			dest[i].next = a[i].next;
		}

		Subject temp;
        for (int i = 1; i < size; i++) {
            temp = dest[i];
            int j = i - 1;
            while (j >= 0 && ++compare_cnt && temp.time_table[0].start_time < dest[j].time_table[0].start_time) {
                dest[j + 1] = dest[j];
                j--;
            }
            dest[j + 1] = temp;
        }
		return dest;
    }

    Subject* daySort(Subject* a, int size) { // 오름차순 정렬
        Subject temp;
        for (int i = 1; i < size; i++) {
            temp = a[i];
            int j = i - 1;
            while (j >= 0 && ++compare_cnt && temp.time_table[0].day < a[j].time_table[0].day) {
                a[j + 1] = a[j];
                j--;
            }
            a[j + 1] = temp;
        }
		return a;
    }

	Subject* scheduleSort(Subject* a, int size) { // 오름차순 정렬
        Subject temp;
        for (int i = 1; i < size; i++) {
            temp = a[i];
            int j = i - 1;
            while (j >= 0 && ++compare_cnt && temp.schedule_num < a[j].schedule_num) {
                a[j + 1] = a[j];
                j--;
            }
            a[j + 1] = temp;
        }
		return a;
    }

	// 한 학생의 수강강의들이 저장된 배열 subject, 강의 요일, 강의 시작교시, 강의 시간 분할 수에 따른 기수정렬, 반환값은 정렬된 배열
	Subject* Sort(Subject *subject, int size) {
		Subject* dest=timeSort(subject, size);
		daySort(dest, size);
		scheduleSort(dest, size);
		// 학생별로 과목 정렬. 과목 정렬 우선순위는  1순위 적은 강의 시간 분할 수, 2순위 같은 분할 수라면 빠른 요일별, 3순위 같은 요일이라면 빠른 시간대
		return dest;
	}

	// 강의 요일 중 가장 시험이 덜 배정되어있는(가중치가 가장 작은) 요일에 해당하는 time_table 배열의 인덱스를 반환하는 함수
	// subjectSchedule는 특정 강의의 시간표 배열, examTable는 전체시간표에 해당하는 2차원 배열, userSubject는 특정 강의
	int minExamDay(Schedule *subjectSchedule, Subject **examTable, Subject* userSubject) {
		// min은 시험이 가장 덜 배정되어있는 요일(0번째 시간표의 요일을 기본적인 최소로 설정), minSum은 min요일에 해당하는 가중치, sum은 minSum과 비교할 다음 요일의 가중치
		int i, j, min = 0, minSum = INT_MAX, sum = 0;
		int scheduleNum = userSubject->schedule_num; // 강의시간표의 분할 개수를 저장
		int startTime, endTime; // 강의의 시작 시간과 종료 시간을 저장할 변수
		Subject *ptr; // 요일에 특정 교시를 가리킬 포인터

		for (i = 0; i < scheduleNum; i++) { // 강의 시간표의 요일들을 검토하여 minSum보다 작은 요일이 발견될경우 해당 시간표를 min으로 할당
			startTime = userSubject->time_table[i].start_time, endTime = userSubject->time_table[i].end_time; // 시작교시와 종료교시를 i번째 시간표의 시작교시와 종료교시로 초기화
			for (j = 0; j < max_time; j++) {
				ptr = &examTable[subjectSchedule[i].day][j]; // i번째 시간표 요일의 1교시부터 15.5교시까지 시험이 배정되어있는지 확인
				if (ptr->subject_name != "") { // 해당 요일의 교시에 시험이 배정되어있다면
					while (ptr != NULL && ptr->subject_name != "") { // 시험 연결리스트의 끝까지 확인
						if (ptr->subject_name.substr(0, 7) != userSubject->subject_name.substr(0, 7) && ++compare_cnt && // 같은 강의의 분반이 아니거나 시험 시간이 겹치지 않는다면 가중치 증가
							!((ptr->time_table->start_time <= startTime && ptr->time_table->end_time > startTime) || (
								ptr->time_table->start_time < endTime && ptr->time_table->end_time >= endTime)))
							sum += ptr->student_num;
						ptr = ptr->next;
					}
				}
			}
			if (minSum > sum) { // minSum을 INT_MAX로 초기화함으로써 0번째 시간표는 반드시 가중치합이 최소인 시간표로 설정, 그 다음 시간표부터는 0번째 시간표와 비교
				minSum = sum;
				min = i;
			}
			sum = 0;
		}

		return min; // 시험 최소 배정 요일에 해당하는 Schedule 배열의 인덱스 반환
	}

	// minExamDay를 토대로 한 사람의 시험시간표를 배정하는 함수, subjectList는 한 학생의 강의 배열, size는 강의 개수
	void allocateExamSchedule(Subject* subjectList, int size) { 
		Schedule *subject_schedule; // 특정 강의의 시간표를 가리킬 포인터
		Subject *ptr; // 특정 강의를 가리킬 포인터

		for (int i = 0; i < size; i++) {  // 학생의 모든 수강강의들에 대해 진행
			subject_schedule = subjectList[i].time_table; // subject_schedule이 i번째 강의의 시간표 배열을 가리킴
			int examDay = minExamDay(subject_schedule, this->examSchedule, &subjectList[i]); // 가중치가 최소인 요일 저장
			ptr = &(this->examSchedule[subjectList[i].time_table[examDay].day][subjectList[i].time_table[examDay].start_time]); // ptr이 강의의 최소 가중치 시간표를 가리킴
			*ptr = subjectList[i]; // 시험 배정
			ptr->time_table = & subjectList[i].time_table[examDay]; // 해당 시험의 시간표 배열에 examDay에 해당하는 시간표만 저장(후에 시간표 배열을 확인하는 과정 생략을 위함)
			subjectList[i].time_table = ptr->time_table; // 원본 배열의 시간표 배열도 시험시간에 해당하는 시간표로 변경(후에 시간표 배열을 확인하는 과정 생략을 위함)
		}
	}

	// minExamDay를 토대로 한 사람의 시험 시간표를 전체 시험 시간표에 배정하는 함수
	void allocateResultExamSchedule(Subject* userSubject, int size) { 
		Subject *ptr; // 특정 강의를 가리킬 포인터

		for (int i = 0; i < size; i++) {
			ptr = &(this->resultExamSchedule[userSubject[i].time_table->day][userSubject[i].time_table->start_time]);
			if (ptr!= NULL && ptr->subject_name != "") { // 이미 배정되어 있는 시간표가 있다면
				while (++compare_cnt && ptr->next != NULL) {
					if (ptr->subject_name == userSubject[i].subject_name) { // 지금 강의의 이름과 같다면 가중치 증가
						ptr->student_num++;
						break;
					}
					ptr = ptr->next;
				}
				if (ptr->subject_name == userSubject[i].subject_name) { // 지금 강의의 이름과 같다면 가중치 증가
					ptr->student_num++;
				}
				else { // 지금 강의의 이름과 같지 않다면 전체 시험 시간표에 한 사람의 시험 시간표 배정
					ptr->next = new Subject;
					*ptr->next = userSubject[i];
				}
			}
			else
				*ptr = userSubject[i]; // 이미 배정되어 있는 시간표가 없다면 바로 시간표 배정
		}
	}

	// 전체 시험 시간표에서 중복되는 강의의 시험 중 가중치가 높은 시험만 놔두고 나머지는 삭제하는 함수
	void deleteExamResult(Subject* sub, int size) { 
		int max, diff_flag, sole_flag, j; // max는 같은 강의의 시험들 중 가장 가중치가 높은 시험의 시간표의 인덱스를 저장할 변수, diff_flag는 가중치가 같은지 판단하는 플래그 변수(1이면 다른 가중치)
		// sole_flag는 특정 강의의 시험이 여러개로 분할 배정된 것이 아닌 한 곳에 배정된 것을 판단하는 플래그 변수(1일 경우 시험 시간표가 단 하나)
		Subject *ptr, *prev; // ptr은 특정 강의를 가리킬 포인터, prev는 ptr의 앞을 가리키는 포인터(prev->next == ptr)
		for (int i = 0; i < size; i++) { // 한 학생의 수강 강의의 개수만큼 진행
			max = 0; // 첫번째 강의 시험을 최대 가중치로 초기화
			sole_flag = 1; // 시험 시간표가 단 하나라는 가정으로 초기화(같은 강의의 시험 시간표가 여러개일 경우 sole_flag가 0으로 초기화)
			for (j = 0; j < sub[i].schedule_num; j++) { // 특정 강의의 시간표 만큼 진행
				ptr = &resultExamSchedule[sub[i].time_table[j].day][sub[i].time_table[j].start_time]; // 특정 강의에 해당하는 시간표를 가리킴
				while (ptr != NULL && ptr->subject_name != "") { // 특정 강의에 해당하는 학수번호를 찾을때까지 진행
					++compare_cnt;
					if (ptr->subject_name == sub[i].subject_name) // 찾으면 종료
						break;
					ptr = ptr->next;
				}
				if (ptr != NULL) // 특정 강의를 찾으면 종료(같은 강의의 시험 시간표 중 가장 빠른 시간표를 찾은것)
					break;
			}

			max = j, diff_flag = 0, ++j; // 최대 가중치 시간표를 위 과정에서 진행한 가장 빠른 시험 시간표로 설정, j는 다음 시간표를 가리키도록 1 증가
			for (j ; j < sub[i].schedule_num; j++) { // 다음 시간표부터 가중치 비교
				ptr = &resultExamSchedule[sub[i].time_table[j].day][sub[i].time_table[j].start_time]; // 다음 시험 시간표를 가리킴
				while (++compare_cnt && ptr != NULL && ptr->subject_name != "") { // 특정 강의를 찾을때까지 진행
					if (ptr->subject_name == sub[i].subject_name) { // 찾을 경우 다른 시험 시간표가 존재하므로 sole_flag를 0으로 설정
						sole_flag = 0;
						break;
					}
					ptr = ptr->next;
				}
				if (ptr != NULL && ptr->subject_name == sub[i].subject_name) { // 만약 다음 시험 시간표를  찾을 경우
					if (ptr->student_num != resultExamSchedule[sub[i].time_table[max].day][sub[i].time_table[max].start_time].student_num)
						diff_flag = 1; // 이전에 존재하던 시험 시간표랑 가중치가 다를 경우 diff_flag를 1로 설정
					// 가중치가 다르다면 더 큰 가중치의 시간표를 max에 저장
					if (ptr->student_num > resultExamSchedule[sub[i].time_table[max].day][sub[i].time_table[max].start_time].student_num) 
						max = j; // 
				}
			}
			if (sole_flag == 1) { // 시험 시간표가 단 하나일 경우 그대로 놔두고 다음 강의 시험 시간표에 대해 진행
				continue;
			}
			if (diff_flag == 0) { // 가중치가 동일하다면 해당 시험들이 배정된 요일 중 가중치가 가장 적은 요일의 시험 시간표를 선택
				max = minExamDay(sub[i].time_table, this->resultExamSchedule, &sub[i]);
			}
			for (int j = 0; j < sub[i].schedule_num; j++) { 
				if (j != max) { // 최대 가중치의 시간표, 혹은 같은 가중치이지만 요일의 가중치가 가장 적은 시간표가 아닌 나머지 시간표는 삭제
					ptr = &resultExamSchedule[sub[i].time_table[j].day][sub[i].time_table[j].start_time];
					prev = ptr;
					while (ptr != NULL && ptr->subject_name != "") {
						++compare_cnt;
						if (ptr->subject_name == sub[i].subject_name) {
							if (ptr->next == NULL) // 같은 강의의 다음 연결리스트가 없다면 바로 삭제
								*ptr = { "", NULL, 0, 0, NULL };
							else { // 다음 연결리스트가 존재한다면 이전 연결리스트와 연결
								if (ptr == &resultExamSchedule[sub[i].time_table[j].day][sub[i].time_table[j].start_time]) {
									*ptr = *ptr->next;
								}
								else {
									prev->next = prev->next->next;
									*ptr = { "", NULL, 0, 0, NULL };
									break;
								}
							}
						}
						prev = ptr;
						ptr = ptr->next;
					}
				}
			}
		}
	}

	// 전체 학생의 종합된 시험 시간표를 출력하는 함수
	void printExamResultSchedule() {
		Subject* ptr; // 특정 강의를 가리킬 포인터
		for (int i = 0; i < 5; i++) {
			switch (i) {
			case 0:
				cout << "월 : ";
				break;
			case 1:
				cout << "화 : ";
				break;
			case 2:
				cout << "수 : ";
				break;
			case 3:
				cout << "목 : ";
				break;
			case 4:
				cout << "금 : ";
				break;
			}
			for (int j = 0; j < max_time; j++) {
				if (resultExamSchedule[i][j].subject_name != "") {
					ptr = &resultExamSchedule[i][j];
					cout << ptr->subject_name << " " << (float)(ptr->time_table->start_time*0.5) + 1 << "-" << (float)(ptr->time_table->end_time*0.5) + 1 << "/ ";
					while (ptr->next != NULL && ptr->next->subject_name != "") {
						ptr = ptr->next;
						cout << ptr->subject_name << " " << (float)(ptr->time_table->start_time*0.5) + 1 << "-" << (float)(ptr->time_table->end_time*0.5) + 1 << "/ ";
					}
				}
			}
			cout << endl;
		}
		cout << "Q3 비교횟수 : " << compare_cnt << endl;
	}

	// 전체 학생의 강의정보를 받아서, 최대한 많은 학생이 시험이 겹치는 경우를 피하게 만드는 알고리즘
	void examAllocate(Subject** subjectList, int student_num) { 
		Subject *subject;
		for (int i = 0; i < student_num; i++) { // 학생의 수만큼 전체 시험 시간표 배열에 시험 배정
			subject = subjectList[i];
			subject = Sort(subject, student_subjectNum[i]);
			allocateExamSchedule(subject, student_subjectNum[i]);
			allocateResultExamSchedule(subject, student_subjectNum[i]);
			delete this->examSchedule;
			this->examSchedule = new Subject*[5]; // 5개의 요일 배열 생성(월, 화, 수, 목, 금)
			for (int j = 0; j < 5; j++) {
				this->examSchedule[j] = new Subject[max_time]{ {"", NULL, 0, 0, NULL} }; // 각각의 요일에 1~15.5교시까지의 배열 생성(시간이 겹치는 강의는 연결 리스트로 연결)
			}
		}
		for (int i = 0; i < student_num; i++) { // 학생의 수만큼 강의배열들을 받아서 중복되는 시험 삭제
			deleteExamResult(subjectList[i], student_subjectNum[i]);
		}
		printExamResultSchedule();
	}
};

int main() {
	setData();
	Q3 q3;
	q3.examAllocate(student_sub ,student_cnt);
}