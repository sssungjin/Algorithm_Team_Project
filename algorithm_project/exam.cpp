#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

#define max_subject 24 // ���� ������ ������ ��, �� 24�������� ��������(1���� ���� 24���� �ִ�)
#define max_time 30 // ���� ���� ��(�迭�� �ε��� n = (1 + 0.5*n)����) (0 <= n < 30�� ����)
#define max_day 5 // ������ ����(��~��)
#define max_scheduleNum 3 // ������ ���� �ð� ���� ��(�Ϲݹ����й׽���1�� ��� ���ǰ� 3���� �ð����� ���ҵ�)

enum Day {mon, tue, wed, thu, fri}; // ���Ͽ� ���� ������ ������ ������

typedef struct schedule{
	enum Day day; // ����
	int start_time; // �ش� ���Ͽ��� ���� ���۱���
	int end_time; // �ش� ���Ͽ��� ���� ���ᱳ��
} Schedule; // ���Ͽ� ���� ���� �ð� ������ ��� �ִ� ����ü

typedef struct subject {
	string subject_name; // ���� �м���ȣ
	Schedule *time_table; // ���Ϻ� ���ǽð�
	int schedule_num; // ���ǽð� ���� ��
	int student_num; // �ش� ���Ǹ� ��� �ִ� �л��� ��, ����ġ�� ���
	struct subject* next; // ���� �ð��� ��ġ�� ���Ǹ� ���� ���Ḯ��Ʈ �����ͷ� ���
} Subject; // ������ ������ ��� ����ü

int student_cnt; // �����ͷ� �޾ƿ� �л��� ���� ������ ����
int *student_subjectNum; // �л��� ���������� ���� ������ ������
Subject **student_sub; // �л����� �м���ȣ�� ���� Subject ����ü�� ��ȯ

