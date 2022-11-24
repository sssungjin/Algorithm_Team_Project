#include <string>

using namespace std;

enum Day {mon, tue, wed, thu, fri ,sat, fri}; // 요일에 따른 수를 열거한 열거형

typedef struct schedule{
   enum Day day; // 요일
   int start_time; // 해당 요일에서 강의 시작시간
   int end_time; // 해당 요일에서 강의 종료시간
} Schedule; // 요일에 따른 강의 시간 정보를 담고 있는 구조체

typedef struct subject {
   string subject_name; // 강의 학수번호
   Schedule *time_table; // 요일별 강의시간
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

// n은 학생들의 각자 듣는 과목들의 종류(ex : 1번 학생이 CSE2022-01, EGC2022-02 2번 CSE2022-01, EGC2021-01을 듣는다면 종류는 CSE2022-01, EGC2022-02, EGC2021-01로 총 3개)