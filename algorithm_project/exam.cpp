#include <string>
#include <iostream>

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

class subject_table // ������ �м���ȣ�� key, Subject�� value�� ������ �ؽ� ���̺�
{
	Subject **table;
	int size;

public:
	subject_table(int n)
	{
		size = n;
		table = new Subject* [size]{NULL};
	}

	//�ؽð��� ��ȯ�� �Լ�
	int hash_value(string subject_name)
	{
		int hash_value = 0, a = 33;

		for (int i = 0; i < subject_name.length(); i++)
		{
			hash_value += hash_value * a + subject_name[i];
		}

		return hash_value % size;
	}

	// table�� operation�� �ش��ϴ� code�� ������ �Լ�
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

	// table���� operation�� �ش��ϴ� code�� �������� �Լ�
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
	Subject **subject; // [0][0]�̸� ������ 0(1����), [3][5]�̸� ����� 5(3.5����)
	int size; // ���������� ������ ������ ����

public:
	Q1(int size) {
		this->size = size;
		this->subject = new Subject*[5]; // 5���� ���� �迭 ����(��, ȭ, ��, ��, ��)
		for (int i = 0; i < 5; i++) {
			this->subject[i] = new Subject[max_time]; // ������ ���Ͽ� 1~15.5���ñ����� �迭 ����(�ð��� ��ġ�� ���Ǵ� ���� ����Ʈ�� ����)
		}
	}

	Subject* timeSort(Subject* src, int n, int k) { // ���� ���۽ð� ���� �������, src�� ������ �迭, dest�� ������ �Ϸ�� �迭, n�� src ������ ����, k�� ������ ����
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

	Subject* daySort(Subject* src, int n, int k) { // ���� ���� ���� �������, src�� ������ �迭, dest�� ������ �Ϸ�� �迭, n�� src ������ ����, k�� ������ ����
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

	Subject* scheduleNumSort(Subject* src, int n, int k) { // ���� ���� �ð� ���� �� ���� �������, src�� ������ �迭, dest�� ������ �Ϸ�� �迭, n�� src ������ ����, k�� ���� ������ ���� ����
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

	Subject* radixSort(Subject *subject) { // ����, ���� �ð�, ���� �ð� ���� ���� ���� �������
		subject = timeSort(subject, size, max_time);
		subject = daySort(subject, size, max_day);
		subject = scheduleNumSort(subject, size, max_scheduleNum);
		// �л����� ���� ����. ���� ���� ������  1��° ���� ���� �ð� ���� ��, 2��° ���� ���� ����� ���� ���Ϻ�, 3��° ���� �����̶�� ���� �ð���

		return subject;
	}

	int minExamDay(Schedule *sc, Subject **sb, int scheduleNum) { // ���� ���� �� ���� ������ �� �����Ǿ��ִ�(����ġ�� ���� ����) ���Ͽ� �ش��ϴ� Schedule �迭�� �ε����� ��ȯ�ϴ� �Լ�
		int i, j, min = 0, minSum = 0, sum = 0; // min�� ������ ���� �� �����Ǿ��ִ� ����(1��° ������ �⺻���� �ּҷ� ����), minSum�� min���Ͽ� �ش��ϴ� ����ġ
		Subject *ptr; // ���Ͽ� ������ ������ ����ų ������
		for (i = 0; i < max_time; i++) { // 1���ú��� 15.5���ñ��� ��� ������� ����ġ�� ����
			if (sb[sc[0].day][i].subject_name != "") { // �ش� ���ÿ� ������ �����Ǿ��ִٸ�
				ptr = &sb[sc[0].day][i];
				minSum += ptr->student_num;
				while(ptr->next != NULL) {
					ptr = ptr->next; // �ð��� �ߺ��Ǵ� ������ �����ϴ��� �Ʒ� ���ǹ��� ���� Ȯ��
					minSum += ptr->student_num; // ����ġ �߰�
				}
			}
		}

		for (i = 1; i < scheduleNum; i++) { // ������ ���ϵ��� �����Ͽ� minSum���� ���� ������ �߰ߵɰ�� �ش� ������ min���� �Ҵ�
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

		return min; // ���� �ּ� ���� ���Ͽ� �ش��ϴ� Schedule �迭�� �ε��� ��ȯ
	}

	void allocateExam(Subject* userSubject, int size) { // minExamDay�� ���� ������ �����ϴ� �Լ�
		Schedule *sc;
		Subject *ptr;

		for (int i = 0; i < size; i++) {  // �л��� ��� �������ǵ鿡 ���� ����
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

// n�� �л����� ���� ��� ������� ����(ex : 1�� �л��� CSE2022-01, EGC2022-02 2�� CSE2022-01, EGC2021-01�� ��´ٸ� ������ CSE2022-01, EGC2022-02, EGC2021-01�� �� 3��)

int main() {
	Q1 q1(6);
	q1.what(NULL);
}