#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "student.h"
#include "user.h"

// 声明外部变量
extern User current_user;

// 清理输入缓冲区
void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// 获取单个字符输入（y/n）
char get_yes_no_input() {
    char input;
    char line[256];
    
    while (1) {
        if (fgets(line, sizeof(line), stdin)) {
            // 检查输入是否只有一个字符加换行符
            if (strlen(line) == 2 && (line[0] == 'y' || line[0] == 'Y' || 
                                    line[0] == 'n' || line[0] == 'N')) {
                input = line[0];
                return tolower(input);
            }
        }
        printf("无效输入，请输入 y 或 n：");
    }
}

// 检查用户权限
int check_permission(UserRole required_role) {
    if (strlen(current_user.username) == 0) {
        printf("错误：请先登录！\n");
        return 0;
    }
    
    if (current_user.role < required_role) {
        printf("错误：权限不足！\n");
        return 0;
    }
    
    return 1;
}

void add_student() {
    Student s;
    FILE *fp;
    
    printf("\n========== 添加学生信息 ==========\n");
    
    // 输入学号并验证
    do {
        printf("请输入学号（必须以2023开头）: ");
        scanf("%19s", s.id);
        getchar(); // 清除换行符
        
        // 验证学号格式
        if (strncmp(s.id, "2023", 4) != 0) {
            printf("错误：学号必须以2023开头！\n");
            continue;
        }
        
        if (strlen(s.id) != 7) {
            printf("错误：学号长度必须为7位！\n");
            continue;
        }
        
        // 检查学号是否已存在
        fp = fopen("students.dat", "rb");
        if (fp) {
            Student temp;
            int exists = 0;
            while (fread(&temp, sizeof(Student), 1, fp)) {
                if (strcmp(temp.id, s.id) == 0) {
                    exists = 1;
                    break;
                }
            }
            fclose(fp);
            if (exists) {
                printf("错误：该学号已存在！\n");
                continue;
            }
        }
        break;
    } while (1);
    
    // 输入姓名并验证
    do {
        printf("\n请输入姓名（2-4个汉字）: ");
        scanf("%49s", s.name);
        getchar(); // 清除换行符
        
        // 验证姓名长度（假设一个汉字占3个字节）
        int len = strlen(s.name);
        if (len < 6 || len > 12) {  // 2个汉字=6字节，4个汉字=12字节
            printf("错误：姓名必须是2-4个汉字！\n");
            continue;
        }
        
        // 验证是否都是汉字
        int valid = 1;
        for (int i = 0; i < len; i++) {
            if ((unsigned char)s.name[i] < 0x80) {  // 非汉字
                valid = 0;
                break;
            }
        }
        if (!valid) {
            printf("错误：姓名只能包含汉字！\n");
            continue;
        }
        break;
    } while (1);
    
    // 输入成绩并验证
    do {
        printf("\n请输入成绩（0-100）: ");
        if (scanf("%f", &s.score) != 1) {
            printf("错误：请输入有效的数字！\n");
            while (getchar() != '\n'); // 清除输入缓冲
            continue;
        }
        if (s.score < 0 || s.score > 100) {
            printf("错误：成绩必须在0-100之间！\n");
            while (getchar() != '\n'); // 清除输入缓冲
            continue;
        }
        getchar(); // 清除换行符
        break;
    } while (1);
    
    // 显示输入的信息并确认
    printf("\n========== 请确认学生信息 ==========\n");
    printf("学号：%s\n", s.id);
    printf("姓名：%s\n", s.name);
    printf("成绩：%.1f\n", s.score);
    printf("===================================\n");
    
    printf("\n确认添加该学生信息吗？(y/n): ");
    char confirm = get_yes_no_input();
    
    if (confirm == 'y') {
        // 保存学生信息
        fp = fopen("students.dat", "ab");
        if (!fp) {
            printf("错误：无法打开文件！\n");
            return;
        }
        
        if (fwrite(&s, sizeof(Student), 1, fp) != 1) {
            printf("错误：保存数据失败！\n");
            fclose(fp);
            return;
        }
        
        printf("\n成功：学生信息已添加！\n");
        printf("===================================\n");
        fclose(fp);
    } else {
        printf("\n已取消添加操作。\n");
    }
}

