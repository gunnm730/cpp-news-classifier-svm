# C++ 新闻分类系统 (cpp-news-classifier-svm)

基于 SVM 和 TF-IDF 的中文新闻分类系统，使用 C++ 实现。

---

## 目录

- [快速开始](#快速开始)
- [详细配置](#详细配置)
- [项目结构](#项目结构)
- [API 接口](#api-接口)
- [自研 SVM 模块](#自研-svm-模块)
- [已知问题与待完成功能](#已知问题与待完成功能)

---

## 快速开始

### 环境要求

- CMake 3.10+
- Visual Studio 2022 (Windows) 或 GCC 9+ (Linux)
- C++17 编译器

### 编译步骤

```bash
# 1. 进入项目目录
cd cpp-news-classifier-svm

# 2. 创建构建目录
mkdir -p build

# 3. 配置 CMake
cd build
cmake ..

# 4. 编译
cmake --build . --config Release

# 5. 运行服务器
cd ..
./build/Release/news-classifier.exe
```

### 访问服务

1. 启动服务器后，打开浏览器访问：http://localhost:8080
2. 在前端页面输入中文新闻文本
3. 点击"开始分类"查看分类结果

---

## 详细配置

### 目录结构要求

项目运行时需要以下目录和文件：

```
cpp-news-classifier-svm/
├── config/
│   └── paths.json          # 路径配置文件
├── data/                    # 训练数据（需自行准备）
│   ├── cnews.train.txt     # 训练集
│   ├── cnews.test.txt      # 测试集
│   └── stopwords.txt       # 停用词表
├── model/                   # 模型文件（训练后生成）
│   ├── svm_model.bin
│   ├── svm_model_tfidf.bin
│   └── svm_model_labels.bin
├── vendor/                  # 第三方库
│   ├── cppjieba/           # 结巴分词
│   └── cpp-httplib/        # HTTP 服务器
└── build/Release/
    └── news-classifier.exe  # 编译后的可执行文件
```

### 配置文件 (config/paths.json)

```json
{
    "jieba_dict_path": "vendor/cppjieba/dict/jieba.dict.utf8",
    "hmm_model_path": "vendor/cppjieba/dict/hmm_model.utf8",
    "user_dict_path": "vendor/cppjieba/dict/user.dict.utf8",
    "idf_path": "vendor/cppjieba/dict/idf.utf8",
    "stopwords_path": "data/stopwords.txt",
    
    "svm_model_path": "model/svm_model.bin",
    "train_data_path": "data/cnews.train.txt",
    "test_data_path": "data/cnews.test.txt",
    
    "bert_model_path": "",
    "server_port": 8080
}
```

### 训练数据格式

训练数据文件采用以下格式（标签与内容用 Tab 分隔）：

```
体育	马晓旭意外受伤让国奥警惕 无奈大雨格外青睐殷家军记者傅亚雨沈阳报道...
财经	央行宣布降准0.5个百分点 释放长期资金约1万亿元...
科技	苹果公司发布新款iPhone 搭载最新A18芯片...
...
```

### 训练模型

训练程序位于 `build/Release/core-trainer.exe`：

```bash
# 运行训练程序
./build/Release/core-trainer.exe

# 训练完成后，模型文件将保存到 model/ 目录
```

训练过程说明：
1. 加载训练数据和停用词表
2. 使用 jieba 分词
3. 构建 TF-IDF 词汇表
4. 训练 SVM 分类器
5. 保存模型到 `model/` 目录

---

## 项目结构

### 整体架构

```
┌─────────────────────────────────────────────────────────────┐
│                      前端 (Web Browser)                      │
│                    http://localhost:8080                     │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                   网络层 (Network Layer)                     │
│              HttpServer / http_server.cpp                    │
│                  监听端口，处理 HTTP 请求                     │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                   API 层 (API Layer)                         │
│         ClassificationController / classification_           │
│                    处理请求参数，返回结果                      │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                  服务层 (Service Layer)                      │
│               TaskCoordinator / task_coordinator             │
│           协调解析和预测流程，管理解析器工厂                   │
└─────────────────────────────────────────────────────────────┘
                              │
              ┌───────────────┴───────────────┐
              ▼                               ▼
┌─────────────────────────┐     ┌─────────────────────────────┐
│   解析层 (Parser Layer)  │     │    代理层 (Proxy Layer)     │
│   ParserFactory          │     │    ProxyManager             │
│   ├── TxtParser          │     │    ├── SvmPredictor         │
│   ├── DocxParser         │     │    └── BertPredictor (未完成)│
│   └── ZipParser          │     │                             │
└─────────────────────────┘     └─────────────────────────────┘
                                              │
                                              ▼
                              ┌─────────────────────────────────┐
                              │    预测层 (Prediction Layer)    │
                              │    TextPreprocessor             │
                              │    └── 分词、停用词过滤          │
                              │    SvmClassifier (自研)         │
                              │    └── TF-IDF + SVM 分类        │
                              └─────────────────────────────────┘
```

### 目录说明

| 目录 | 说明 |
|------|------|
| `src/api/` | API 控制器，处理 HTTP 请求和响应 |
| `src/common/` | 公共类型定义 |
| `src/core/` | 训练模块（SVM 训练代码） |
| `src/my_svm/` | **自研 SVM 模块**（TF-IDF + SVM） |
| `src/network/` | HTTP 服务器实现 |
| `src/parser/` | 文件解析器（TXT、DOCX、ZIP） |
| `src/preprocessor/` | 文本预处理（分词、清洗） |
| `src/proxy/` | 模型代理管理器 |
| `src/response/` | 响应格式化（JSON、CSV） |
| `src/service/` | 任务协调器 |
| `src/web/` | 前端页面 |
| `vendor/` | 第三方依赖库 |

---

## API 接口

### 分类接口

**端点**: `POST /api/classify`

**请求头**:
```
Content-Type: application/json
```

**请求体**:
```json
{
    "category": 0,
    "model_id": "svm_news",
    "format": 0,
    "data": "要分类的新闻文本内容"
}
```

**参数说明**:

| 参数 | 类型 | 必填 | 说明 |
|------|------|------|------|
| category | int | 是 | 领域分类 (0=新闻, 1=医学, 2=科技) |
| model_id | string | 是 | 模型标识符，当前支持: `svm_news` |
| format | int | 是 | 输出格式 (0=JSON, 1=CSV) |
| data | string | 是 | 要分类的文本内容 |

**响应示例**:
```json
{
    "label": "体育",
    "confidence": 0.95
}
```

### 前端页面

**端点**: `GET /`

返回 HTML 前端页面，可用于手动测试分类功能。

---

## 自研 SVM 模块

### 模块位置

`src/my_svm/include/`

### 核心文件

| 文件 | 说明 |
|------|------|
| `svm.cpp/hpp` | SVM 分类器实现 |
| `tfidf_vectorizer.cpp/hpp` | TF-IDF 向量化器 |
| `classifier.cpp/hpp` | 分类器封装 |
| `model_serializer.cpp/hpp` | 模型序列化 |
| `types.hpp` | 类型定义 |

### TF-IDF 原理

TF-IDF (Term Frequency-Inverse Document Frequency) 用于将文本转换为数值特征向量：

```
TF-IDF = TF × IDF

TF (词频) = 词语出现次数 / 文档总词数
IDF (逆文档频率) = log(文档总数 / 包含该词的文档数)
```

### SVM 原理

本项目使用**一对一多 (One-vs-All)** 策略：

1. 为每个类别训练一个二分类 SVM
2. 预测时，计算样本在所有分类器上的得分
3. 选择得分最高的分类器对应的类别作为预测结果

### 模型文件格式

训练后生成三个模型文件：

| 文件 | 说明 |
|------|------|
| `svm_model.bin` | SVM 权重和偏置 |
| `svm_model_tfidf.bin` | TF-IDF 词汇表和 IDF 值 |
| `svm_model_labels.bin` | 类别 ID 到名称的映射 |

---

## 已知问题与待完成功能

### 1. 🔴 PKL 模型调用模块未完成

**问题描述**: 代码中存在 `BertPredictor` 的占位实现，但未实际完成。

**影响**: 无法加载和使用 Python 训练的 PKL 格式模型。

**状态**: 计划中，待开发。

### 2. 🔴 新闻大类未区分训练模型

**问题描述**: 配置文件中的 `category` 参数 (0=新闻, 1=医学, 2=科技) 未实际用于选择不同模型。

**当前行为**: 所有请求都使用同一个 SVM 模型 (`model/svm_model.bin`)，即使用户选择了不同领域。

**期望行为**: 
- `category=0` → 新闻分类模型
- `category=1` → 医学分类模型
- `category=2` → 科技分类模型

**状态**: 计划中，需重构模型管理逻辑。

### 3. 🟡 训练程序需手动编译运行

**问题描述**: 模型训练需要单独编译和运行 `core-trainer.exe`。

**状态**: 计划集成到服务中。

### 4. 🟡 缺少单元测试

**问题描述**: 项目暂无单元测试覆盖。

**状态**: 计划中。

---

## 技术栈

| 组件 | 技术 |
|------|------|
| 语言 | C++17 |
| HTTP 服务器 | cpp-httplib |
| 分词 | cppjieba |
| 构建系统 | CMake |
| 编译器 | Visual Studio 2022 / GCC |

---

## 许可证

本项目仅供学习和研究使用。

---

## 参考资源

- [THUCNews 数据集](http://thuctc.thunlp.org/) - 中文新闻分类基准数据集
- [cppjieba](https://github.com/yanyiwu/cppjieba) - C++ 中文分词库
- [cpp-httplib](https://github.com/yhirose/cpp-httplib) - C++ HTTP 库
