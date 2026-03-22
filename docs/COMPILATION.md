# 编译指南

## 目录

1. [环境要求](#环境要求)
2. [依赖下载](#依赖下载)
3. [CMake 配置](#cmake-配置)
4. [编译步骤](#编译步骤)
5. [Visual Studio 配置](#visual-studio-配置)
6. [常见问题](#常见问题)

---

## 环境要求

### 编译环境

| 项目 | 要求 |
|------|------|
| 编译器 | MSVC 2019+ / GCC 9+ / Clang 10+ |
| CMake | 3.15+ |
| C++ 标准 | C++17 |

### 运行时环境

| 项目 | 要求 |
|------|------|
| 操作系统 | Windows 10+ / Linux / macOS |
| 内存 | 建议 4GB+ |
| 磁盘 | 建议 2GB+ |

---

## 依赖下载

本项目需要以下第三方依赖，请下载并放置到 `vendor` 目录下：

### 1. cpp-httplib

HTTP 服务器库。

```bash
# 使用 Git 克隆
git clone https://github.com/yhirose/cpp-httplib.git vendor/cpp-httplib
```

**放置位置**：`vendor/cpp-httplib/`

### 2. nlohmann/json

JSON 解析库。

```bash
# 使用 Git 克隆
git clone https://github.com/nlohmann/json.git vendor/json
```

**放置位置**：`vendor/json/`

### 3. cppjieba

中文分词库。

```bash
# 使用 Git 克隆
git clone --recursive https://github.com/yanyiwu/cppjieba.git vendor/cppjieba
```

**注意**：需要包含子模块 limonp。

**放置位置**：`vendor/cppjieba/`

### 4. miniz

压缩库。

```bash
# 使用 Git 克隆
git clone https://github.com/richgel999/miniz.git vendor/miniz
```

**放置位置**：`vendor/miniz/`

### 目录结构

```
vendor/
├── cpp-httplib/          # HTTP 库
│   └── httplib.h
├── json/                 # JSON 库
│   └── include/nlohmann/
├── cppjieba/             # 中文分词
│   ├── include/cppjieba/
│   └── deps/limonp/
└── miniz/                # 压缩库
    ├── miniz.h
    ├── miniz.c
    └── ...
```

---

## CMake 配置

### 基本配置

项目根目录的 `CMakeLists.txt` 已配置好所有依赖路径。

**主要配置**：

```cmake
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 依赖路径
set(HTTPLIB_INCLUDE ${VENDOR_DIR}/cpp-httplib)
set(JSON_INCLUDE ${VENDOR_DIR}/json/include)
set(CPPJIEBA_INCLUDE ${VENDOR_DIR}/cppjieba/include)
set(LIMONP_INCLUDE ${VENDOR_DIR}/cppjieba/deps/limonp/include)
```

### 编码设置

项目已配置 UTF-8 编码：

```cmake
if(MSVC)
    add_compile_options(/utf-8 /W4)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()
```

---

## 编译步骤

### Windows (命令行)

```bash
# 1. 进入项目目录
cd cpp-news-classifier-svm

# 2. 创建构建目录
mkdir build
cd build

# 3. 配置 CMake
cmake ..

# 4. 编译
cmake --build . --config Release

# 5. 可选：安装（如果配置了）
# cmake --install .
```

### Windows (Visual Studio)

```bash
# 1. 创建构建目录
mkdir build
cd build

# 2. 使用 CMake 生成 Visual Studio 项目
cmake .. -G "Visual Studio 17 2022"

# 3. 打开生成的项目文件
# start cpp-news-classifier-svm.sln

# 4. 在 Visual Studio 中编译
```

### Linux/macOS

```bash
# 1. 进入项目目录
cd cpp-news-classifier-svm

# 2. 创建构建目录
mkdir build
cd build

# 3. 配置 CMake
cmake ..

# 4. 编译
cmake --build . --config Release

# 5. 运行
./news-classifier
```

---

## Visual Studio 配置

### 1. 打开项目

使用 Visual Studio 打开项目根目录或 `CMakeLists.txt` 文件。

### 2. 配置依赖

Visual Studio 会自动识别 CMakeLists.txt 并配置项目。

### 3. 编译

1. 选择配置（Release/Debug）
2. 选择平台（x64/x86）
3. 点击「生成」→「生成解决方案」

### 4. 运行

1. 右键点击 `news-classifier` 项目
2. 选择「设为启动项目」
3. 按 F5 运行

### 5. 配置工作目录

如果运行失败，可能需要配置工作目录：

1. 右键点击 `news-classifier` 项目
2. 选择「属性」
3. 展开「配置属性」→「调试」
4. 设置「工作目录」为项目根目录

---

## 常见问题

### 1. 找不到头文件

**问题**：`'xxx.h' file not found`

**解决**：确保依赖已正确下载到 vendor 目录，并检查 CMakeLists.txt 中的路径配置。

### 2. 链接错误

**问题**：`unresolved external symbol`

**解决**：
- 检查是否所有源文件都包含在 CMakeLists.txt 中
- 检查 miniz 库是否正确编译

### 3. 编码问题

**问题**：中文字符显示乱码

**解决**：
- 确保源文件保存为 UTF-8 编码
- 确保编译器使用 `/utf-8` 选项（已配置）

### 4. 中文分词失败

**问题**：jieba 分词返回空结果

**解决**：
- 检查 cppjieba 词典文件是否存在
- 检查 `config/paths.json` 中的路径配置

### 5. 模型文件不存在

**问题**：`Failed to open model file`

**解决**：
- 确保模型文件存在于 `model/` 目录
- 或运行训练程序生成模型

---

## 编译产物

编译成功后，会在 `build/` 目录下生成：

| 文件 | 说明 |
|------|------|
| `news-classifier.exe` | Web 服务主程序 |
| `core/core.exe` | 训练程序（如果编译了 core 模块） |

---

## 后续步骤

编译成功后：

1. [训练模型](TRAINING.md)
2. [运行服务](../README.md#快速开始)
3. [使用 API](API.md)