void delete_student() {
    if (!check_permission(ROLE_ADMIN)) {
        printf("权限不足！\n");
        return;
    }

    char id[20];
    printf("\n请输入要删除的学生学号: ");
    scanf("%19s", id);
    clear_input_buffer();  // 清理输入缓冲区

    FILE *fp = fopen("students.dat", "rb");
    if (!fp) {
        printf("无法打开文件！\n");
        return;
    }

    // 查找学生
    Student student;
    int found = 0;
    while (fread(&student, sizeof(Student), 1, fp)) {
        if (strcmp(student.id, id) == 0) {
            found = 1;
            break;
        }
    }
    fclose(fp);

    if (!found) {
        printf("未找到该学生！\n");
        return;
    }

    // 显示学生信息并确认删除
    printf("\n找到学生信息：\n");
    printf("学号：%s\n", student.id);
    printf("姓名：%s\n", student.name);
    printf("成绩：%.2f\n", student.score);
    
    printf("\n确认删除该学生信息？(y/n): ");
    char confirm = get_yes_no_input();
    
    if (confirm == 'y') {
        // 创建临时文件
        FILE *fp_old = fopen("students.dat", "rb");
        FILE *fp_new = fopen("temp.dat", "wb");
        
        if (!fp_old || !fp_new) {
            printf("文件操作失败！\n");
            if (fp_old) fclose(fp_old);
            if (fp_new) fclose(fp_new);
            return;
        }

        // 复制不需要删除的记录
        Student temp;
        while (fread(&temp, sizeof(Student), 1, fp_old)) {
            if (strcmp(temp.id, id) != 0) {
                fwrite(&temp, sizeof(Student), 1, fp_new);
            }
        }

        fclose(fp_old);
        fclose(fp_new);

        // 删除原文件，重命名临时文件
        remove("students.dat");
        rename("temp.dat", "students.dat");

        printf("\n学生信息已成功删除！\n");
    } else {
        printf("\n已取消删除操作。\n");
    }
}

void modify_student() {
    if (!check_permission(ROLE_ADMIN)) {
        printf("权限不足！\n");
        return;
    }

    char id[20];
    printf("\n请输入要修改的学生学号: ");
    scanf("%19s", id);
    clear_input_buffer();

    FILE *fp = fopen("students.dat", "r+b");
    if (!fp) {
        printf("无法打开文件！\n");
        return;
    }

    Student student;
    int found = 0;
    long pos = 0;

    while (fread(&student, sizeof(Student), 1, fp)) {
        if (strcmp(student.id, id) == 0) {
            found = 1;
            break;
        }
        pos += sizeof(Student);
    }

    if (!found) {
        printf("未找到该学生！\n");
        fclose(fp);
        return;
    }

    // 显示当前信息
    printf("\n当前学生信息：\n");
    printf("学号：%s\n", student.id);
    printf("姓名：%s\n", student.name);
    printf("成绩：%.2f\n", student.score);

    Student new_info = student;  // 保存原始信息

    // 修改信息
    printf("\n请输入新信息（直接回车保持不变）：\n");
    
    // 修改姓名
    printf("姓名 [%s]: ", student.name);
    char input[50];
    if (fgets(input, sizeof(input), stdin)) {
        input[strcspn(input, "\n")] = 0;  // 移除换行符
        if (strlen(input) > 0) {
            strncpy(new_info.name, input, sizeof(new_info.name) - 1);
            new_info.name[sizeof(new_info.name) - 1] = '\0';
        }
    }

    // 修改成绩
    printf("成绩 [%.2f]: ", student.score);
    if (fgets(input, sizeof(input), stdin)) {
        input[strcspn(input, "\n")] = 0;
        if (strlen(input) > 0) {
            float new_score = atof(input);
            if (new_score >= 0 && new_score <= 100) {
                new_info.score = new_score;
            } else {
                printf("成绩必须在0-100之间，保持原值！\n");
            }
        }
    }

    // 显示修改后的信息
    printf("\n修改后的信息：\n");
    printf("学号：%s\n", new_info.id);
    printf("姓名：%s\n", new_info.name);
    printf("成绩：%.2f\n", new_info.score);

    printf("\n确认保存修改？(y/n): ");
    char confirm = get_yes_no_input();

    if (confirm == 'y') {
        fseek(fp, pos, SEEK_SET);
        if (fwrite(&new_info, sizeof(Student), 1, fp) != 1) {
            printf("保存修改失败！\n");
        } else {
            printf("\n修改成功！\n");
        }
    } else {
        printf("\n已取消修改。\n");
    }

    fclose(fp);
}

