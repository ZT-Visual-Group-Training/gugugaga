# 培训3

C++17 / OpenCV

## HSV

```bash
cmake -S hsv_demo -B hsv_demo/build
cmake --build hsv_demo/build -j4
./hsv_demo/build/hsv_demo hsv_demo/armor.png hsv_demo
```

![HSV](shots/hsv_windows.png)

## 视频识别

```bash
cmake -S detect_demo -B detect_demo/build
cmake --build detect_demo/build -j4
./detect_demo/build/detect_demo detect_demo/test_clip.mp4 result.mp4
```

![识别效果](shots/detection.jpg)

[演示视频](演示视频.zip)
