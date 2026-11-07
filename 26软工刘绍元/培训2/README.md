# 培训2
04261220 刘绍元
环境：Ubuntu 22.04、GCC 11、CMake 3.22、OpenCV 4.5.4

## 目录结构

```
26软工刘绍元/培训2/
├── README.md
├── student_demo/
│   ├── CMakeLists.txt        # 构建静态库 student 并链接到可执行文件
│   ├── includes/student.hpp  # 类声明
│   ├── tools/student.cpp     # 类实现（打包为静态库 libstudent.a）
│   └── src/main.cpp          # 测试 main
└── opencv_demo/
    ├── CMakeLists.txt        # find_package(OpenCV) 查找并链接
    ├── main.cpp              # 读取并显示图片
    └── assets/sample.png     # 测试图片
```

## Student lib

```bash
cd student_demo
cmake -S . -B build
cmake --build build --parallel 4
./build/student_demo
cd ..
```

运行结果：

```
修改前
姓名=小明 学号=2026001 成绩=85
修改后
姓名=小明 学号=2026001 成绩=92.5
```

## OpenCV

```bash
cd opencv_demo
cmake -S . -B build
cmake --build build --parallel 4
./build/opencv_demo assets/sample.png
```

运行结果：

```
Found OpenCV: /usr (found suitable version "4.5.4", minimum required is "4")
found components: core imgcodecs highgui
...
图片尺寸: 640 x 400
```

> 程序会弹出窗口显示图片，按任意键关闭；无图形界面的环境下仅能完成编译链接。

