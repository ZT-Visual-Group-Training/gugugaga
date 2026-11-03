# opencv_demo —— OpenCV 读取并显示图片

## 工程内容

- `main.cpp`：用 `cv::imread` 读取图片，`cv::imshow` 显示，`cv::waitKey(0)` 等待按键。
- `CMakeLists.txt`：用 `find_package(OpenCV REQUIRED)` 查找 OpenCV，
  再用 `target_link_libraries(... ${OpenCV_LIBS})` 链接。
- `test.jpg`：示例图片（640×400），可以换成自己的图。

## 编译与运行

### Ubuntu（作业环境）

在**仓库根目录**下执行：

```bash
cd "26智能制造吴希言/培训2/opencv_demo"
cmake -S . -B build
cmake --build build
./build/opencv_demo ./test.jpg
```

`find_package` 成功时的输出（实测）：

```
-- Found OpenCV: /usr (found version "4.10.0")
-- 找到 OpenCV 版本: 4.10.0
-- OpenCV 头文件目录: /usr/include/opencv4
-- Configuring done (0.1s)
```

然后弹出窗口显示图片，按任意键关闭。

> ⚠️ **必须在图形桌面里运行**。通过 ssh 或纯字符终端运行时，
> `imshow` 会报 `cannot open display` 并退出。
> 正常做法：用 VMware 窗口登录 Ubuntu 桌面，在桌面上的终端里执行。

> ℹ️ 运行时可能看到一行：
> `QSocketNotifier: Can only be used with threads started with QThread`
> 这是 Ubuntu 26.04 默认的 Wayland 会话下，OpenCV 使用 Qt5 后端时的已知警告，
> **不影响窗口显示和按键等待**（已实测：窗口正常弹出并停在 `waitKey`）。可以忽略。

## 运行结果

![opencv_demo 运行结果](运行结果.png)

截图里同时拍到了：终端中 `读取成功: ./test.jpg`、`图片尺寸: 640 x 400` 的输出，
以及弹出的 `OpenCV Demo` 图片窗口（显示的就是同目录下的 `test.jpg`）。

### Windows（MinGW / MSYS2）

```bash
cmake -S . -B build -G Ninja
cmake --build build
./build/opencv_demo.exe ./test.jpg
```

> Windows 上需要额外装 `mingw-w64-x86_64-qt6-base`（OpenCV 5 的 highgui 依赖 Qt6），
> 并且必须在 MSYS2 MINGW64 终端里运行。

## 如果 CMake 找不到 OpenCV

Ubuntu 下装了 `libopencv-dev` 就能自动找到，配置文件在：

```
/usr/lib/x86_64-linux-gnu/cmake/opencv4/OpenCVConfig.cmake
```

实在找不到可以手动指定：

```bash
cmake -S . -B build -DOpenCV_DIR=/usr/lib/x86_64-linux-gnu/cmake/opencv4
```

## 依赖安装

### Ubuntu

```bash
sudo apt update
sudo apt install -y build-essential cmake pkg-config libopencv-dev
```

### Windows（MSYS2 MINGW64）

```bash
pacman -S --needed mingw-w64-x86_64-gcc \
    mingw-w64-x86_64-cmake \
    mingw-w64-x86_64-ninja \
    mingw-w64-x86_64-opencv \
    mingw-w64-x86_64-qt6-base
```

编译器、CMake、OpenCV 必须是**同一套工具链**，混用不同来源会出现 ABI 不兼容。
