# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

## [2025-12-18]

### 🐛 Bug Fixes
- **[server]** 客户端：增加聊天界面占位，完善流程，修改 ctrl + q，明确关闭 tcp 连接 tcp 服务器：选择把 self 进行值传递，同时放弃 协程 || 操作，选择单独挂一个读协程，并阻塞当前 session 进入读协程，此时才可以监听到对端关闭事件

## [2025-12-17]

### ✨ Features
- **[frame]** 客户端：Http 请求头增加 keep-alive，避免频繁创建连接，将 uuid 移动到 userinfo 中，jwt-token 同样如此，而不是在 serverinfo 中，确保 serverinfo 只保存服务器信息，GateWay： 引入 jwt-cpp 库，新增 JWT 组件用于生成和验证 token，新增基于 Redis 滑动窗口的限流组件，默认每分钟 100 次请求，重构 connection.cc： 从 lambda 改为成员函数，代码更清晰 支持 HTTP keep-alive，避免频繁创建连接 增加 JWT 校验中间件，非白名单路由需携带 Authorization 头 增加限流检查，超限返回 429 状态码 日志格式增强，增加 client_ip 字段便于追踪 登录接口返回 jwt_token，客户端后续请求需携带此 token

## [2025-12-16]

### ✨ Features
- **[login]** chatServer: 逻辑层修改，查询用户信息并存入 redis 和返回给客户端 引入 mariadb + redis，包括连接池以及调用，复用网关层的东西 gateWay:登录时先判断用户是否存在，若不存在则直接返回错误 status: 修改现有两个请求，调整为 uuid -> token/server_idx 的映射关系，在获取和登录时更新相关信息 client: 存储返回信息

## [2025-12-15]

### ✨ Features
- **[server]** 搭建 chatserver 骨架，详细文档参见对应目录的 develop，增加 statusserver 的登录验证服务，抽离 grpcerror，方便横向扩展

## [2025-12-13]

### ✨ Features
- **[server]** 增加客户端 Tcp 管理者，初始化 chatserver

## [2025-12-12]

### ✨ Features
- **[login]** httpmanager 增加一个处理 Login 模块的函数 完善登录页面，所有模块进行动态检查，以及登录按钮的使能状态 补充 main.cc 对重置页面的样式加载 在 mainwindow 中切换到其他页面对登录页面进行重置 增加 ServerInfo 类，用于保存 TCP 服务器信息 更换头像图片为新图片 增加状态服务器，因此封装了一个 新的 rpc client，用于处理登录请求 更新 global 的配置，增加 status_server 的配置，并修改 email_server 的名称，在 main.cc 中初始化 增加用户登录接口，与注册和重置密码类似，暂时只调用 RPC 返回一个空闲的 tcp server，真正连接请求暂未完成 增加获取 Tcp 服务器的请求，自动计算负载最小的服务器，并将请求转发到该服务器 存储 uuid -> token 的映射关系，以便后续使用

## [2025-12-11]

### ✨ Features
- **[reset]** 增加重置密码接口，与注册比较接近，写了一个数据库定时备份的服务，前端主要是对三个页面的联调，，完善了一下页面跳转以及这个重置密码的页面
- **[logs]** 增加日志落盘

## [2025-12-10]

### ✨ Features
- **[ui]** 改 LoginDialog 的信号为大驼峰命名法，因为是共有的 修改 MainWindow 的管理为父子机制，而非智能指针，界面的管理采用 stackedWidget 进行管理 实现定时按钮基类封装，可自动显示倒计时按钮 注册页面: 设置初始页面为 Page，安装过滤器，对五个输入框的焦点缺失事件进行校验，密码和确认密码搞了个眼睛切换显示密码功能，还有小手图标，注册成功后，等待5s/手动返回/page的取消按钮都会返回登录页面，并重置注册页面

## [2025-12-09]

### ✨ Features
- **[server]** 创建一个新的用户表，完善用户注册接口，更新 API 文档，增加 db_pool 的日志详情，引入 sodium 库，用于密码加盐哈希处理

## [2025-12-08]

### ✨ Features
- **[chore]** 修改验证码生成逻辑为 uuid 截取前六位，增加 redis 连接池，并封装多个操作方法接口增加 health-check 路由，用于健康检查 增加安全响应头设置，增加安全路由白名单 优化发送验证码逻辑，先查询 redis 中是否存在未过期的验证码，若存在则直接返回成功，避免频繁发送邮件

## [2025-12-07]

### ✨ Features
- **[server]** 验证码服务器增加优雅退出机制，捕获 SIGINT 和 SIGTERM 信号，调用服务器的 Shutdown 方法进行优雅关闭。网关:增加 Global 配置，引入网络层池子和逻辑层池子大小配置。实现 Bussiness，存储一个线程池，用于逻辑处理投递，默认为 8 个线程。实现 io 池子，启动 8 个线程，每个线程跑一个 ioc，并配套一个 work_guard 来维持生命，建立连接时不再绑定到主线程的 ioc，而是采用 round-bing 来分配 ioc。调整 connection 逻辑，把处理后的请求直接投递到 bussiness 的池子里处理，随后再返回 io 线程发送回包。修复 UserController/Logic/UserRepository/UserService 的单例实现。为 context 增加移动相关操作。

## [2025-12-06]

### ✨ Features
- **[emial]** 增加发送验证码的服务，集成 curl + smtp + hiredis + rpc server

## [2025-12-05]

### ✨ Features
- **[refactor]** 移除 demo 模块相关代码，移除重复包含。更新 context 模块，用于上下文存储，目前有 request_id。

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
