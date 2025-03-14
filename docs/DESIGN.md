# 学生管理系统设计文档

## 1. 系统架构
### 模块划分
- **用户管理模块**  
  - 功能：登录、注册、权限控制、密码修改  
  - 关键技术：文件I/O操作（`users.dat`）、输入验证（用户名唯一性、密码复杂度）  
- **学生管理模块**  
  - 功能：增删改查（CRUD）、成绩统计、排序、CSV导入  
  - 关键技术：冒泡排序、二进制文件存储（`students.dat`）  
- **跨平台适配模块**  
  - 功能：Windows/Linux终端中文支持  
  - 关键技术：`SetConsoleOutputCP`（Windows）、`setlocale`（Linux）  

### 数据流图
```mermaid
graph TD
    A[用户登录] --> B{权限检查}
    B -->|管理员| C[增删改查/导入数据]
    B -->|普通用户| D[查看学生列表/统计]
    C --> E[保存到students.dat]
    D --> F[读取students.dat]

##汉字校验（假设UTF-8编码）
for (int i = 0; i < strlen(name); i++) {
    if ((unsigned char)name[i] < 0x80) {
        printf("错误：姓名必须为汉字！");
        break;
    }
}
###难点：Windows中文乱码
- **问题现象**：控制台输出乱码  
- **解决方案**：  
  1. 设置控制台编码为UTF-8  
  2. 修改字体为“新宋体”  
  3. 使用 `wprintf` 输出宽字符  