void sort_students() {
    Student arr[100];
    Student valid_arr[100];
    int count = 0;
    int valid_count = 0;
    int choice;
    char confirm;
    FILE *fp;
    
    // 打开文件并读取数据
    fp = fopen("students.dat", "rb");
    if (!fp) {
        printf("错误：无法打开学生数据文件！\n");
        return;
    }
    
    // 读取所有数据并验证
    while (fread(&arr[count], sizeof(Student), 1, fp) && count < 100) {
        // 验证数据有效性
        if (strlen(arr[count].id) > 0 && strlen(arr[count].id) < 20 &&
            strlen(arr[count].name) >= 2 && strlen(arr[count].name) < 50 &&
            arr[count].score >= 0 && arr[count].score <= 100 &&
            strncmp(arr[count].id, "2023", 4) == 0) { // 确保学号以2023开头
            valid_arr[valid_count] = arr[count];
            valid_count++;
        }
        count++;
    }
    fclose(fp);
    
    if (valid_count == 0) {
        printf("错误：没有有效的学生数据！\n");
        if (count > 0) {
            printf("提示：发现 %d 条无效数据，请检查数据格式。\n", count - valid_count);
        }
        return;
    }
    
    // 选择排序方式
    do {
        printf("\n请选择排序方式：\n");
        printf("1. 按成绩从高到低排序\n");
        printf("2. 按成绩从低到高排序\n");
        printf("3. 按学号排序\n");
        printf("4. 按姓名排序\n");
        printf("0. 取消排序\n");
        printf("请输入选择（0-4）: ");
        
        if (scanf("%d", &choice) != 1) {
            printf("错误：请输入有效的数字！\n");
            while (getchar() != '\n');
            continue;
        }
        
        if (choice >= 0 && choice <= 4) {
            break;
        }
        printf("错误：无效的选择，请重新输入！\n");
    } while (1);
    
    if (choice == 0) {
        printf("已取消排序操作。\n");
        return;
    }
    
    // 执行排序
    for (int i = 0; i < valid_count - 1; i++) {
        for (int j = 0; j < valid_count - i - 1; j++) {
            int should_swap = 0;
            
            switch (choice) {
                case 1: // 成绩从高到低
                    should_swap = valid_arr[j].score < valid_arr[j + 1].score;
                    break;
                case 2: // 成绩从低到高
                    should_swap = valid_arr[j].score > valid_arr[j + 1].score;
                    break;
                case 3: // 按学号
                    should_swap = strcmp(valid_arr[j].id, valid_arr[j + 1].id) > 0;
                    break;
                case 4: // 按姓名
                    // 获取姓氏的第一个字符进行比较
                    unsigned char first_char_j = (unsigned char)valid_arr[j].name[0];
                    unsigned char first_char_j1 = (unsigned char)valid_arr[j + 1].name[0];
                    
                    // 如果第一个字相同，则继续比较后面的字
                    if (first_char_j == first_char_j1) {
                        should_swap = strcmp(valid_arr[j].name, valid_arr[j + 1].name) > 0;
                    } else {
                        // 按照拼音顺序排序
                        const char* pinyin_order = "李孙王吴张赵郑周";
                        char name1[3] = {valid_arr[j].name[0], valid_arr[j].name[1], '\0'};
                        char name2[3] = {valid_arr[j + 1].name[0], valid_arr[j + 1].name[1], '\0'};
                        
                        int pos1 = strstr(pinyin_order, name1) - pinyin_order;
                        int pos2 = strstr(pinyin_order, name2) - pinyin_order;
                        
                        if (pos1 >= 0 && pos2 >= 0) {
                            should_swap = pos1 > pos2;
                        } else {
                            // 如果姓氏不在预定义列表中，使用字符编码比较
                            should_swap = first_char_j > first_char_j1;
                        }
                    }
                    break;
            }
            
            if (should_swap) {
                Student temp = valid_arr[j];
                valid_arr[j] = valid_arr[j + 1];
                valid_arr[j + 1] = temp;
            }
        }
    }
    
    // 显示排序结果
    printf("\n排序结果：\n");
    printf("============================================\n");
    printf("序号  学号          姓名          成绩\n");
    printf("--------------------------------------------\n");
    for (int i = 0; i < valid_count; i++) {
        printf("%-6d%-14s%-14s%.1f\n", 
               i + 1, 
               valid_arr[i].id, 
               valid_arr[i].name, 
               valid_arr[i].score);
    }
    printf("============================================\n");
    printf("共显示 %d 条有效记录\n", valid_count);
    if (count > valid_count) {
        printf("（注：有 %d 条无效记录未显示）\n", count - valid_count);
    }
    
    // 确认是否保存排序结果
    printf("\n是否保存排序结果？(y/n): ");
    confirm = get_yes_no_input();
    
    if (confirm == 'y') {
        fp = fopen("students.dat", "wb");
        if (!fp) {
            printf("错误：无法保存排序结果！\n");
            return;
        }
        
        if (fwrite(valid_arr, sizeof(Student), valid_count, fp) != valid_count) {
            printf("错误：保存排序结果失败！\n");
            fclose(fp);
            return;
        }
        
        printf("成功：排序结果已保存！\n");
        printf("注意：保存时已自动过滤无效数据。\n");
        fclose(fp);
    } else {
        printf("已取消保存排序结果。\n");
    }
}

