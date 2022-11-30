#include <string>
#include <iostream>

#define max_snum 3

using namespace std;

enum Day {mon, tue, wed, thu, fri}; // ���Ͽ� ���� ���� ������ ������

typedef struct schedule{
	enum Day day; // ����
	int start_time; // �ش� ���Ͽ��� ���� ���۽ð�
	int end_time; // �ش� ���Ͽ��� ���� ����ð�
} Schedule; // ���Ͽ� ���� ���� �ð� ������ ��� �ִ� ����ü

typedef struct subject {
	string subject_name; // ���� �м���ȣ
	Schedule *time_table; // ���Ϻ� ���ǽð�
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
	string **stud;

public:
	Q1(string ** student) {
		this->stud = student;
	}

	void allocateExam(string *subject) {
		// ���ĵ� �ð�ǥ�� �������� ���� ����
	}

	void sortSchedule(string *subject) {
		int i, j, value;
		for (i = 0; subject[i] != ""; i++) {

		}
		// �л����� ���� ���� -> ���� ���� ������ 1��° ���Ϻ�, 2��° ���� �����̶�� �����ð���
	}

	void what(Subject** subjectList ) {
		// �л����� ���� sortSchedule ȣ��
		// �л����� allocateExam�� ���� ������ �ð�ǥ ����
		// �л����� �ð�ǥ ����
		// ���յ� �ð�ǥ �� ����ġ�� �����ͺ��� ����, ���� ����ġ��� ���� ����, ���� �ð������ ����
	}
};

// n�� �л����� ���� ��� ������� ����(ex : 1�� �л��� CSE2022-01, EGC2022-02 2�� CSE2022-01, EGC2021-01�� ��´ٸ� ������ CSE2022-01, EGC2022-02, EGC2021-01�� �� 3��)

int main() {

}