void setData() { // ���Ϸκ��� ������ ������ �޾� Subject ����ü�� �����ϴ� �Լ�
	ifstream fp("totalData.txt"); // totalData
	if (!fp.is_open()) { cout << "not open" << endl; return; }
	string fpstr; // ���Ϸκ��� �о�� �� ������ ������ ����
	string subject_code, time[max_scheduleNum]; // �м���ȣ�� ������ string, �κк��� ������ �ð�ǥ(�� ����� �ִ� 3������ ������ ���� �����)
	int cnt_idx, cnt_len; // student_cnt�� �������� ���� �ʿ��� ����

	// �л�����ŭ �����Ҵ��ϴ� ����, ������ ù ���忡�� �л����� ���� �ԷµǾ�����
	getline(fp, fpstr);
	cnt_len = fpstr.length(); // ù ������ ���̸� ����
	fpstr = fpstr.substr(12, cnt_len - 12); // �л��� ���� �ش��ϴ� ���ڿ��� ����(totalData.txt�� student_cnt:~~ ���忡�� ~~�� �����ϴ� ����)
	student_cnt = stoi(fpstr); // �о�� �л��� �� ���ڿ��� ���ڷ� ��ȯ
	student_sub = new Subject*[student_cnt]; // �л�����ŭ�� Subject ����ü ������ �迭 ũ�� �Ҵ�
	student_subjectNum = new int[student_cnt]; // �л�����ŭ �� �л��� �������Ǽ��� ������ ������ ������ �迭 ũ�� �Ҵ�

	// student_sub ������ ������ ������ ���� ����
	for (int pCnt = 0; pCnt < student_cnt; pCnt++) { // �о�� �л����� ����ŭ �ݺ�
		getline(fp, fpstr); // �������� ������ �����ִ� ������ �о��
		int subject_cnt = stoi(fpstr); // �������� ������ ����

		student_sub[pCnt] = new Subject[subject_cnt]; // �������� ������ŭ student_sub�� �迭����
		student_subjectNum[pCnt] = subject_cnt; // �л��� �������� ������ �ش� �ε����� ����

		for (int sCnt = 0; sCnt < subject_cnt; sCnt++) { // �� �л��� �������� ������ŭ �ݺ�
			time[0] = time[1] = time[2] = "";  // �ð�ǥ�� �޾ƿ� �迭 �ʱ�ȭ
			int scheduleNum = 0; // �ð�ǥ�� ���� �� �ʱ�ȭ
			getline(fp, fpstr); // ���� �ϳ��� ������ ���Ϸκ��� �о��
			stringstream split(fpstr);
			split >> subject_code >> time[0] >> time[1] >> time[2]; // ����������� ���� ������ ��� ����

			// �л��� Subject ����ü ��� �ʱ�ȭ ����
			student_sub[pCnt][sCnt].subject_name = subject_code; // �м���ȣ ����
			student_sub[pCnt][sCnt].student_num = 1; // �ش� ���Ǹ� ��� �ִ� �л��� 1�� �ʱ�ȭ
			int partCnt = 0; // ���ǽð�ǥ�� ���ҵ� ����
			for (int i = 0; i < 3; i++)
				if (!(time[i] == "")) // ���ǽð�ǥ�� �����Ѵٸ� �ð�ǥ ���� ���� ����
					partCnt++;
			student_sub[pCnt][sCnt].schedule_num = partCnt; // ���ǽð� ���� ���� ����
			student_sub[pCnt][sCnt].time_table = new Schedule[partCnt]; // ���ǽð� ���� �� ��ŭ Schedule �迭 ����
			student_sub[pCnt][sCnt].next = NULL;

			// �л��� schedule �ʱ�ȭ ����
			for (int tidx = 0; tidx < partCnt; tidx++) { // ���ǽð� ���� �� ��ŭ ����
				int split = time[tidx].find('-'); // ������ ���۱��ÿ� ���ᱳ�ø� ������ �ε���
				string date = time[tidx].substr(0, 3); // ������ ����
				string startT_str = time[tidx].substr(3, 3); // ������ ���� �ð��� ����
				float startT = stof(startT_str); startT = (startT - 1) * 2; // ���ڿ��� ǥ���� ������ ���� �ð��� �Ǽ������� ��ȯ�Ͽ� ����
				string endT_str = time[tidx].substr(split + 1, 3); // ���� ���� �ð��� ����
				float endT = stof(endT_str); endT = (endT - 1) * 2;  // ���ڿ��� ǥ���� ������ ���� �ð��� �Ǽ������� ��ȯ�Ͽ� ����

				if (date == "mon") student_sub[pCnt][sCnt].time_table[tidx].day = mon; // ���� ���Ͽ� �°� ������ ���� day�� ����
				else if (date == "tue") student_sub[pCnt][sCnt].time_table[tidx].day = tue;
				else if (date == "wed") student_sub[pCnt][sCnt].time_table[tidx].day = wed;
				else if (date == "thu") student_sub[pCnt][sCnt].time_table[tidx].day = thu;
				else if (date == "fri") student_sub[pCnt][sCnt].time_table[tidx].day = fri;
				student_sub[pCnt][sCnt].time_table[tidx].start_time = (int)startT; // �Ǽ������� ǥ���� ���� �ð��� ���������� ��ȯ
				student_sub[pCnt][sCnt].time_table[tidx].end_time = (int)endT; // �Ǽ������� ǥ���� ���� �ð��� ���������� ��ȯ
			}
		}
	}
}

class Q1 {
	Subject **examSchedule; // [0][0]�̸� ������ 0(1����), [3][5]�̸� ����� 5(3.5����), �� �л��� ���� �ð�ǥ�� ������ ���� ������
	Subject **resultExamSchedule; // examSchedule�� ����, ��� �л��� ���� �ð�ǥ�� ������ ���� ������
	int size; // ���������� ������ ������ ����

public:
	Q1() {
		this->examSchedule = new Subject*[5]; // 5���� ���� �迭 ����(��, ȭ, ��, ��, ��)
		this->resultExamSchedule = new Subject*[5];
		for (int i = 0; i < 5; i++) {
			this->examSchedule[i] = new Subject[max_time]{ {"", NULL, 0, 0, NULL} }; // ������ ���Ͽ� 1~15.5���ñ����� �迭 ����(���۽ð��� ��ġ�� ���Ǵ� ���� ����Ʈ�� ����)
			this->resultExamSchedule[i] = new Subject[max_time]{ {"", NULL, 0, 0, NULL} }; // examSchedule�� ����
		}
	}

