#ifndef STUDENT_H
#define STUDENT_H

// 学生结构体
typedef struct {
    char id[20];     // 学号
    char name[50];   // 姓名
    float score;     // 成绩
} Student;

// 函数声明
void add_student();
void delete_student();
void modify_student();
void sort_students();
void show_statistics();
void import_csv();
void display_students();
void exit_system();

#endif // STUDENT_H