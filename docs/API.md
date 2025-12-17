# API 文档

## 用户模块

### 健康检查

```http
GET {{baseUrl}}/health-check HTTP/1.1
```

```json
{
  "code": 0,
  "data": "",
  "message": "Service is healthy"
}
```

### 发送验证码

```http
POST {{baseUrl}}/user/send-code HTTP/1.1
Content-Type: application/json

{
  "email": "2262317520@qq.com",
  "purpose": 1
}
```

```json
{
  "code": 0,
  "data": "",
  "message": "Verification code sent successfully"
}
```

### 用户注册

```http
POST {{baseUrl}}/user/register HTTP/1.1
Content-Type: application/json

{
  "nickname": "chulan",
  "email": "2262317520@qq.com",
  "password": "whx051021",
  "confirm_password": "whx051021",
  "verify_code": "89140e",
  "purpose": 1
}
```

```json
{
  "code": 0,
  "data": "",
  "message": "User registered successfully"
}
```

### 重置密码

```http
POST {{baseUrl}}/user/reset HTTP/1.1
Content-Type: application/json

{
  "email": "2262317520@qq.com",
  "password": "newwhx051021",
  "confirm_password": "newwhx051021",
  "verify_code": "89140e",
  "purpose": 2
}
```

```json
{
  "code": 0,
  "data": "",
  "message": "Password reset successfully"
}
```

### 用户登录

```http
POST {{baseUrl}}/user/login HTTP/1.1
Content-Type: application/json

{
  "user": "2262317520@qq.com",  // 可以是 email 或者 nickname
  "password": "whx051021"
}
```

```json
{
  "code": 0,
  "message": "Get tcp server info successfully",
  "data": {
    "host": "127.0.0.1",
    "port": 10004,
    "token": "96d0b4b9-5887-4c1a-9e09-d3a52e8f6b1a",
    "uuid": "715eeb30-6b1a-4554-89a2-7794bd20c3a6",
    "jwt_token": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXUyJ9.eyJleHAiOjE3NjY1NTQwOTcsImlhdCI6MTc2NTk0OTI5NywiaXNzIjoiQ2hhdFJvb20tR2F0ZVdheSIsInV1aWQiOiI3MTVlZWIzMC02YjFhLTQ1NTQtODlhMi03Nzk0YmQyMGMzYTYifQ.yWMmdsSpOv1TsZirWCYvUpiHjv_s-YcICT-tZcMioSE"
  }
}
```

### 逻辑登录

本接口为 tcp 服务接口，用于用户与 tcp 服务的登录，采用 tlv 的格式发送，其中，[1005] 为消息 id，[len] 为消息体的长度，消息体为 json 格式序列化后的字符串，[1006] 表示登录成功的回包 id:

```
[1005][len]
[{
  "uuid": "715eeb30-6b1a-4554-89a2-7794bd20c3a6",
  "token": "96d0b4b9-5887-4c1a-9e09-d3a52e8f6b1a"
}]
```

```
[1006][len]
[{
  "code": 0,
  "message": "Login successful",
  "data": {
    "nickname": "chulan",
    "avatar": "http://example.com/avatar.jpg",
    "email": "2262317520@qq.com"
  }
}]
```
