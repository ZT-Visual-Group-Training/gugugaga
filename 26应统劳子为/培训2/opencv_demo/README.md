# OpenCV demo

Build and run from this directory (requires OpenCV development files):

```sh
cmake -S . -B build
cmake --build build
./build/opencv_demo images/sample.ppm
```

The program reads the supplied image, prints its dimensions, and displays it. Press a key in the image window to exit.
