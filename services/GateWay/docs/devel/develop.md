## GateWay 网关开发进度

### [2025-12-02] 初始化

- 增加 jsoncpp 依赖。
- 创建优雅关闭的主函数，可以启动服务，并捕获 ctrl+c 信号进行优雅关闭。
- 在 global 中定义默认服务器端口和最大扁平化缓冲区大小常量。
- 创建核心服务器入口 server，用于监听所有的连接信息，采用 PIMPL 设计模式进行封装，每次挂起连接并投递连接对象处理。
- 创建单个连接 connection，核心是先创建一个 _impl，随后调用公有 Start，这会初始化三个异步操作：读取请求、发送响应、检查连接存活时间，随后挂起读取请求操作，并根据不同的请求方法投递到 logic 处理，通过 std::expected 进行优雅处理。
- logic 层注册所有路由，用哈希表进行优雅查找。
- 创建 democontroller，注册一个 GET 请求，配套的有 query。
- 设置 common_vo 作为所有请求的返回载体，包含 code、message、data 三个字段。
- code 状态码枚举，query_parser 工具解析 query 参数，type 自定义类型。

### [2025-12-03] 功能完善

- 增加 boost url 依赖，用于解析 URL，提取 query 路径和参数。
- 完善 democontroller，增加 POST、 PUT、 DELETE 三种请求方法的处理。
- 修改 utils 目录结构，增加为 common(json, func, routes)
- 更新 ci 配置，删除 jsoncpp 安装，确保统一配置。
- 创建用户模块，初始化发送邮件功能。

### [2025-12-04] RPC 框架集成 MariaDB 封装

- 配置邮件 rpc 服务器参数和 mysql 参数，并在主函数中初始化。
- 集成 RPC 框架，封装连接池子和客户端同步调用接口。
- 封装 mysql 连接池，以及 增删查改 的优雅接口。
- 完善 service -> repository 架构，则加验证码接口。

### [2025-12-05] 框架调整

- 更新 context 模块，用于上下文存储，目前有 request_id。
- 移除 demo 模块相关代码，移除重复包含。

### [2025-12-07] 服务架构调整

- 增加 Global 配置，引入网络层池子和逻辑层池子大小配置。
- 实现 Bussiness，存储一个线程池，用于逻辑处理投递，默认为 8 个线程。
- 实现 io 池子，启动 8 个线程，每个线程跑一个 ioc，并配套一个 work_guard 来维持生命，建立连接时不再绑定到主线程的 ioc，而是采用 round-bing 来分配 ioc。
- 调整 connection 逻辑，把处理后的请求直接投递到 bussiness 的池子里处理，随后再返回 io 线程发送回包。
- 修复 UserController/Logic/UserRepository/UserService 的单例实现。
- 为 context 增加移动相关操作。

### [2025-12-08] 优化调整

- 增加 redis 连接池，并封装多个操作方法接口
- 增加 health-check 路由，用于健康检查
- 增加安全响应头设置，增加安全路由白名单
- 优化发送验证码逻辑，先查询 redis 中是否存在未过期的验证码，若存在则直接返回成功，避免频繁发送邮件

### [2025-12-09] 完善用户模块

- 创建一个新的用户表，完善用户注册接口
- 更新 API 文档，增加 db_pool 的日志详情
- 引入 sodium 库，用于密码加盐哈希处理