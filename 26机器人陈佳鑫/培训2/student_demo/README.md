# student_demo

实现一个 `student` 类，用于存储学生信息（姓名、学号、分数），提供信息打印和修改成绩功能。
代码分文件组织，使用 CMake 构建，并将 Student 类打包成静态库后链接使用。

## 目录结构

```text
student_demo/
├── CMakeLists.txt        # CMake 构建文件
├── includes/
│   └── student.hpp       # Student 类声明（头文件）
├── tools/
│   └── student.cpp       # Student 类成员函数实现（静态库）
└── src/
    └── main.cpp          # main 函数，测试 Student 类
```

## 平台说明

- `main.cpp` 中 `SetConsoleOutputCP` / `SetConsoleCP`（`windows.h`）用于把 Windows
  控制台代码页切换为 UTF-8，已用 `#ifdef _WIN32` 包起来。
- 在 Windows 上编译时这段代码生效；在 Linux / macOS 上编译时会自动跳过，
  所以同一份代码三个平台都能编译。

## 编译方法（命令行）

```bash
cmake -B build
cmake --build build
```

若使用 Visual Studio 编译器并需要指定配置，可加 `--config`：

```bash
cmake --build build --config Release
```

## 运行方法

可执行文件路径取决于 CMake 生成器：

| 生成器 | 可执行文件位置 | 运行命令（Windows） |
| --- | --- | --- |
| Ninja / MinGW Makefiles（单配置） | `build/STUDENT.exe` | `./build/STUDENT.exe` |
| Visual Studio（多配置，默认 Debug） | `build/Debug/STUDENT.exe` | `./build/Debug/STUDENT.exe` |
| Visual Studio（`--config Release`） | `build/Release/STUDENT.exe` | `./build/Release/STUDENT.exe` |

Linux / macOS 下可执行文件没有 `.exe` 后缀，路径为 `./build/STUDENT`。

按提示依次输入：姓名、学号、分数，再输入要修改的新分数。

## 输入校验规则

- 学号用字符串保存，可以保留前导零（如 `001`）。
- 成绩范围要求为 **0-100**：构造函数和 `setscore()` 中都会判断，
  超出范围时给出提示，构造时成绩记为 0，修改时保持原成绩不变。
- `main.cpp` 读取分数后用 `cin.fail()` 和 `cin.eof()` 检查输入状态：
  - 输入字母等非数字内容时，清空错误状态后提示重新输入；
  - 输入超出 0-100 时提示重新输入；
  - 遇到 EOF（输入流结束）时提示并退出程序。

## 运行结果示例

正常输入：

```text
请输入学生名字，学号，分数：
小明 001 100
学生信息如下：
姓名：小明
学号：001
分数：100
请输入你要修改的分数：
95
小明的分数修改完毕。
学生信息如下：
姓名：小明
学号：001
分数：95
```

非法输入（字母、越界成绩）会被拦截并要求重新输入：

```text
请输入学生名字，学号，分数：
小明 002 abc
输入无效，请重新输入分数（0-100）：101
输入无效，请重新输入分数（0-100）：88
学生信息如下：
姓名：小明
学号：002
分数：88
请输入你要修改的分数：
150
输入无效，请重新输入分数（0-100）：60
小明的分数修改完毕。
学生信息如下：
姓名：小明
学号：002
分数：60
```

## 构建说明

- `add_library(TOOLS STATIC tools/student.cpp)`：将类实现编译为静态库 TOOLS。
- `add_executable(STUDENT src/main.cpp)`：编译主程序。
- `target_link_libraries(STUDENT TOOLS)`：主程序链接静态库。
