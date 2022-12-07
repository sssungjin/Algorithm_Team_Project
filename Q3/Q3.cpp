#include <iostream>
#include <fstream>
#include <sstream>
#include <climits>

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
	string subject_name;	 // ���� �м���ȣ
	Schedule *time_table; // ���Ϻ� ���ǽð�
	int schedule_num; // ���ǽð� ���� ��
	int student_num; // �ش� ���Ǹ� ��� �ִ� �л��� ��, ����ġ�� ���
	struct subject* next; // ���� �ð��� ��ġ�� ���Ǹ� ���� ���Ḯ��Ʈ �����ͷ� ���
} Subject; // ������ ������ ��� ����ü

int student_cnt; // �����ͷ� �޾ƿ� �л��� ���� ������ ����
int *student_subjectNum; // �л��� ���������� ���� ������ ������
Subject **student_sub; // �л����� �м���ȣ�� ���� Subject ����ü�� ��ȯ
int compare_cnt = 0;     //�ð����⵵�� ���� ��Ƚ�� ������ ����

void setData() { // ���Ϸκ��� ������ ������ �޾� Subject ����ü�� �����ϴ� �Լ�
	ifstream fp("C:\\Users\\nicol\\Desktop\\algorithm_project\\Algorithm_Team_Project\\Q3\\totalData.txt"); // totalData
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

class Q3 {
	Subject **examSchedule; // [0][0]�̸� ������ 0(1����), [3][5]�̸� ����� 5(3.5����), �� �л��� ���� �ð�ǥ�� ������ ���� ������
	Subject **resultExamSchedule; // examSchedule�� ����, ��� �л��� ���� �ð�ǥ�� ������ ���� ������

public:
	Q3() {
		this->examSchedule = new Subject*[max_day]; // 5���� ���� �迭 ����(��, ȭ, ��, ��, ��)
		this->resultExamSchedule = new Subject*[max_day];
		for (int i = 0; i < 5; i++) {
			this->examSchedule[i] = new Subject[max_time]{ {"", NULL, 0, 0, NULL} }; // ������ ���Ͽ� 1~15.5���ñ����� �迭 ����(���۽ð��� ��ġ�� ���Ǵ� ���� ����Ʈ�� ����)
			this->resultExamSchedule[i] = new Subject[max_time]{ {"", NULL, 0, 0, NULL} }; // examSchedule�� ����
		}
	}

	Subject* timeSort(Subject* a, int size) { // �������� ����
		Subject *dest= new Subject[size];
		for (int i = 0; i < size; i++){		// ���� subject�迭�� �״�� �����ϱ� ���� ���ο� �迭�� �����ؼ� ���� ���� ����.
			dest[i].subject_name = a[i].subject_name; // ���� �м���ȣ
			dest[i].time_table = a[i].time_table;	  // ���Ϻ� ���ǽð�
			dest[i].schedule_num = a[i].schedule_num; // ���ǽð� ���� ��
			dest[i].student_num = a[i].student_num;	  // �ش� ���Ǹ� ��� �ִ� �л��� ��, ����ġ�� ���
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

    Subject* daySort(Subject* a, int size) { // �������� ����
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

	Subject* scheduleSort(Subject* a, int size) { // �������� ����
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

	// �� �л��� �������ǵ��� ����� �迭 subject, ���� ����, ���� ���۱���, ���� �ð� ���� ���� ���� �������, ��ȯ���� ���ĵ� �迭
	Subject* Sort(Subject *subject, int size) {
		Subject* dest=timeSort(subject, size);
		daySort(dest, size);
		scheduleSort(dest, size);
		// �л����� ���� ����. ���� ���� �켱������  1���� ���� ���� �ð� ���� ��, 2���� ���� ���� ����� ���� ���Ϻ�, 3���� ���� �����̶�� ���� �ð���
		return dest;
	}

	// ���� ���� �� ���� ������ �� �����Ǿ��ִ�(����ġ�� ���� ����) ���Ͽ� �ش��ϴ� time_table �迭�� �ε����� ��ȯ�ϴ� �Լ�
	// subjectSchedule�� Ư�� ������ �ð�ǥ �迭, examTable�� ��ü�ð�ǥ�� �ش��ϴ� 2���� �迭, userSubject�� Ư�� ����
	int minExamDay(Schedule *subjectSchedule, Subject **examTable, Subject* userSubject) {
		// min�� ������ ���� �� �����Ǿ��ִ� ����(0��° �ð�ǥ�� ������ �⺻���� �ּҷ� ����), minSum�� min���Ͽ� �ش��ϴ� ����ġ, sum�� minSum�� ���� ���� ������ ����ġ
		int i, j, min = 0, minSum = INT_MAX, sum = 0;
		int scheduleNum = userSubject->schedule_num; // ���ǽð�ǥ�� ���� ������ ����
		int startTime, endTime; // ������ ���� �ð��� ���� �ð��� ������ ����
		Subject *ptr; // ���Ͽ� Ư�� ���ø� ����ų ������

		for (i = 0; i < scheduleNum; i++) { // ���� �ð�ǥ�� ���ϵ��� �����Ͽ� minSum���� ���� ������ �߰ߵɰ�� �ش� �ð�ǥ�� min���� �Ҵ�
			startTime = userSubject->time_table[i].start_time, endTime = userSubject->time_table[i].end_time; // ���۱��ÿ� ���ᱳ�ø� i��° �ð�ǥ�� ���۱��ÿ� ���ᱳ�÷� �ʱ�ȭ
			for (j = 0; j < max_time; j++) {
				ptr = &examTable[subjectSchedule[i].day][j]; // i��° �ð�ǥ ������ 1���ú��� 15.5���ñ��� ������ �����Ǿ��ִ��� Ȯ��
				if (ptr->subject_name != "") { // �ش� ������ ���ÿ� ������ �����Ǿ��ִٸ�
					while (ptr != NULL && ptr->subject_name != "") { // ���� ���Ḯ��Ʈ�� ������ Ȯ��
						if (ptr->subject_name.substr(0, 7) != userSubject->subject_name.substr(0, 7) && ++compare_cnt && // ���� ������ �й��� �ƴϰų� ���� �ð��� ��ġ�� �ʴ´ٸ� ����ġ ����
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

	// minExamDay�� ���� �� ����� ����ð�ǥ�� �����ϴ� �Լ�, subjectList�� �� �л��� ���� �迭, size�� ���� ����
	void allocateExamSchedule(Subject* subjectList, int size) { 
		Schedule *subject_schedule; // Ư�� ������ �ð�ǥ�� ����ų ������
		Subject *ptr; // Ư�� ���Ǹ� ����ų ������

		for (int i = 0; i < size; i++) {  // �л��� ��� �������ǵ鿡 ���� ����
			subject_schedule = subjectList[i].time_table; // subject_schedule�� i��° ������ �ð�ǥ �迭�� ����Ŵ
			int examDay = minExamDay(subject_schedule, this->examSchedule, &subjectList[i]); // ����ġ�� �ּ��� ���� ����
			ptr = &(this->examSchedule[subjectList[i].time_table[examDay].day][subjectList[i].time_table[examDay].start_time]); // ptr�� ������ �ּ� ����ġ �ð�ǥ�� ����Ŵ
			*ptr = subjectList[i]; // ���� ����
			ptr->time_table = & subjectList[i].time_table[examDay]; // �ش� ������ �ð�ǥ �迭�� examDay�� �ش��ϴ� �ð�ǥ�� ����(�Ŀ� �ð�ǥ �迭�� Ȯ���ϴ� ���� ������ ����)
			subjectList[i].time_table = ptr->time_table; // ���� �迭�� �ð�ǥ �迭�� ����ð��� �ش��ϴ� �ð�ǥ�� ����(�Ŀ� �ð�ǥ �迭�� Ȯ���ϴ� ���� ������ ����)
		}
	}

	// minExamDay�� ���� �� ����� ���� �ð�ǥ�� ��ü ���� �ð�ǥ�� �����ϴ� �Լ�
	void allocateResultExamSchedule(Subject* userSubject, int size) { 
		Subject *ptr; // Ư�� ���Ǹ� ����ų ������

		for (int i = 0; i < size; i++) {
			ptr = &(this->resultExamSchedule[userSubject[i].time_table->day][userSubject[i].time_table->start_time]);
			if (ptr!= NULL && ptr->subject_name != "") { // �̹� �����Ǿ� �ִ� �ð�ǥ�� �ִٸ�
				while (++compare_cnt && ptr->next != NULL) {
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

	// ��ü ���� �ð�ǥ���� �ߺ��Ǵ� ������ ���� �� ����ġ�� ���� ���踸 ���ΰ� �������� �����ϴ� �Լ�
	void deleteExamResult(Subject* sub, int size) { 
		int max, diff_flag, sole_flag, j; // max�� ���� ������ ����� �� ���� ����ġ�� ���� ������ �ð�ǥ�� �ε����� ������ ����, diff_flag�� ����ġ�� ������ �Ǵ��ϴ� �÷��� ����(1�̸� �ٸ� ����ġ)
		// sole_flag�� Ư�� ������ ������ �������� ���� ������ ���� �ƴ� �� ���� ������ ���� �Ǵ��ϴ� �÷��� ����(1�� ��� ���� �ð�ǥ�� �� �ϳ�)
		Subject *ptr, *prev; // ptr�� Ư�� ���Ǹ� ����ų ������, prev�� ptr�� ���� ����Ű�� ������(prev->next == ptr)
		for (int i = 0; i < size; i++) { // �� �л��� ���� ������ ������ŭ ����
			max = 0; // ù��° ���� ������ �ִ� ����ġ�� �ʱ�ȭ
			sole_flag = 1; // ���� �ð�ǥ�� �� �ϳ���� �������� �ʱ�ȭ(���� ������ ���� �ð�ǥ�� �������� ��� sole_flag�� 0���� �ʱ�ȭ)
			for (j = 0; j < sub[i].schedule_num; j++) { // Ư�� ������ �ð�ǥ ��ŭ ����
				ptr = &resultExamSchedule[sub[i].time_table[j].day][sub[i].time_table[j].start_time]; // Ư�� ���ǿ� �ش��ϴ� �ð�ǥ�� ����Ŵ
				while (ptr != NULL && ptr->subject_name != "") { // Ư�� ���ǿ� �ش��ϴ� �м���ȣ�� ã�������� ����
					++compare_cnt;
					if (ptr->subject_name == sub[i].subject_name) // ã���� ����
						break;
					ptr = ptr->next;
				}
				if (ptr != NULL) // Ư�� ���Ǹ� ã���� ����(���� ������ ���� �ð�ǥ �� ���� ���� �ð�ǥ�� ã����)
					break;
			}

			max = j, diff_flag = 0, ++j; // �ִ� ����ġ �ð�ǥ�� �� �������� ������ ���� ���� ���� �ð�ǥ�� ����, j�� ���� �ð�ǥ�� ����Ű���� 1 ����
			for (j ; j < sub[i].schedule_num; j++) { // ���� �ð�ǥ���� ����ġ ��
				ptr = &resultExamSchedule[sub[i].time_table[j].day][sub[i].time_table[j].start_time]; // ���� ���� �ð�ǥ�� ����Ŵ
				while (++compare_cnt && ptr != NULL && ptr->subject_name != "") { // Ư�� ���Ǹ� ã�������� ����
					if (ptr->subject_name == sub[i].subject_name) { // ã�� ��� �ٸ� ���� �ð�ǥ�� �����ϹǷ� sole_flag�� 0���� ����
						sole_flag = 0;
						break;
					}
					ptr = ptr->next;
				}
				if (ptr != NULL && ptr->subject_name == sub[i].subject_name) { // ���� ���� ���� �ð�ǥ��  ã�� ���
					if (ptr->student_num != resultExamSchedule[sub[i].time_table[max].day][sub[i].time_table[max].start_time].student_num)
						diff_flag = 1; // ������ �����ϴ� ���� �ð�ǥ�� ����ġ�� �ٸ� ��� diff_flag�� 1�� ����
					// ����ġ�� �ٸ��ٸ� �� ū ����ġ�� �ð�ǥ�� max�� ����
					if (ptr->student_num > resultExamSchedule[sub[i].time_table[max].day][sub[i].time_table[max].start_time].student_num) 
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
						++compare_cnt;
						if (ptr->subject_name == sub[i].subject_name) {
							if (ptr->next == NULL) // ���� ������ ���� ���Ḯ��Ʈ�� ���ٸ� �ٷ� ����
								*ptr = { "", NULL, 0, 0, NULL };
							else { // ���� ���Ḯ��Ʈ�� �����Ѵٸ� ���� ���Ḯ��Ʈ�� ����
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

	// ��ü �л��� ���յ� ���� �ð�ǥ�� ����ϴ� �Լ�
	void printExamResultSchedule() {
		Subject* ptr; // Ư�� ���Ǹ� ����ų ������
		for (int i = 0; i < 5; i++) {
			switch (i) {
			case 0:
				cout << "�� : ";
				break;
			case 1:
				cout << "ȭ : ";
				break;
			case 2:
				cout << "�� : ";
				break;
			case 3:
				cout << "�� : ";
				break;
			case 4:
				cout << "�� : ";
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
		cout << "Q3 ��Ƚ�� : " << compare_cnt << endl;
	}

	// ��ü �л��� ���������� �޾Ƽ�, �ִ��� ���� �л��� ������ ��ġ�� ��츦 ���ϰ� ����� �˰���
	void examAllocate(Subject** subjectList, int student_num) { 
		Subject *subject;
		for (int i = 0; i < student_num; i++) { // �л��� ����ŭ ��ü ���� �ð�ǥ �迭�� ���� ����
			subject = subjectList[i];
			subject = Sort(subject, student_subjectNum[i]);
			allocateExamSchedule(subject, student_subjectNum[i]);
			allocateResultExamSchedule(subject, student_subjectNum[i]);
			delete this->examSchedule;
			this->examSchedule = new Subject*[5]; // 5���� ���� �迭 ����(��, ȭ, ��, ��, ��)
			for (int j = 0; j < 5; j++) {
				this->examSchedule[j] = new Subject[max_time]{ {"", NULL, 0, 0, NULL} }; // ������ ���Ͽ� 1~15.5���ñ����� �迭 ����(�ð��� ��ġ�� ���Ǵ� ���� ����Ʈ�� ����)
			}
		}
		for (int i = 0; i < student_num; i++) { // �л��� ����ŭ ���ǹ迭���� �޾Ƽ� �ߺ��Ǵ� ���� ����
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