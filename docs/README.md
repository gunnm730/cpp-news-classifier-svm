# C++ 新闻分类系统

基于自研 SVM 的中文文本分类系统，提供 Web 服务和 REST API 接口。

## 项目概述

本项目是一个使用纯 C++ 实现的新闻分类系统，支持：
- 自研 SVM 分类算法
- TF-IDF 文本向量化
- Web 界面交互
- REST API 接口
- 多种文件格式支持（txt, docx, zip）

## 目录结构

```
cpp-news-classifier-svm/
├── src/                      # 源代码目录
│   ├── main.cpp              # Web 服务入口
│   ├── core/                 # 训练模块（核心算法）
│   │   ├── svm.hpp/cpp      # SVM 分类器实现
│   │   ├── Tfidfvectorizer.hpp/cpp  # TF-IDF 向量化
│   │   ├── onetoall.hpp/cpp  # 一对多分类器
│   │   ├── Preprocessor.hpp/cpp  # 文本预处理
│   │   ├── types.hpp        # 类型定义
│   │   └── main.cpp          # 训练程序入口
│   ├── my_svm/              # 推理模块
│   │   └── include/         # 模型加载和预测
│   ├── api/                  # API 层
│   ├── service/              # 服务层
│   ├── proxy/                # 模型代理
│   ├── parser/               # 文件解析
│   ├── preprocessor/         # 文本预处理
│   ├── response/             # 响应格式化
│   └── network/              # HTTP 服务器
├── model/                    # 模型文件目录
├── data/                     # 训练数据目录
├── config/                  # 配置文件目录
├── vendor/                  # 第三方依赖
├── docs/                    # 项目文档
└── CMakeLists.txt           # 构建配置
```

## 核心模块说明

### 1. core 模块（训练）

负责模型训练，**不参与运行时推理**。

| 文件 | 功能 |
|------|------|
| `svm.hpp/cpp` | SVM 核心算法实现 |
| `Tfidfvectorizer.hpp/cpp` | TF-IDF 向量化算法 |
| `onetoall.hpp/cpp` | 一对多分类策略 |
| `Preprocessor.hpp/cpp` | 文本预处理（分词、停用词过滤） |
| `main.cpp` | 训练程序入口 |

### 2. my_svm 模块（推理）

负责**加载预训练模型**并进行推理，**不包含训练代码**。

| 文件 | 功能 |
|------|------|
| `svm.hpp/cpp` | SVM 模型加载 |
| `tfidf_vectorizer.hpp/cpp` | TF-IDF 模型加载 |
| `classifier.hpp/cpp` | 分类器封装 |
| `model_serializer.hpp/cpp` | 模型序列化工具 |

### 3. proxy 模块

模型代理层，提供统一的预测接口。

| 文件 | 功能 |
|------|------|
| `i_model_proxy.hpp` | 模型代理接口 |
| `svm_predictor.hpp/cpp` | SVM 预测器 |
| `bert_predictor.hpp/cpp` | BERT 预测器（桩） |
| `proxy_manager.hpp/cpp` | 代理管理器 |

### 4. api 模块

HTTP API 处理层。

| 文件 | 功能 |
|------|------|
| `http_request.hpp/cpp` | HTTP 请求解析 |
| `http_response.hpp/cpp` | HTTP 响应构建 |
| `classification_controller.hpp/cpp` | 分类控制器 |

## 快速开始

### 1. 下载依赖

```bash
# 进入项目目录
cd cpp-news-classifier-svm

# 创建 vendor 目录（如果不存在）
mkdir -p vendor
```

需要下载以下依赖到 vendor 目录：

| 依赖 | 下载地址 | 放置位置 |
|------|----------|----------|
| cpp-httplib | https://github.com/yhirose/cpp-httplib |
| nlohmann/json | https://github.com/nlohmann/json |
| cppjieba | https://github.com/yanyiwu/cppjieba |
| miniz | https://github.com/richgel999/miniz |

### 2. 编译项目

```bash
# 创建构建目录
mkdir build
cd build

# 配置 CMake
cmake ..

# 编译
cmake --build . --config Release
```

### 3. 训练模型

```bash
# 确保训练数据存在
# data/cnews.train.txt  - 训练集
# data/cnews.test.txt   - 测试集
# data/stopwords.txt    - 停用词表

# 运行训练程序
./build/core/core.exe

# 或在 Visual Studio 中打开 build 目录下的解决方案文件
```

训练完成后，模型将保存到：
- `model/svm_model.bin` - SVM 模型
- `model/svm_model_tfidf.bin` - TF-IDF 向量化器
- `model/svm_model_labels.bin` - 标签映射

### 4. 运行服务

```bash
# 启动 Web 服务
./build/news-classifier.exe

# 服务启动后访问
# http://localhost:8080
```

## API 接口

详见 [API 文档](API.md)

### 基本调用示例

```bash
# 使用 curl 调用分类接口
curl -X POST http://localhost:8080/api/classify \
  -H "Content-Type: application/json" \
  -d '{
    "category": 0,
    "model_id": "svm_news",
    "format": 0,
    "data": "今日A股三大指数集体收涨，沪指重回3000点。"
  }'
```

## 架构设计

详见 [架构文档](ARCHITECTURE.md)

## 编译指南

详见 [编译文档](COMPILATION.md)

## 训练指南

详见 [训练文档](TRAINING.md)

## 算法原理

本项目使用线性 SVM 进行文本分类，主要算法包括：

### TF-IDF 向量化

```
TF-IDF = TF × IDF

TF = 词在文档中出现次数 / 文档总词数
IDF = log(文档总数 / 包含该词的文档数) + 1
```

### 线性 SVM

```
决策函数: f(x) = w·x + b

目标: 最小化 (1/2)||w||² + Σ cost(w,b)
其中 cost = max(0, 1 - y*(w·x + b))
```

详见项目内代码注释和算法文档。

## 技术栈

- **语言**: C++17
- **构建系统**: CMake
- **HTTP 库**: cpp-httplib
- **JSON 库**: nlohmann/json
- **中文分词**: cppjieba
- **压缩支持**: miniz

## 许可证

本项目仅供学术研究使用。

## 联系方式

如有问题，请提交 Issue。
