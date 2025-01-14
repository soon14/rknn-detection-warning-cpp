# 介绍
基于RK3588的边缘预警项目（C++）。
采用Yolov7算法，通过读取输入视频源（RTSP、USB摄像头等），经过自创轻量级目标跟踪算法去重检测，将告警信息实时通过HTTP接口推送，同时将视频源进行转发并利用RTMP协议推送。其中告警条件可拓展，目前支持的条件为物体：
* 出现（某个物体在画面中第一次出现）
* 离开（某个物体从画面中离开）
* 经过（物体从画面中的某个方向[上/下/左/右]经过）
* 滞留（物体留在画面中超过多少秒）
* 数量（物体的数量的逻辑运算[等于/不等于/大于/小于...]）
* ...

# 编译
环境：
* 硬件：RK3588
* OS：Ubuntu
* 语言：C++17
* 工具：CMake、vcpkg等

## 安装vcpkg
采用vcpkg作为依赖管理工具，不能用vcpkg安装的依赖都放在3rd目录下了。

安装vcpkg：
```bash
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.bat
export VCPKG_ROOT={vcpkg根目录}
```

## 安装依赖

安装程序所用依赖，依赖项在`vcpkg.json`中：

*出现某文件下不下来的情况时，可以手动下载放到它指定的地方*

*出现apt-get安装软件依赖冲突情况时，可使用aptitude工具手动选择冲突解决方案*

```bash
git clone https://gitee.com/purple-sky/edge-warning-cpp.git
cd edge-warning-cpp
vcpkg install
```

安装ffmpeg（现在程序里需要ffmepg的依赖以及ffpmeg程序，以后可能会去掉直接调用ffmpeg程序）
```bash
sudo apt-get install ffmpeg
```

## 编译
```bash
cmake -Bbuild --preset=release
# cmake -Bbuild --preset=debug
# cmake -Bbuild --preset=min-release
cmake --build build
```
程序会生成在`build/bin`目录下。

## VSCode开发

1. 安装CMake Tools插件
2. 在登录VSCode的用户对应的.bashrc中设置VCPKG_ROOT环境变量: `echo "export VCPKG_ROOT={vcpkg根目录}" >> ~/.bashrc`
3. 重新登录VSCode应用环境变量
4. 打开项目根目录，在下方工具栏选择default的预设，然后生成即可


# 使用
待续...