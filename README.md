# 🎓 学生管理系统（C语言实现）
> 一个支持多角色权限、数据统计与CSV导入的跨平台学生管理系统，专为教育场景设计。
## ✨ 核心功能

### 用户管理
- **多角色权限**  
  - 管理员：完整权限（增删改查、数据导入、密码修改）  
  - 普通用户：仅查看学生列表和统计信息  
- **安全登录**  
  - 3次密码尝试限制  
  - 用户名唯一性校验  
  - 密码复杂度验证（6-20字符）
   
### **学生数据操作**
- **增删改查**  
  - 学号强制校验（以 `2023` 开头的7位数字）  
  - 姓名汉字校验（2-4个汉字）  
  - 成绩范围控制（0-100分）  
- **数据统计**  
  - 平均分、最高分、最低分  
  - 成绩分布直方图（0-60, 60-70, ..., 90-100）  
  - 及格率分析  

### **高级功能**
- **CSV批量导入**  
  - 支持预览前5行数据  
  - 自动跳过无效或重复记录  
- **跨平台支持**  
  - Windows/Linux 中文UTF-8编码适配  
  - 自动清理输入缓冲区  

---

## 🛠️ 技术栈

| 模块             | 实现细节                                                                 
|------------------|-------------------------------------------------------------------------
| **文件存储**     | 二进制文件存储用户和学生数据（`users.dat` / `students.dat`）                
| **数据验证**     | 正则表达式替代方案：字符级校验（如学号前缀、汉字编码检测）                    
| **排序算法**     | 冒泡排序（支持按成绩、学号、姓名拼音排序）                                   
| **跨平台适配**   | `#ifdef` 区分Windows和Unix系统API，自动设置控制台编码和字体                  

---

## 🚀 快速开始

### 环境要求
- GCC编译器（推荐 [MinGW](https://www.mingw-w64.org/) 或 [GCC for Linux](https://gcc.gnu.org/)）
- UTF-8终端（Windows需启用新版控制台）

### 安装步骤
1. 克隆仓库：
   bash
   git clone https://github.com/huangpinlin/Student-Management-System-C.git
   cd Student-Management-System-C