	void setSize(int size) { // ���������� ������ �����Ѵ� �Լ�
		this->size = size;
	}

	Subject* timeSort(Subject* src, int n, int k) { // ���� ���� ���۽ð� ���� �������, src�� ������ �迭, n�� �������� ����, k�� ������ ����(1~15.5���ñ��� �� 30), ��ȯ���� ���ĵ� dest �迭
		Subject *dest = new Subject[size]; // �������� ������ŭ �迭 ����
		int *N = new int[k]{ 0 }; // ������ ������ŭ �迭 ����(���۽ð��� 0���� 29�����̹Ƿ� �迭�� ũ��� k ����)
		int i;
		// ��� ����
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

	Subject* daySort(Subject* src, int n, int k) { // ���� ���� ���� �������, src�� ������ �迭, n�� �������� ����, k�� ������ ����(��~�ݱ��� �� 5), ��ȯ���� ���ĵ� dest �迭
		Subject *dest = new Subject[size]; // �������� ������ŭ �迭 ����
		int *N = new int[k] {0}; // ������ ������ŭ �迭 ����(Day ������ ������ 0���� 4�����̹Ƿ� �迭�� ũ��� k ����)
		int i;
		// ��� ����
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

	Subject* scheduleNumSort(Subject* src, int n, int k) { // ���� ���ǽð� ���Ҽ� ���� �������, src�� ������ �迭, n�� �������� ����, k�� ���� ������ ����(1~3���� �� 3), ��ȯ���� ���ĵ� dest �迭
		Subject *dest = new Subject[size]; // �������� ������ŭ �迭 ����
		int *N = new int[k + 1]{ 0 }; // ���� ������ŭ �迭 ����(���Ҽ��� 1���� 3�����̹Ƿ� �迭�� ũ��� k+1 ����, 0��° �ε����� ���x)
		int i;
		// ��� ����
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

	Subject* radixSort(Subject *subject) { // �� �л��� �������ǵ��� ����� �迭 subject, ���� ����, ���� ���۱���, ���� �ð� ���� ���� ���� �������, ��ȯ���� ���ĵ� �迭
		subject = timeSort(subject, size, max_time);
		subject = daySort(subject, size, max_day);
		subject = scheduleNumSort(subject, size, max_scheduleNum);
		// �л����� ���� ����. ���� ���� �켱������  1���� ���� ���� �ð� ���� ��, 2���� ���� ���� ����� ���� ���Ϻ�, 3���� ���� �����̶�� ���� �ð���

		return subject;
	}

	int minExamDay(Schedule *subjectSchedule, Subject **examTable, Subject* userSubject) { // ���� ���� �� ���� ������ �� �����Ǿ��ִ�(����ġ�� ���� ����) ���Ͽ� �ش��ϴ� time_table �迭�� �ε����� ��ȯ�ϴ� �Լ�
		// sc�� Ư�� ������ �ð�ǥ �迭, sb�� ��ü�ð�ǥ�� �ش��ϴ� 2���� �迭, userSubject�� Ư�� ����
		int i, j, min = 0, minSum = INT_MAX, sum = 0; // min�� ������ ���� �� �����Ǿ��ִ� ����(0��° �ð�ǥ�� ������ �⺻���� �ּҷ� ����), minSum�� min���Ͽ� �ش��ϴ� ����ġ, sum�� minSum�� ���� ���� ������ ����ġ
		int scheduleNum = userSubject->schedule_num; // ���ǽð�ǥ�� ���� ������ ����
		int startTime, endTime; // ������ ���� �ð��� ���� �ð��� ������ ����
		Subject *ptr; // ���Ͽ� Ư�� ���ø� ����ų ������

		for (i = 0; i < scheduleNum; i++) { // ���� �ð�ǥ�� ���ϵ��� �����Ͽ� minSum���� ���� ������ �߰ߵɰ�� �ش� �ð�ǥ�� min���� �Ҵ�
			startTime = userSubject->time_table[i].start_time, endTime = userSubject->time_table[i].end_time; // ���۱��ÿ� ���ᱳ�ø� i��° �ð�ǥ�� ���۱��ÿ� ���ᱳ�÷� �ʱ�ȭ
			for (j = 0; j < max_time; j++) {
				ptr = &examTable[subjectSchedule[i].day][j]; // i��° �ð�ǥ ������ 1���ú��� 15.5���ñ��� ������ �����Ǿ��ִ��� Ȯ��
				if (ptr->subject_name != "") { // �ش� ������ ���ÿ� ������ �����Ǿ��ִٸ�
					while (ptr != NULL && ptr->subject_name != "") { // ���� ���Ḯ��Ʈ�� ������ Ȯ��
						if (ptr->subject_name.substr(0, 7) != userSubject->subject_name.substr(0, 7) && // ���� ������ �й��� �ƴϰų� ���� �ð��� ��ġ�� �ʴ´ٸ� ����ġ ����
							!((ptr->time_table->start_time <= startTime && ptr->time_table->end_time > startTime) || (
								ptr->time_table->start_time < endTime && ptr->time_table->end_time >= endTime)))
							sum += ptr->student_num;
						ptr = ptr->next;
					}
				}
			}
			if (minSum > sum) { // minSum�� INT_MAX�� �ʱ�ȭ�����ν� 0��° �ð�ǥ�� �ݵ�� ����ġ���� �ּ��� �ð�ǥ�� ����, �� ���� �ð�ǥ���ʹ� 0��° �ð�ǥ�� ��
				minSum = sum;
				min = i;
			}
			sum = 0;
		}

		return min; // ���� �ּ� ���� ���Ͽ� �ش��ϴ� Schedule �迭�� �ε��� ��ȯ
	}

	void allocateExamSchedule(Subject* subjectList, int size) { // minExamDay�� ���� �� ����� ����ð�ǥ�� �����ϴ� �Լ�, subjectList�� �� �л��� ���� �迭, size�� ���� ����
		Schedule *subject_schedule; // Ư�� ������ �ð�ǥ�� ����ų ������
		Subject *ptr; // Ư�� ���Ǹ� ����ų ������

		for (int i = 0; i < size; i++) {  // �л��� ��� �������ǵ鿡 ���� ����
			subject_schedule = subjectList[i].time_table; // subject_schedule�� i��° ������ �ð�ǥ �迭�� ����Ŵ
			int examDay = minExamDay(subject_schedule, this->examSchedule, &subjectList[i]); // ����ġ�� �ּ��� ���� ����
			ptr = &(this->examSchedule[subjectList[i].time_table[examDay].day][subjectList[i].time_table[examDay].start_time]); // ptr�� ������ �ּ� ����ġ �ð�ǥ�� ����Ŵ
			if (ptr == NULL || ptr->subject_name == "") { // �ش� �ð��뿡 ������ �������� �ʴ´ٸ�
				*ptr = subjectList[i]; // ���� ����
				ptr->time_table = & subjectList[i].time_table[examDay]; // �ش� ������ �ð�ǥ �迭�� examDay�� �ش��ϴ� �ð�ǥ�� ����(�Ŀ� �ð�ǥ �迭�� Ȯ���ϴ� ���� ������ ����)
			}
			else { // �ٸ� ������ �����Ѵٸ� ���Ḯ��Ʈ�� ����
				while (ptr->next != NULL) {
					ptr = ptr->next;
				}
				ptr->next = &subjectList[i];
				ptr->next->time_table = &subjectList[i].time_table[examDay];
				ptr = ptr->next;
			}
			subjectList[i].time_table = ptr->time_table; // ���� �迭�� �ð�ǥ �迭�� ����ð��� �ش��ϴ� �ð�ǥ�� ����(�Ŀ� �ð�ǥ �迭�� Ȯ���ϴ� ���� ������ ����)
		}
	}

	void allocateResultExamSchedule(Subject* userSubject, int size) { // minExamDay�� ���� �� ����� ���� �ð�ǥ�� ��ü ���� �ð�ǥ�� �����ϴ� �Լ�
		Subject *ptr; // Ư�� ���Ǹ� ����ų ������

		for (int i = 0; i < size; i++) {
			ptr = &(this->resultExamSchedule[userSubject[i].time_table->day][userSubject[i].time_table->start_time]);
			if (ptr!= NULL && ptr->subject_name != "") { // �̹� �����Ǿ� �ִ� �ð�ǥ�� �ִٸ�
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
				else { // ���� ������ �̸��� ���� �ʴٸ� ��ü ���� �ð�ǥ�� �� ����� ���� �ð�ǥ ����
					ptr->next = new Subject;
					*ptr->next = userSubject[i];
				}
			}
			else
				*ptr = userSubject[i]; // �̹� �����Ǿ� �ִ� �ð�ǥ�� ���ٸ� �ٷ� �ð�ǥ ����
		}
	}

	void printExamSchedule() { // �� ����� �ּ��� ���� �ð�ǥ�� ����ϴ� �Լ�
		Subject* ptr; // Ư�� ���Ǹ� ����ų ������
		for (int i = 0; i < 5; i++) {
			cout << i << " : ";
			for (int j = 0; j < max_time; j++) {
				if (examSchedule[i][j].subject_name != "") {
					ptr = &examSchedule[i][j];
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

	void printExamResultSchedule() { // ��ü �л��� ���յ� ���� �ð�ǥ�� ����ϴ� �Լ�
		Subject* ptr; // Ư�� ���Ǹ� ����ų ������
		for (int i = 0; i < 5; i++) {
			cout << i << " : ";
			for (int j = 0; j < max_time; j++) {
				if (resultExamSchedule[i][j].subject_name != "") {
					ptr = &resultExamSchedule[i][j];
					cout << ptr->subject_name << " " << ptr->student_num << " ";
					while (ptr->next != NULL) {
						ptr = ptr->next;
						cout << "->" << ptr->subject_name << " " << ptr->student_num << " ";
					}
				}
			}
			cout << endl;
		}
		cout << endl;
	}

	void deleteExamResult(Subject* sub, int size) { // ��ü ���� �ð�ǥ���� �ߺ��Ǵ� ������ ���� �� ����ġ�� ���� ���踸 ���ΰ� �������� �����ϴ� �Լ�
		int max, diff_flag, sole_flag, j; // max�� ���� ������ ����� �� ���� ����ġ�� ���� ������ �ð�ǥ�� �ε����� ������ ����, diff_flag�� ����ġ�� ������ �Ǵ��ϴ� �÷��� ����(1�̸� �ٸ� ����ġ)
		// sole_flag�� Ư�� ������ ������ �������� ���� ������ ���� �ƴ� �� ���� ������ ���� �Ǵ��ϴ� �÷��� ����(1�� ��� ���� �ð�ǥ�� �� �ϳ�)
		Subject *ptr, *prev; // ptr�� Ư�� ���Ǹ� ����ų ������, prev�� ptr�� ���� ����Ű�� ������(prev->next == ptr)
		for (int i = 0; i < size; i++) { // �� �л��� ���� ������ ������ŭ ����
			max = 0; // ù��° ���� ������ �ִ� ����ġ�� �ʱ�ȭ
			sole_flag = 1; // ���� �ð�ǥ�� �� �ϳ���� �������� �ʱ�ȭ(���� ������ ���� �ð�ǥ�� �������� ��� sole_flag�� 0���� �ʱ�ȭ)
			for (j = 0; j < sub[i].schedule_num; j++) { // Ư�� ������ �ð�ǥ ��ŭ ����
				ptr = &resultExamSchedule[sub[i].time_table[j].day][sub[i].time_table[j].start_time]; // Ư�� ���ǿ� �ش��ϴ� �ð�ǥ�� ����Ŵ
				while (ptr != NULL && ptr->subject_name != "") { // Ư�� ���ǿ� �ش��ϴ� �м���ȣ�� ã�������� ����
					if (ptr->subject_name == sub[i].subject_name) /// ã���� ����
						break;
					ptr = ptr->next;
				}
				if (ptr != NULL) // Ư�� ���Ǹ� ã���� ����(���� ������ ���� �ð�ǥ �� ���� ���� �ð�ǥ�� ã����)
					break;
			}

			max = j, diff_flag = 0, ++j; // �ִ� ����ġ �ð�ǥ�� �� �������� ������ ���� ���� ���� �ð�ǥ�� ����, j�� ���� �ð�ǥ�� ����Ű���� 1 ����
			for (j ; j < sub[i].schedule_num; j++) { // ���� �ð�ǥ���� ����ġ ��
				ptr = &resultExamSchedule[sub[i].time_table[j].day][sub[i].time_table[j].start_time]; // ���� ���� �ð�ǥ�� ����Ŵ
				while (ptr != NULL && ptr->subject_name != "") { // Ư�� ���Ǹ� ã�������� ����
					if (ptr->subject_name == sub[i].subject_name) { // ã�� ��� �ٸ� ���� �ð�ǥ�� �����ϹǷ� sole_flag�� 0���� ����
						sole_flag = 0;
						break;
					}
					ptr = ptr->next;
				}
				if (ptr != NULL && ptr->subject_name == sub[i].subject_name) { // ���� ���� ���� �ð�ǥ��  ã�� ���
					if (ptr->student_num != resultExamSchedule[sub[i].time_table[max].day][sub[i].time_table[max].start_time].student_num)
						diff_flag = 1; // ������ �����ϴ� ���� �ð�ǥ�� ����ġ�� �ٸ� ��� diff_flag�� 1�� ����
					if (ptr->student_num > resultExamSchedule[sub[i].time_table[max].day][sub[i].time_table[max].start_time].student_num) // ����ġ�� �ٸ��ٸ� �� ū ����ġ�� �ð�ǥ�� max�� ����
						max = j; // 
				}
			}
			if (sole_flag == 1) { // ���� �ð�ǥ�� �� �ϳ��� ��� �״�� ���ΰ� ���� ���� ���� �ð�ǥ�� ���� ����
				continue;
			}
			if (diff_flag == 0) { // ����ġ�� �����ϴٸ� �ش� ������� ������ ���� �� ����ġ�� ���� ���� ������ ���� �ð�ǥ�� ����
				max = minExamDay(sub[i].time_table, this->resultExamSchedule, &sub[i]);
			}
			for (int j = 0; j < sub[i].schedule_num; j++) { 
				if (j != max) { // �ִ� ����ġ�� �ð�ǥ, Ȥ�� ���� ����ġ������ ������ ����ġ�� ���� ���� �ð�ǥ�� �ƴ� ������ �ð�ǥ�� ����
					ptr = &resultExamSchedule[sub[i].time_table[j].day][sub[i].time_table[j].start_time];
					prev = ptr;
					while (ptr != NULL && ptr->subject_name != "") {
						if (ptr->subject_name == sub[i].subject_name) {
							if (ptr->next == NULL)
								*ptr = { "", NULL, 0, 0, NULL };
							else {
								prev->next = prev->next->next;
								*ptr = { "", NULL, 0, 0, NULL };
								break;
							}
						}
						prev = ptr;
						ptr = ptr->next;
					}
				}
			}
		}
	}

	void examAllocate(Subject** subjectList, int student_num) { // ��ü �л��� ���������� �޾Ƽ�, �ִ��� ���� �л��� ������ ��ġ�� ��츦 ���ϰ� ����� �˰���
		Subject *subject;
		for (int i = 0; i < student_num; i++) { // �л��� ����ŭ ��ü ���� �ð�ǥ �迭�� ���� ����
			subject = subjectList[i];
			setSize(student_subjectNum[i]);
			subject = radixSort(subject);
			allocateExamSchedule(subject, size);
			cout << "��ü ��� �Ҵ� ��\n";
			printExamSchedule();
			allocateResultExamSchedule(subject, size);
			cout << "��ü ��� �Ҵ� ��\n";
			printExamResultSchedule();
			delete this->examSchedule;
			this->examSchedule = new Subject*[5]; // 5���� ���� �迭 ����(��, ȭ, ��, ��, ��)
			for (int j = 0; j < 5; j++) {
				this->examSchedule[j] = new Subject[max_time]{ {"", NULL, 0, 0, NULL} }; // ������ ���Ͽ� 1~15.5���ñ����� �迭 ����(�ð��� ��ġ�� ���Ǵ� ���� ����Ʈ�� ����)
			}
		}
		for (int i = 0; i < student_num; i++) { // �л��� ����ŭ ���ǹ迭���� �޾Ƽ� �ߺ��Ǵ� ���� ����
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