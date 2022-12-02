#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

#define max_snum 3 // �Է����� �� �л� ��
#define max_subject 24 // ���� ������ ������ ��
#define max_time 30 // ���� ���� ��(n = (1 + 0.5*n)����) (n >= 0�� 30���� ���� ����)
#define max_day 5
#define max_scheduleNum 3 // ������ ���� �ð� ���� ��(�Ϲݹ����й׽���1�� ��� ���ǰ� 3���� �ð����� ���ҵ�)

enum Day {mon, tue, wed, thu, fri}; // ���Ͽ� ���� ���� ������ ������

typedef struct schedule{
	enum Day day; // ����
	int start_time; // �ش� ���Ͽ��� ���� ���۱���
	int end_time; // �ش� ���Ͽ��� ���� ���ᱳ��
} Schedule; // ���Ͽ� ���� ���� �ð� ������ ��� �ִ� ����ü

typedef struct subject {
	string subject_name; // ���� �м���ȣ
	Schedule *time_table; // ���Ϻ� ���ǽð�
	int schedule_num; // ���ǽð� ���� ��
	int student_num; // �ش� ���Ǹ� ��� �ִ� �л��� ��
	struct subject* next;
} Subject; // ������ ������ ��� ����ü

string **student; // �л��� �������� �м���ȣ�� ��� ����������(ex : 1�� �л��� CSE2022-01, EGC2022-02... �� ��� �ִٸ� student[1][0] = "CSE2022-01" student[1][1] = "EGC2022-02")

int student_cnt; // �����ͷ� �޾ƿ� �л� ��
int *student_subjectNum; // �л��� �������� ���� ������ ������
Subject **student_sub; // �л����� �м���ȣ�� ���� Subject ����ü�� ��ȯ