void show_statistics() {
    float sum = 0, max = 0, min = 100;
    int count = 0;
   int distribution[5] = {0}; // 成绩分布：0-60, 60-70, 70-80, 80-90, 90-100
    int pass_count = 0;        // 及格人数
    int fail_count = 0;        // 不及格人数
    Student s;
    
    // 打开文件并检
    FILE *fp = fopen("students.dat", "rb");
   if (!fp) {
        printf("错误：无法打开学生数据文件！\n");
        return;
    }
    // 读取数据并统计
    while (fread(&s, sizeof(Student), 1, fp)) {
         // 基本统计
        sum += s.score;
        max = s.score > max ? s.score : max;
        min = s.score < min ? s.score : min;
        count++;
        // 及格统计
        if (s.score >= 60) {
            pass_count++;
        } else {
            fail_count++;
        }
        
        // 成绩分布统计
        if (s.score < 60) distribution[0]++;
        else if (s.score < 70) distribution[1]++;
        else if (s.score < 80) distribution[2]++;
        else if (s.score < 90) distribution[3]++;
        else distribution[4]++;
    }
    fclose(fp);
    
     // 检查是否有数据
    if (count == 0) {
        printf("错误：没有学生数据！\n");
        return;
    }
    
    // 计算平均分
    float average = sum / count;
    
    // 显示统计结果
    printf("\n========== 成绩统计报告 ==========\n");
    printf("总人数：%d\n", count);
    printf("--------------------------------\n");
    printf("成绩分布：\n");
    printf("0-60分：%d人 (%.1f%%)\n", distribution[0], (float)distribution[0]/count*100);
    printf("60-70分：%d人 (%.1f%%)\n", distribution[1], (float)distribution[1]/count*100);
    printf("70-80分：%d人 (%.1f%%)\n", distribution[2], (float)distribution[2]/count*100);
    printf("80-90分：%d人 (%.1f%%)\n", distribution[3], (float)distribution[3]/count*100);
    printf("90-100分：%d人 (%.1f%%)\n", distribution[4], (float)distribution[4]/count*100);
    printf("--------------------------------\n");
    printf("及格情况：\n");
    printf("及格人数：%d人 (%.1f%%)\n", pass_count, (float)pass_count/count*100);
    printf("不及格人数：%d人 (%.1f%%)\n", fail_count, (float)fail_count/count*100);
    printf("--------------------------------\n");
    printf("成绩统计：\n");
    printf("平均分：%.1f\n", average);
    printf("最高分：%.1f\n", max);
    printf("最低分：%.1f\n", min);
    printf("================================\n");
}

