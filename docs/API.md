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
    "uuid": "715eeb30-6b1a-4554-89a2-7794bd20c3a6"
  }
}
```
