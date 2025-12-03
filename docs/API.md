# API 文档

## Demo 模块

### GET /demo/get-test

GET {{baseUrl}}/demo/get-test?name=chulan&age=18 HTTP/1.1

{
  "code": 0,
  "message": "Success",
  "data": {
    "age": 18,
    "name": "chulan"
  }
}

### POST /demo/post-test

POST {{baseUrl}}/demo/post-test HTTP/1.1
Content-Type: application/json

{
  "title": "Test Title",
  "content": "Test Content"
}

{
  "code": 0,
  "message": "Success",
  "data": {
    "content": "Test Content",
    "title": "Test Title"
  }
}

### PUT /demo/put-test
PUT {{baseUrl}}/demo/put-test HTTP/1.1
Content-Type: application/json

{
  "title": "Test Title",
  "content": "Test Content"
}

{
  "code": 0,
  "message": "Success",
  "data": {
    "content": "Test Content",
    "title": "Test Title"
  }
}

### DELETE /demo/delete-test
DELETE {{baseUrl}}/demo/delete-test?id=123 HTTP/1.1

{
  "code": 0,
  "message": "receive id is: 123",
  "data": {
    "id": "d1f2fd98-df90-4900-84c1-d5411241da07"
  }
}