void import_csv() {
    char filename[100];
    char line[256];
    Student s;
    FILE *csv, *dat;
    int success_count = 0;
    int error_count = 0;
    int duplicate_count = 0;
    char confirm;
    
    // 输入文件名并验证
    do {
        printf("请输入CSV文件名（最多99个字符）: ");
        scanf("%99s", filename);
        getchar(); // 清除换行符
        
        // 检查文件是否存在
        csv = fopen(filename, "r");
        if (!csv) {
            printf("错误：无法打开CSV文件！\n");
        } else {
            break;
        }
    } while (1);
    
    // 打开或创建数据文件
    dat = fopen("students.dat", "ab+");
    if (!dat) {
        printf("错误：无法创建或打开学生数据文件！\n");
        fclose(csv);
        return;
    }
    
    // 显示CSV文件预览
    printf("\nCSV文件预览（前5行）：\n");
    printf("学号\t姓名\t成绩\n");
    int preview_count = 0;
    while (fgets(line, sizeof(line), csv) && preview_count < 5) {
        char temp_id[20], temp_name[50];
        float temp_score;
        if (sscanf(line, "%[^,],%[^,],%f", temp_id, temp_name, &temp_score) == 3) {
            printf("%s\t%s\t%.1f\n", temp_id, temp_name, temp_score);
            preview_count++;
        }
    }
    
    // 确认是否继续导入
    printf("\n是否继续导入数据？(y/n): ");
    confirm = get_yes_no_input();
    
    if (confirm == 'n') {
        printf("已取消导入操作。\n");
        fclose(csv);
        fclose(dat);
        return;
    }
    
    // 重置文件指针
    rewind(csv);
    
    // 读取并处理CSV数据
    while (fgets(line, sizeof(line), csv)) {
        // 跳过空行
        if (strlen(line) <= 1) continue;
        
        // 解析CSV行
        if (sscanf(line, "%[^,],%[^,],%f", s.id, s.name, &s.score) != 3) {
            printf("警告：无效数据格式，已跳过：%s", line);
            error_count++;
            continue;
        }
        
        // 验证数据
        if (strlen(s.id) >= 20 || strlen(s.name) >= 50 || s.score < 0 || s.score > 100) {
            printf("警告：数据范围无效，已跳过：%s", line);
            error_count++;
            continue;
        }
        
        // 检查学号是否重复
        int is_duplicate = 0;
        Student temp;
        fseek(dat, 0, SEEK_SET);  // 重置文件指针到开始
        while (fread(&temp, sizeof(Student), 1, dat)) {
            if (strcmp(temp.id, s.id) == 0) {
                is_duplicate = 1;
                break;
            }
        }
        
        if (is_duplicate) {
            printf("警告：学号 %s 已存在，已跳过\n", s.id);
            duplicate_count++;
            continue;
        }
        
        // 写入数据
        fseek(dat, 0, SEEK_END);  // 移动到文件末尾
        if (fwrite(&s, sizeof(Student), 1, dat) != 1) {
            printf("错误：写入数据失败：%s", line);
            error_count++;
        } else {
            printf("成功：已导入学生 %s 的数据\n", s.id);
            success_count++;
        }
    }
    
    // 显示导入结果
    printf("\n========== 导入结果统计 ==========\n");
    printf("成功导入：%d 条记录\n", success_count);
    printf("重复记录：%d 条\n", duplicate_count);
    printf("错误记录：%d 条\n", error_count);
    printf("================================\n");
    
    // 关闭文件
    fclose(csv);
    fclose(dat);
    
    // 刷新文件缓冲区
    fflush(dat);
}

void display_students() {
    Student s;
    FILE *fp = fopen("students.dat", "rb");
    if (!fp) {
        printf("错误：无法打开学生数据文件！\n");
        return;
    }
    
    printf("\n========== 学生信息列表 ==========\n");
    printf("学号\t姓名\t成绩\n");
    printf("--------------------------------\n");
    
    int count = 0;
    while(fread(&s, sizeof(Student), 1, fp)) {
        printf("%s\t%s\t%.1f\n", s.id, s.name, s.score);
        count++;
    }
    
    printf("--------------------------------\n");
    printf("共显示 %d 条记录\n", count);
    printf("================================\n");
    
    fclose(fp);
}

void exit_system() {
    printf("\n========== 退出系统 ==========\n");
    printf("确定要退出系统吗？(y/n): ");
    
    char confirm = get_yes_no_input();
    
    if (confirm == 'y') {
        printf("\n正在保存数据...\n");
        // 这里可以添加其他需要保存的数据
        
        printf("感谢使用学生管理系统！\n");
        printf("再见！\n");
        exit(0);
    } else {
        printf("\n已取消退出操作。\n");
    }
}