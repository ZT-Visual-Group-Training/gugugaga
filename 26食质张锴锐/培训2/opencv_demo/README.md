# opencv_demo

培训2 作业：使用 OpenCV 读取并显示一张图片。

## 依赖

- OpenCV 4.x（Ubuntu：`sudo apt install libopencv-dev`）
- CMake ≥ 3.10
- 支持 C++17 的编译器

## 编译

```bash
cmake -S . -B build
cmake --build build
```

## 运行

```bash
./build/app          # 显示默认图片 zhuzhuxia.jpg
./build/app 图片路径  # 指定其他图片
```

程序会弹出窗口显示图片，按任意键关闭。
