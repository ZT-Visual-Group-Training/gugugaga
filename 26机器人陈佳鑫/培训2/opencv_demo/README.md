# opencv_demo

使用 OpenCV 读取一张图片并弹出窗口显示，是 OpenCV 的入门程序。
使用 CMake 管理工程，通过 `find_package` 查找并链接 OpenCV。

## 目录结构

```text
opencv_demo/
├── CMakeLists.txt        # CMake 构建文件
├── images/
│   └── test.jpg          # 测试图片
└── src/
    └── main.cpp          # 读图并显示的源代码
```

## 环境准备（Ubuntu）

在 Ubuntu 24.04 下通过 apt 安装 OpenCV 开发包和编译工具：

```bash
sudo apt install cmake build-essential libopencv-dev
```

本机使用的版本：OpenCV 4.6.0。

## 编译方法（命令行）

```bash
cmake -B build
cmake --build build
```

## 运行方法

必须在 opencv_demo 目录下运行（代码里使用相对路径 images/test.jpg）：

```bash
./build/OPENCV_DEMO
```

运行后会弹出名为 opencv_demo 的窗口显示图片，在窗口上按任意键关闭。

## 运行结果

终端输出：

```text
图片读取成功，尺寸：480 x 320
```

同时弹出窗口显示 images/test.jpg（渐变背景、方框、圆形和 "OpenCV Demo" 文字）。

## 代码说明

- `imread("images/test.jpg")`：读取图片，返回 Mat 对象。
- `img.empty()`：判断图片是否读取失败（如文件不存在）。
- `imshow("opencv_demo", img)`：弹出窗口显示图片。
- `waitKey(0)`：等待按键，参数 0 表示一直等待；不加这句窗口会一闪而过。
