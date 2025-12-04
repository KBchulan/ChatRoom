# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

## [2025-12-04]

### ✨ Features
- **[framework]** 配置邮件 rpc 服务器参数和 mysql 参数，并在主函数中初始化。集成 RPC 框架，封装连接池子和客户端同步调用接口。封装 mysql 连接池，以及 增删查改 的优雅接口。完善 service -> repository 架构，则加验证码接口。

## [2025-12-03]

### ✨ Features
- **[user]** 更新发送验证码骨架
- **[server]** 增加 boost url 依赖，用于解析 URL，提取 query 路径和参数。完善 democontroller，增加 POST、 PUT、 DELETE 三种请求方法的处理。修改 utils 目录结构，增加为 common(json, func, routes)

## [2025-12-02]

### ✨ Features
- **[server]** 增加 jsoncpp 依赖，创建优雅关闭的主函数，可以启动服务，并捕获 ctrl+c 信号进行优雅关闭。在 global 中定义默认服务器端口和最大扁平化缓冲区大小常量。创建核心服务器入口 server，用于监听所有的连接信息，采用 PIMPL 设计模式进行封装，每次挂起连接并投递连接对象处理。创建单个连接 connection，核心是先创建一个 _impl，随后调用公有 Start，这会初始化三个异步操作：读取请求、发送响应、检查连接存活时间，随后挂起读取请求操作，并根据不同的请求方法投递到 logic 处理，通过 std::expected 进行优雅处理。logic 层注册所有路由，用哈希表进行优雅查找。创建 democontroller，注册一个 GET 请求，配套的有 query。设置 common_vo 作为所有请求的返回载体，包含 code、message、data 三个字段。code 状态码枚举，query_parser 工具解析 query 参数，type 自定义类型。

## [2025-12-01]

### ✨ Features
- **[http]** 增加 Defer 定义，模仿 Go 语言的 defer 关键字，方便资源释放，增加 Global 的枚举定义，增加 HttpManager 类，封装 HTTP 的 POST 请求功能，增加注册页面回调处理函数，有一个 _handlers，暂未实现具体功能
- **[ui]** 增加注册页面密码输入样式,增加快捷键绑定 ctrl + q 以调用 close，弥补 hyprland 下无法通过标题栏关闭窗口的问题,增加刷新样式函数，并为注册页面增加一个 error_msg_label 用于显示错误信息，封装 show_tip 函数用于显示错误信息,增加注册页面的获取验证码按钮槽函数，只检查邮箱格式

## [2025-11-30]

### 🐛 Bug Fixes

- **[build]** 更新ci权限以发布
- **[build]** 再次c尝试build，更改文件名
- **[build]** update png size
- **[build]** 更新ci以适应新图标文件
- **[build]** 更新图标以适应打包需求
- **[ci]** 更新发布打包ci，更新根目录README文档
- **[git]** 更新hook以适应changelog
- **[infra]** 更新 CMake 以兼容 qt5，更新客户端开发文档
- **[infra]** 更新hook以支持-m参数

### ✨ Features

- **[infra]** 添加 Git hooks 自动检查 commit message 格式
- **[infra]** 添加 CHANGELOG 自动生成功能
- **[scripts]** 添加安装 Git hooks 脚本
- **[ci]** 增加 client 与 gateway 服务的 CI 流程
- **[client]** 添加 Qt 客户端基础框架，增加登录、注册界面
- **[server]** 初始化 Gateway 服务，暂未开发实际代码

### 📝 Documentation

- **[docs]** 更新项目级别 README，添加项目简介和安装说明
