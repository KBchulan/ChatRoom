# 邮件服务器

这是一个用于处理邮件发送的服务器组件，主要用于给网关层进行 RPC 调用，完成邮件发送功能，属于内部服务，因此核心代码并不复杂，仅作了如下封装：

- 基于 GRPC + PROTOBUF 实现 [RPC 服务端](./src/core/server/server.hpp)，提供发送邮件的接口。
- 基于 CURL 库封装 [SMTP 客户端](./src/core/smtp/smtp.hpp)，实现邮件发送功能。
- 简单集成 [hiredis](./src/utils/redis/Redis.hpp)，实现了一个 GET 操作。

## 介绍

本项目主要参考 [SMTP](github.com/Mq-b/CppLab/blob/main/md/C%2B%2B%E5%AE%9E%E7%8E%B0%E9%82%AE%E4%BB%B6%E5%8F%91%E9%80%81%EF%BC%9A%E5%9F%BA%E4%BA%8Ecurl%E5%BA%93%E7%9A%84Email%E6%A8%A1%E5%9D%97%E5%B0%81%E8%A3%85.md)，是基于 curl 库封装实现的，可以使用 curl 测试:

```bash
curl --url "smtp://smtp.qq.com:587" --ssl-reqd \
--mail-from "2262317520@qq.com" \
--mail-rcpt "2262317520@qq.com" \
--upload-file ./tests/test.txt \
--user "2262317520@qq.com:cwkuekbiwcvlebdi" \
--login-options "AUTH=LOGIN"
```

后续封装也是相同的逻辑，只是将 curl 的接口进行了封装，方便调用，然后启动了一个 RPC 服务，供网关层调用。

## 快速开始

对于环境的配置可以直接参考 [ci](../../.github/workflows/verify-code.yml)，里面是基于 ubuntu:latest 进行环境配置的，可以直接参考。

请把 [Global](./include/global/Global.hpp) 里的授权码修改为自己注册的邮箱授权码，我传到仓库里的是一个测试的，已删除，这个在 qq 邮箱的安全里就有，需要手动申请。

随后所有的编译以及启动操作可以查看 [指引文档](./docs/guide/README.md)。