void setData() {
	ifstream fp("totalData.txt");
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
	student = new string*[student_cnt]; // �л�����ŭ�� ������ �迭 �Ҵ�
	student_sub = new Subject*[student_cnt]; // �л�����ŭ�� ������ �迭 ũ�� �Ҵ�
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
			student_sub[pCnt][sCnt].next = NULL;

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

class Q1 {
	Subject **subject; // [0][0]�̸� ������ 0(1����), [3][5]�̸� ����� 5(3.5����)
	Subject **resultSubject;
	int size; // ���������� ������ ������ ����

public:
	Q1() {
		this->subject = new Subject*[5]; // 5���� ���� �迭 ����(��, ȭ, ��, ��, ��)
		this->resultSubject = new Subject*[5];
		for (int i = 0; i < 5; i++) {
			this->subject[i] = new Subject[max_time]; // ������ ���Ͽ� 1~15.5���ñ����� �迭 ����(�ð��� ��ġ�� ���Ǵ� ���� ����Ʈ�� ����)
			this->resultSubject[i] = new Subject[max_time];
		}
	}

	void setSize(int size) {
		this->size = size;
	}

	Subject* timeSort(Subject* src, int n, int k) { // ���� ���۽ð� ���� �������, src�� ������ �迭, dest�� ������ �Ϸ�� �迭, n�� src ������ ����, k�� ������ ����
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

	Subject* daySort(Subject* src, int n, int k) { // ���� ���� ���� �������, src�� ������ �迭, dest�� ������ �Ϸ�� �迭, n�� src ������ ����, k�� ������ ����
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

	Subject* scheduleNumSort(Subject* src, int n, int k) { // ���� ���� �ð� ���� �� ���� �������, src�� ������ �迭, dest�� ������ �Ϸ�� �迭, n�� src ������ ����, k�� ���� ������ ���� ����
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

	Subject* radixSort(Subject *subject) { // ����, ���� �ð�, ���� �ð� ���� ���� ���� �������
		subject = timeSort(subject, size, max_time);
		subject = daySort(subject, size, max_day);
		subject = scheduleNumSort(subject, size, max_scheduleNum);
		// �л����� ���� ����. ���� ���� ������  1��° ���� ���� �ð� ���� ��, 2��° ���� ���� ����� ���� ���Ϻ�, 3��° ���� �����̶�� ���� �ð���

		return subject;
	}

	int minExamDay(Schedule *sc, Subject **sb, Subject* userSubject) { // ���� ���� �� ���� ������ �� �����Ǿ��ִ�(����ġ�� ���� ����) ���Ͽ� �ش��ϴ� Schedule �迭�� �ε����� ��ȯ�ϴ� �Լ�
		int i, j, min = 0, minSum = 0, sum = 0; // min�� ������ ���� �� �����Ǿ��ִ� ����(1��° ������ �⺻���� �ּҷ� ����), minSum�� min���Ͽ� �ش��ϴ� ����ġ
		int scheduleNum = userSubject->schedule_num;
		int userStartTime, userEndTime;
		Subject *ptr; // ���Ͽ� ������ ������ ����ų ������
		userStartTime = userSubject->time_table[0].start_time, userEndTime = userSubject->time_table[0].end_time;
		for (i = 0; i < max_time; i++) { // 1���ú��� 15.5���ñ��� ��� ������� ����ġ�� ����
			if (sb[sc[0].day][i].subject_name != "") { // �ش� ���ÿ� ������ �����Ǿ��ִٸ�
				ptr = &sb[sc[0].day][i]; // ���Ḯ��Ʈ ��ü Ȯ��
				while (ptr != NULL) {
					if ((ptr->subject_name.substr(0, 7) != userSubject->subject_name.substr(0, 7)) &&
						!((ptr->time_table->start_time <= userStartTime && ptr->time_table->end_time > userStartTime) || (
							ptr->time_table->start_time < userEndTime && ptr->time_table->end_time >= userEndTime))) // ���࿡ ���� �������� �й��� �ƴϰų� �ð��밡 ��ġ�� ���ǰ� �ƴ϶��
						minSum += ptr->student_num;
					ptr = ptr->next; // �ð��� �ߺ��Ǵ� ������ �����ϴ��� �Ʒ� ���ǹ��� ���� Ȯ��
				}
			}
		}

		for (i = 1; i < scheduleNum; i++) { // ������ ���ϵ��� �����Ͽ� minSum���� ���� ������ �߰ߵɰ�� �ش� ������ min���� �Ҵ�
			userStartTime = userSubject->time_table[i].start_time, userEndTime = userSubject->time_table[i].end_time;
			for (j = 0; j < max_time; j++) {
				if (sb[sc[i].day][j].subject_name != "") {
					ptr = &sb[sc[i].day][j];
					while (ptr != NULL) {
						if (ptr->subject_name.substr(0, 7) != userSubject->subject_name.substr(0, 7) &&
							!((ptr->time_table->start_time <= userStartTime && ptr->time_table->end_time > userStartTime) || (
								ptr->time_table->start_time < userEndTime && ptr->time_table->end_time >= userEndTime))) // ���࿡ ���� �������� �й��� �ƴϰų� �ð��밡 ��ġ�� ���ǰ� �ƴ϶��
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

		return min; // ���� �ּ� ���� ���Ͽ� �ش��ϴ� Schedule �迭�� �ε��� ��ȯ
	}

	void allocateExam(Subject* userSubject, int size) { // minExamDay�� ���� �� ����� ������ �����ϴ� �Լ�
		Schedule *sc;
		Subject *ptr;

		for (int i = 0; i < size; i++) {  // �л��� ��� �������ǵ鿡 ���� ����
			sc = userSubject[i].time_table;
			int examDay = minExamDay(sc, this->subject, &userSubject[i]); // ����ġ�� �ּ��� ���� Ȯ��
			ptr = &(this->subject[userSubject[i].time_table[examDay].day][userSubject[i].time_table[examDay].start_time]);
			if (ptr->subject_name == "") { // �ش� �ð��뿡 ������ �������� �ʴ´ٸ�
				*ptr = userSubject[i]; // ���� ����
				ptr->time_table = & userSubject[i].time_table[examDay];
			}
			else { // �ٸ� ������ �����Ѵٸ� ���Ḯ��Ʈ�� ����
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

	void allocateExamResult(Subject* userSubject, int size) { // minExamDay�� ���� �� ����� ������ �����ϴ� �Լ�
		Subject *ptr;
		for (int i = 0; i < size; i++) {
			ptr = &(this->resultSubject[userSubject[i].time_table->day][userSubject[i].time_table->start_time]);
			if (ptr->subject_name != "") { // �̹� �����Ǿ� �ִ� �ð�ǥ�� �ִٸ�
				while (ptr->next != NULL) {
					if (ptr->subject_name == userSubject[i].subject_name) { // ���� ������ �̸��� ���ٸ� ����ġ ����
						ptr->student_num++;
						break;
					}
					ptr = ptr->next;
				}
				if (ptr->subject_name == userSubject[i].subject_name) { // ���� ������ �̸��� ���ٸ� ����ġ ����
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
			cout << "��ü ��� �Ҵ� ��\n";
			printExamSchedule();
			allocateExamResult(subject, size);
			cout << "��ü ��� �Ҵ� ��\n";
			printExamResultSchedule();
			delete this->subject;
			this->subject = new Subject*[5]; // 5���� ���� �迭 ����(��, ȭ, ��, ��, ��)
			for (int i = 0; i < 5; i++) {
				this->subject[i] = new Subject[max_time]; // ������ ���Ͽ� 1~15.5���ñ����� �迭 ����(�ð��� ��ġ�� ���Ǵ� ���� ����Ʈ�� ����)
			}
		}
		for (int i = 0; i < student_num; i++) {
			setSize(student_subjectNum[i]);
			deleteExamResult(subjectList[i], size);
			cout << "���� ��\n";
			printExamResultSchedule();
		}
	}
};

int main() {
	setData();
	Q1 q1;
	q1.examAllocate(student_sub ,student_cnt);
}