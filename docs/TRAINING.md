# 训练指南

## 目录

1. [概述](#概述)
2. [数据准备](#数据准备)
3. [训练流程](#训练流程)
4. [模型配置](#模型配置)
5. [评估指标](#评估指标)
6. [常见问题](#常见问题)

---

## 概述

本项目使用自研的线性 SVM 进行中文文本分类。训练程序位于 `src/core/main.cpp`。

**训练模块特点**：
- 纯 C++ 实现
- 无需 Python 环境
- 模型序列化保存
- 支持多分类

---

## 数据准备

### 数据集格式

训练和测试数据使用文本格式，格式为：

```
{标签}\t{文本内容}
```

**说明**：
- 标签和文本之间使用 **制表符**（Tab）分隔
- 文本使用 **UTF-8** 编码
- 每行一个样本

**示例**：

```
体育	女排 排球 冠军 奥运
财经	股市 投资 收益 增长
科技	手机 5G 通信 技术
```

### 推荐数据集

可以使用以下中文新闻分类数据集：

1. **THUCNews**（清华大学新闻数据集）
   - 下载地址：http://thuctc.thunlp.org/
   - 包含 14 个分类

2. **Sina News**（新浪新闻数据集）
   - 包含多个分类

3. **自建数据集**
   - 按照上述格式准备数据

### 停用词表

停用词表每行一个词，用于过滤无意义的词汇。

**示例**（`data/stopwords.txt`）：

```
的
了
在
是
我
有
和
就
不
人
...
```

**推荐来源**：
- 哈工大停用词表
- 百度停用词表

### 目录结构

```
data/
├── cnews.train.txt    # 训练集
├── cnews.test.txt    # 测试集
└── stopwords.txt     # 停用词表
```

---

## 训练流程

### 1. 准备数据

确保数据文件存在：

```bash
# 检查数据目录
ls -la data/

# 预期输出：
# cnews.train.txt
# cnews.test.txt
# stopwords.txt
```

### 2. 配置路径

编辑 `src/core/main.cpp` 中的路径配置：

```cpp
// 数据路径
const std::string TRAIN_DATA_PATH = "data/cnews.train.txt";
const std::string TEST_DATA_PATH = "data/cnews.test.txt";
const std::string STOP_WORDS_PATH = "data/stopwords.txt";
const std::string MODEL_SAVE_PATH = "model/svm_model.bin";

// Jieba 分词配置
const std::string JIEBA_DICT_PATH = "vendor/cppjieba/dict/jieba.dict.utf8";
const std::string HMM_MODEL_PATH = "vendor/cppjieba/dict/hmm_model.utf8";
const std::string USER_DICT_PATH = "vendor/cppjieba/dict/user.dict.utf8";
const std::string IDF_PATH = "vendor/cppjieba/dict/idf.utf8";
```

### 3. 编译训练程序

```bash
# 进入构建目录
cd build

# 编译
cmake --build . --config Release
```

### 4. 运行训练

```bash
# 确保在项目根目录
cd cpp-news-classifier-svm

# 运行训练程序
./build/core/core.exe

# 或在 Windows 上
build\core\core.exe
```

### 5. 查看训练结果

训练成功后会输出：

```
========== SVM 文本分类训练流程启动 ==========

[阶段 1] 加载并预处理训练数据...
训练数据已加载: 50000 篇文档

[阶段 2] 训练TF-IDF向量化器...
TF-IDF向量化器训练完成

[阶段 3] 训练SVM模型...
模型训练完成，耗时 120.50 秒

[阶段 4] 保存模型...
模型已保存:
  - model/svm_model.bin (SVM模型)
  - model/svm_model_tfidf.bin (TF-IDF向量化器)
  - model/svm_model_labels.bin (标签映射)

[阶段 5] 在测试集上评估...
========== 评估完成 ==========

【总体性能】
测试样本总数: 10000
正确预测数: 8500
总体准确率: 85.00%
```

---

## 模型配置

### 超参数

可以在 `src/core/main.cpp` 中调整训练参数：

```cpp
// SVM 参数
double learning_rate = 0.01;           // 学习率
double regularization = 0.005;          // 正则化系数
size_t max_features = 50000;           // 最大特征维度
int epochs = 3;                        // 训练轮数
```

### 参数说明

| 参数 | 默认值 | 说明 | 调优建议 |
|------|--------|------|----------|
| learning_rate | 0.01 | 学习率 | 太小收敛慢，太大可能不收敛 |
| regularization | 0.005 | 正则化系数 | 增大可防止过拟合 |
| max_features | 50000 | 词汇表大小 | 根据数据集大小调整 |
| epochs | 3 | 训练轮数 | 增加可提高准确率，但会变慢 |

### 推荐参数组合

| 数据规模 | 学习率 | 正则化系数 | 训练轮数 |
|----------|--------|------------|----------|
| 小（<10K） | 0.001 | 0.01 | 5-10 |
| 中（10K-50K） | 0.01 | 0.005 | 3-5 |
| 大（>50K） | 0.01 | 0.001 | 1-3 |

---

## 评估指标

### 准确率

```
准确率 = 正确预测数 / 总样本数
```

### 各类别指标

训练程序会输出每个类别的：

- 准确率
- 正确预测数
- 总样本数

**示例输出**：

```
【各类别准确率】
类别           准确率        正确/总数
---------------------------------------------------
体育           92.50%       1850/2000
财经           88.30%       1766/2000
科技           82.10%       1642/2000
...
```

---

## 模型输出

### 文件说明

训练完成后，会在 `model/` 目录下生成以下文件：

| 文件 | 说明 | 大小 |
|------|------|------|
| `svm_model.bin` | SVM 分类器模型 | 较大 |
| `svm_model_tfidf.bin` | TF-IDF 向量化器 | 中等 |
| `svm_model_labels.bin` | 标签映射 | 较小 |

### 模型加载

Web 服务启动时会自动加载模型：

```
=== Web Text Classification System ===
Initializing...
System initialized successfully
Server starting on http://localhost:8080
```

如果模型不存在或加载失败，会输出警告信息。

---

## 常见问题

### 1. 训练数据加载失败

**问题**：`无法打开数据文件`

**解决**：
- 检查数据文件路径是否正确
- 确保数据文件存在
- 确保文件有读取权限

### 2. 分词结果为空

**问题**：`训练数据已加载: 0 篇文档`

**解决**：
- 检查停用词表路径是否正确
- 检查 cppjieba 词典文件是否存在
- 查看控制台输出的标签映射信息

### 3. 模型加载失败

**问题**：`Failed to open model file`

**解决**：
- 确保 `model/` 目录存在
- 确保模型文件完整
- 检查文件是否被其他程序占用

### 4. 内存不足

**问题**：程序崩溃

**解决**：
- 减小 `max_features` 参数
- 减少训练数据量
- 增加系统内存

### 5. 准确率过低

**问题**：分类效果不好

**解决**：
- 调整学习率和正则化系数
- 增加训练轮数
- 检查数据质量
- 增加训练数据量

---

## 后续步骤

训练完成后：

1. [编译 Web 服务](../README.md#快速开始)
2. [运行服务](../README.md#快速开始)
3. [使用 API](API.md)
