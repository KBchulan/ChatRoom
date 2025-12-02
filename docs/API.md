# API 文档

## Demo 模块

GET {{baseUrl}}/demo/get-test?name=chulan&age=18 HTTP/1.1

{
  "code": 0,
  "message": "Success",
  "data": {
    "age": 18,
    "name": "chulan"
  }
}

## 发送验证码