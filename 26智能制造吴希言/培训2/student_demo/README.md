# student_demo —— Student 类 + 静态库

## 目录结构

```
student_demo/
├── CMakeLists.txt
├── includes/
│   └── student.hpp   # 类声明（头文件）
├── tools/
│   └── student.cpp   # 类的函数实现
├── src/
│   └── main.cpp      # main 函数，测试这个类
└── README.md
```

## 这个工程做了什么

1. `tools/student.cpp` 单独编译成**静态库** `student`：
   - Linux / Ubuntu：`libstudent.a`
   - Windows（MinGW）：`libstudent.a`
   - Windows（MSVC）：`student.lib`
2. `src/main.cpp` 生成的可执行文件 `student_demo` 通过 `target_link_libraries` 链接这个静态库。
3. 头文件目录 `includes/` 用 `PUBLIC` 方式挂在静态库上，所以 main 里写
   `#include "student.hpp"` 就能直接找到。

### Student 类提供的能力

- 构造：默认构造、`(name, id)`、`(name, id, scores)`
- 基本信息：`setName` / `setId` / `getName` / `getId`
- 成绩管理：`addScore` / `modifyScore(index, newScore)` / `removeScore(index)`
- 成绩查询：`getScoreCount` / `getScore` / `getAverage` / `getMax` / `getMin`
- 打印：`print()`（完整信息）、`printScores()`（只打印成绩）
- 输入保护：成绩必须在 0~100，下标越界返回 `false` 并打印错误，不会崩溃。

## 编译与运行

### Ubuntu（作业环境）

在**仓库根目录**下执行：

```bash
cd "26智能制造吴希言/培训2/student_demo"
cmake -S . -B build
cmake --build build
./build/student_demo
```

产物：

| 文件 | 说明 |
|---|---|
| `build/libstudent.a` | 静态库（实测 69 KB） |
| `build/student_demo` | 链接了该库的可执行文件 |

> Linux 下可执行文件**没有 `.exe` 后缀**。

### Windows（MinGW / MSYS2）

```bash
cmake -S . -B build -G Ninja
cmake --build build
./build/student_demo.exe
```

> 路径含中文时 `MinGW Makefiles` 生成器会失败，请用 `-G Ninja`。

## 预期输出

```
[1] 创建学生并打印信息：
====================
姓名: 吴希言
学号: 20260001
成绩:
  [0] 85.0
  [1] 92.5
  [2] 78.0
平均分: 85.2
最高分: 92.5
最低分: 78.0
====================
```

后续还会演示修改成绩、追加成绩、越界保护、删除成绩、改名。

中间会出现一行：

```
[错误] 成绩下标 10 越界（共 4 门）。
```

这是 `[4]` 那一步**故意**传越界下标的演示，用来证明保护写对了，不是程序崩溃。

## 运行结果

![student_demo 运行结果](运行结果.png)

截图里可以看到：六段演示的完整输出、越界保护生效的提示，以及最后一行 `[退出码 0]`。

## 验证静态链接

```bash
cp build/libstudent.a /tmp/
rm build/libstudent.a
./build/student_demo      # 删掉库后仍能正常运行
cp /tmp/libstudent.a build/
```

删掉库文件后程序仍能跑，说明库的内容在链接时已经复制进可执行文件——这正是"静态链接"的含义。
