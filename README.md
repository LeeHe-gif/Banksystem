# 🏦 银行账户管理系统 - 大学生课程设计项目

> 📘 一个基于 Qt 和 MySQL 的完整银行账户管理系统，适合作为大学生课程设计、毕业设计或项目实践

<div align="center">

![C++](https://img.shields.io/badge/C++-17-blue?style=flat&logo=c%2B%2B)
![Qt](https://img.shields.io/badge/Qt-6.5.3-green?style=flat&logo=qt)
![MySQL](https://img.shields.io/badge/MySQL-8.0-blue?style=flat&logo=mysql)
![License](https://img.shields.io/badge/License-MIT-orange)

</div>

## 📋 项目简介

本项目是一个功能完整的银行账户管理系统，实现了银行日常业务的基本功能。系统采用**C++/Qt**作为前端开发框架，**MySQL**作为后端数据库，使用**CMake**进行项目管理，适合大学生用于学习桌面应用开发、数据库设计和软件开发的全流程。

### 🎯 适用场景
- ✅ 大学课程设计（软件工程、数据库系统）
- ✅ 毕业设计项目
- ✅ C++/Qt学习实践
- ✅ MySQL应用开发练习

## ✨ 功能特性

### 👥 客户管理模块
- **客户注册**：录入客户基本信息（姓名、身份证、电话等）
- **信息修改**：更新客户资料
- **客户删除**：支持级联删除相关账户
- **客户查询**：多条件模糊搜索

### 💳 账户管理模块
- **账户开户**：为客户创建储蓄账户、支票账户等
- **账户销户**：关闭指定账户
- **余额管理**：实时显示和更新账户余额
- **状态管理**：激活、冻结、注销账户状态切换

### 💰 交易管理模块
- **存款操作**：向账户存入资金
- **取款操作**：从账户取出资金（需检查余额）
- **转账操作**：账户间资金转移
- **交易查询**：查看历史交易记录

## 🛠️ 技术栈

| 技术 | 版本 | 用途 |
|------|------|------|
| **C++** | C++17 | 主要编程语言 |
| **Qt** | 6.5.3 | 图形界面框架 |
| **MySQL** | 8.0+ | 数据库管理系统 |
| **CMake** | 3.16+ | 项目构建工具 |
| **Git** | 2.30+ | 版本控制 |

## 📁 项目结构

```
bank-account-system/
├── CMakeLists.txt          # CMake配置文件
├── main.cpp                # 程序入口
├── mainwindow.h            # 主窗口头文件
├── mainwindow.cpp          # 主窗口实现
├── mainwindow.ui           # 界面布局文件
├── databasemanager.h       # 数据库管理类头文件
├── databasemanager.cpp     # 数据库管理类实现
└── bank_system.sql         # 数据库建表脚本
```

## 🚀 快速开始

### 第一步：环境准备

#### 1. 安装开发工具
```bash
# Ubuntu
sudo apt update
sudo apt install qt6-base-dev qt6-tools-dev-tools \
                 mysql-server libmysqlclient-dev \
                 cmake g++ git

# Windows
# 1. 安装 Qt 6.5.3 (Qt Online Installer)
# 2. 安装 MySQL 8.0 (MySQL Installer)
# 3. 安装 CMake (cmake.org)
# 4. 安装 Git (git-scm.com)
```

#### 2. 配置MySQL数据库
```sql
-- 1. 登录MySQL
mysql -u root -p

-- 2. 创建数据库和用户
CREATE DATABASE bank_system;
CREATE USER 'bank_user'@'localhost' IDENTIFIED BY '123456';
GRANT ALL PRIVILEGES ON bank_system.* TO 'bank_user'@'localhost';
FLUSH PRIVILEGES;

-- 3. 导入数据库脚本
USE bank_system;
SOURCE bank_system.sql;
```

### 第二步：编译运行

```bash
# 克隆项目
git clone https://github.com/yourusername/bank-account-system.git
cd bank-account-system

# 创建构建目录
mkdir build
cd build

# 生成构建文件
cmake ..

# 编译项目
cmake --build .

# 运行程序 (Windows使用 .\BankAccountSystem.exe)
./BankAccountSystem
```

### 第三步：修改数据库配置

编辑 `databasemanager.cpp` 文件，修改数据库连接信息：

```cpp
db.setHostName("localhost");      // MySQL服务器地址
db.setPort(3306);                 // MySQL端口
db.setDatabaseName("bank_system"); // 数据库名
db.setUserName("bank_user");      // 用户名
db.setPassword("123456");         // 密码
```

## 📊 数据库设计

### 1. 客户表 (customers)
| 字段名 | 类型 | 说明 |
|--------|------|------|
| customer_id | INT | 客户ID，主键 |
| name | VARCHAR(100) | 客户姓名 |
| id_card | VARCHAR(18) | 身份证号，唯一 |
| phone | VARCHAR(11) | 联系电话 |
| email | VARCHAR(100) | 电子邮箱 |
| address | VARCHAR(200) | 联系地址 |
| created_at | TIMESTAMP | 创建时间 |

### 2. 账户表 (accounts)
| 字段名 | 类型 | 说明 |
|--------|------|------|
| account_id | INT | 账户ID，主键 |
| account_number | VARCHAR(20) | 账户号码，唯一 |
| customer_id | INT | 客户ID，外键 |
| account_type | ENUM | 账户类型 |
| balance | DECIMAL(15,2) | 账户余额 |
| currency | VARCHAR(3) | 货币类型 |
| status | ENUM | 账户状态 |
| created_at | TIMESTAMP | 创建时间 |

### 3. 交易表 (transactions)
| 字段名 | 类型 | 说明 |
|--------|------|------|
| transaction_id | INT | 交易ID，主键 |
| account_id | INT | 账户ID，外键 |
| transaction_type | ENUM | 交易类型 |
| amount | DECIMAL(15,2) | 交易金额 |
| target_account | VARCHAR(20) | 目标账户 |
| description | VARCHAR(200) | 交易描述 |
| transaction_date | TIMESTAMP | 交易时间 |

## 🎨 界面预览

### 主界面
```
+---------------------------------------+
|       银行账户管理系统                |
+---------------------------------------+
| [客户管理] [账户管理] [交易管理]     |
|---------------------------------------|
| 搜索：_______________ [搜索]          |
|                                       |
| +-------+----------+------------+     |
| | ID    | 姓名     | 身份证     |     |
| | 001   | 张三     | 110...     |     |
| | 002   | 李四     | 110...     |     |
| +-------+----------+------------+     |
|                                       |
| 姓名：________  身份证：________     |
| 电话：________  邮箱：________       |
| 地址：________________________       |
|                                       |
| [添加] [修改] [删除] [刷新]          |
+---------------------------------------+
```

## 📝 使用说明

### 1. 添加新客户
1. 点击"客户管理"标签
2. 填写客户信息表单
3. 点击"添加客户"按钮
4. 系统验证并保存到数据库

### 2. 创建账户
1. 在"客户管理"中选择一个客户
2. 切换到"账户管理"标签
3. 客户信息自动填充
4. 输入账户信息，点击"开户"

### 3. 执行交易
1. 在"账户管理"中选择一个账户
2. 切换到"交易管理"标签
3. 选择交易类型（存款/取款/转账）
4. 输入金额和相关信息
5. 点击"执行交易"

## 🔧 常见问题

### Q1: 编译时找不到Qt库
```bash
# 设置Qt环境变量 (Linux/Mac)
export PATH="/path/to/Qt/6.5.3/gcc_64/bin:$PATH"
export CMAKE_PREFIX_PATH="/path/to/Qt/6.5.3/gcc_64"

# Windows: 在Qt Creator中打开项目
```

### Q2: 连接MySQL失败
1. 检查MySQL服务是否启动
2. 验证用户名和密码
3. 确认数据库是否存在
4. 检查防火墙设置

### Q3: 界面显示异常
1. 确保安装了Qt6 Widgets模块
2. 重新生成CMake构建文件
3. 清理构建目录重新编译

## 📚 学习资源

### 前置知识
- C++基础语法
- Qt信号槽机制
- SQL基本操作
- 数据库设计原理

### 推荐学习路径
1. **第1周**: 学习Qt基础，理解项目结构
2. **第2周**: 学习MySQL，理解数据库设计
3. **第3周**: 实现核心功能模块
4. **第4周**: 调试测试，完善功能

### 扩展学习
- 添加用户登录和权限管理
- 实现报表生成功能
- 添加数据导入导出功能
- 优化界面设计和用户体验

## 🧪 测试数据

系统包含预置的测试数据：
- 3个示例客户
- 4个银行账户
- 不同类型的交易记录

可以通过修改 `bank_system.sql` 文件添加更多测试数据。

## 📄 许可证

本项目采用 MIT 许可证 - 查看 [LICENSE](LICENSE) 文件了解详情。

## 👥 贡献指南

欢迎提交 Issue 和 Pull Request！

1. Fork 本项目
2. 创建功能分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 开启 Pull Request

## 📞 联系方式

如有问题或建议，请通过以下方式联系：

- **邮箱**: your.email@example.com
- **GitHub Issues**: [提交问题](https://github.com/yourusername/bank-account-system/issues)

## 🌟 Star History

[![Star History Chart](https://api.star-history.com/svg?repos=yourusername/bank-account-system&type=Date)](https://star-history.com/#yourusername/bank-account-system&Date)

---

<div align="center">
  
### 🎓 特别适合大学生课程设计
  
**如果这个项目对你有帮助，请给个 ⭐️ 支持一下！**
  
</div>