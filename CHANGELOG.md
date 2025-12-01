# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

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
