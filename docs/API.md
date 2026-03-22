# API 接口文档

## 目录

1. [概述](#概述)
2. [分类接口](#分类接口)
3. [请求格式](#请求格式)
4. [响应格式](#响应格式)
5. [错误处理](#错误处理)
6. [使用示例](#使用示例)

---

## 概述

本系统提供 REST API 接口用于文本分类预测。

**基础信息：**

| 项目 | 值 |
|------|-----|
| 协议 | HTTP |
| 端口 | 8080 |
| 编码 | UTF-8 |
| Content-Type | application/json |

---

## 分类接口

### POST /api/classify

文本分类主接口。

**请求头：**

```
Content-Type: application/json
```

**请求体：**

```json
{
    "category": 0,
    "model_id": "svm_news",
    "format": 0,
    "data": "文本内容"
}
```

**参数说明：**

| 参数 | 类型 | 必填 | 说明 |
|------|------|------|------|
| category | int | 是 | 领域分类<br>0 - 新闻 (NEWS)<br>1 - 医学 (MEDICAL)<br>2 - 科技 (TECH) |
| model_id | string | 是 | 模型标识符<br>svm_news - 自研SVM模型<br>bert_news - BERT模型（桩） |
| format | int | 是 | 输出格式<br>0 - JSON<br>1 - CSV |
| data | string | 是 | 待分类的文本内容 |

---

## 请求格式

### 1. JSON 格式（推荐）

```json
{
    "category": 0,
    "model_id": "svm_news",
    "format": 0,
    "data": "今日A股三大指数集体收涨，沪指重回3000点。"
}
```

### 2. 表单格式

```
POST /api/classify
Content-Type: multipart/form-data

category=0&model_id=svm_news&format=0&data=文本内容
```

---

## 响应格式

### 1. JSON 响应 (format=0)

**成功响应：**

```json
{
    "label": "财经",
    "success": true
}
```

**响应字段：**

| 字段 | 类型 | 说明 |
|------|------|------|
| label | string | 预测的分类标签 |
| success | bool | 是否成功 |

### 2. CSV 响应 (format=1)

```csv
label
财经
```

---

## 错误处理

### 错误响应格式

```json
{
    "error": "错误信息",
    "code": 400
}
```

### HTTP 状态码

| 状态码 | 说明 |
|--------|------|
| 200 | 成功 |
| 400 | 请求参数错误 |
| 500 | 服务器内部错误 |

### 常见错误

| 错误信息 | 可能原因 |
|----------|----------|
| 请求参数解析失败 | JSON 格式错误或字段缺失 |
| 参数无效 | category 或 model_id 不合法 |
| 服务未初始化 | 服务器启动失败 |
| 处理失败 | 分类过程中发生异常 |

---

## 使用示例

### 1. cURL

```bash
# 基本调用
curl -X POST http://localhost:8080/api/classify \
  -H "Content-Type: application/json" \
  -d '{
    "category": 0,
    "model_id": "svm_news",
    "format": 0,
    "data": "今日A股三大指数集体收涨，沪指重回3000点。"
  }'

# 使用 CSV 格式
curl -X POST http://localhost:8080/api/classify \
  -H "Content-Type: application/json" \
  -d '{
    "category": 0,
    "model_id": "svm_news",
    "format": 1,
    "data": "今日A股三大指数集体收涨，沪指重回3000点。"
  }'
```

### 2. Python

```python
import requests
import json

url = "http://localhost:8080/api/classify"

payload = {
    "category": 0,
    "model_id": "svm_news",
    "format": 0,
    "data": "今日A股三大指数集体收涨，沪指重回3000点。"
}

response = requests.post(url, json=payload)

print(response.json())
```

### 3. JavaScript

```javascript
fetch('http://localhost:8080/api/classify', {
    method: 'POST',
    headers: {
        'Content-Type': 'application/json'
    },
    body: JSON.stringify({
        category: 0,
        model_id: 'svm_news',
        format: 0,
        data: '今日A股三大指数集体收涨，沪指重回3000点。'
    })
})
.then(response => response.json())
.then(data => console.log(data))
.catch(error => console.error(error));
```

### 4. Java

```java
import java.net.http.*;
import java.net.URI;

HttpClient client = HttpClient.newHttpClient();

String jsonBody = """
{
    "category": 0,
    "model_id": "svm_news",
    "format": 0,
    "data": "今日A股三大指数集体收涨，沪指重回3000点。"
}
""";

HttpRequest request = HttpRequest.newBuilder()
    .uri(URI.create("http://localhost:8080/api/classify"))
    .header("Content-Type", "application/json")
    .POST(HttpRequest.BodyPublishers.ofString(jsonBody))
    .build();

HttpResponse<String> response = client.send(request, HttpResponse.BodyHandlers.ofString());
System.out.println(response.body());
```

---

## Web 界面

系统还提供了 Web 界面，访问 `http://localhost:8080` 即可使用浏览器进行分类测试。

Web 界面支持：
- 文本直接输入
- 文件上传（.txt）
- 实时结果显示
- JSON/CSV 格式切换
