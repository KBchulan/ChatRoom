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