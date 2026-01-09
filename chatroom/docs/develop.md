## ChatRoom 客户端开发进度

### [2025-11-30] 初始化

- 创建空项目，基于 Qt 5.14.2 开发
- 创建登录和注册界面，实现基本的 UI 布局
- 主页面设置为死代码，直接为登录页面，以及登录注册的切换部分
- 登录与注册页面设置样式为米白色

### [2025-12-01]

- 增加注册页面密码输入样式
- 增加快捷键绑定 ctrl + q 以调用 close，弥补 hyprland 下无法通过标题栏关闭窗口的问题
- 增加刷新样式函数，并为注册页面增加一个 error_msg_label 用于显示错误信息，封装 show_tip 函数用于显示错误信息
- 增加注册页面的获取验证码按钮槽函数，只检查邮箱格式
- 增加 Defer 定义，模仿 Go 语言的 defer 关键字，方便资源释放
- 增加 Global 的枚举定义
- 增加 HttpManager 类，封装 HTTP 的 POST 请求功能
- 增加注册页面回调处理函数，有一个 _handlers，暂未实现具体功能

### [2025-12-03]

- 抽离配置文件 config.ini
- 完善客户端发送验证码功能

### [2025-12-09]

- 更改网络管理器，即使返回 4xx 也会在信号中发布信息
- 完善注册功能，确保有网络问题可以正常打印服务器返回的错误信息

### [2025-12-10]

- 修改 LoginDialog 的信号为大驼峰命名法，因为是共有的
- 修改 MainWindow 的管理为父子机制，而非智能指针，界面的管理采用 stackedWidget 进行管理
- 实现定时按钮基类封装，可自动显示倒计时按钮
- 注册页面: 设置初始页面为 Page，安装过滤器，对五个输入框的焦点缺失事件进行校验，密码和确认密码搞了个眼睛切换显示密码功能，还有小手图标，注册成功后，等待5s/手动返回/page的取消按钮都会返回登录页面，并重置注册页面

### [2025-12-11]

- 重置密码页面，增加重置密码的 UI 界面，以及发送逻辑，基本与登录页面一致
- 融入 login 和 mainwindow，联调完成
- login 页面的忘记密码切换小手样式，和浮动样式

### [2025-12-12]

- httpmanager 增加一个处理 Login 模块的函数
- 完善登录页面，所有模块进行动态检查，以及登录按钮的使能状态
- 补充 main.cc 对重置页面的样式加载
- 在 mainwindow 中切换到其他页面对登录页面进行重置
- 增加 ServerInfo 类，用于保存 TCP 服务器信息
- 更换头像图片为新图片
- 修改 serverinfo 的 port 为 uint16_t，避免端口为 0 的情况
- 增加 TCP 管理者，串通端口逻辑，实现 TCP 连接

### [2025-12-13]

- 增加用户信息类 UserInfo，用于保存用户信息，在 tcp 登录后设置此信息

### [2025-12-14]

- Http 请求头增加 keep-alive，避免频繁创建连接
- 将 uuid 移动到 userinfo 中，jwt-token 同样如此，而不是在 serverinfo 中，确保 serverinfo 只保存服务器信息

### [2025-12-15]

- 增加聊天界面占位，完善流程
- 修改 ctrl + q，明确关闭 tcp 连接

### [2025-12-19]

- 更新后端路由名称，为后续 nginx 反向代理做准备
- 完善文档，包括 README.md 和 develop.md
- 增加部署脚本，用于一键部署，并更新发布的 ci.yml 文件

### [2025-12-30]

- 新增 `ChatUserItem` 组件，用于显示聊天用户列表项（头像、昵称、消息预览、时间）
- 新增 `ChatUserList` 组件，继承 `QListWidget`，支持滚动到底部加载更多、鼠标悬停显示滚动条
- 完善 `ChatDialog` 聊天界面布局：
  - 左侧边栏（头像、联系人入口）
  - 中间用户列表区（搜索框 + 添加按钮 + 用户列表）
  - 右侧聊天区（标题栏 + 消息区 + 工具栏 + 输入框 + 发送按钮）
- 新增 `UIConstants` 命名空间，集中管理 UI 常量（窗口尺寸、组件高度等）
- 新增 `main.qss` 全局样式和 `chat_window.qss` 聊天界面样式
- 新增资源文件：测试头像（head_1/2/3.jpg）、搜索图标、清除图标、添加好友图标
- 主窗口切换到聊天界面时自动调整窗口尺寸

### [2026-01-01]

- 新增 `ChatPage` 组件，将聊天主体区域从 `ChatDialog` 中抽离，包含标题栏、消息列表、工具栏、输入框和发送按钮
- 新增 `ChatMsgList` 组件，继承 `QListWidget`，支持滚动到顶部加载历史消息、鼠标悬停显示滚动条、防抖处理
- 新增 `ChatMsgItem` 组件（占位），用于显示单条聊天消息气泡，暂未实现具体内容显示
- 新增 `ChatTextEdit` 组件，自定义聊天输入框
- 新增 `LoadingItem` 组件，用于显示加载动画
- 完善 `ChatUserList`，增加滚动到底部加载更多用户的防抖处理
- 新增资源文件：加载动画（loading.gif）、表情图标（smile_idle/hover.png）、文件图标（folder_idle/hover.png）
- 完善 `chat_window.qss` 样式表，新增 ChatPage、ChatMsgList、ChatTextEdit、LoadingItem 等组件样式
- 代码重构：全面应用 clang-format 格式化、信号槽参数改为 const 引用传递、变量命名规范化

### [2026-01-02 ~ 01-04] 消息气泡与发送功能

- 新增气泡组件体系，采用模板方法模式：
  - `BubbleBase`：气泡基类，绘制带小三角的圆角矩形外框，支持发送/接收两种方向
  - `TextBubble`：文本消息气泡，支持自动换行和文字选中
  - `PictureBubble`：图片消息气泡，支持自动缩放（最大 450x400）
- 重构 `ChatMsgItem` 组件，从 UI 设计器改为纯代码实现：
  - 支持设置圆形头像、昵称、气泡
  - 根据消息方向自动调整布局（发送在右，接收在左）
- 增强 `ChatTextEdit` 输入框：
  - 支持图片拖放（dragEnterEvent/dropEvent）
  - 支持图片粘贴（insertFromMimeData）
  - 自动生成缩略图（最大 350x300）
  - 滚动条透明方案：始终显示但通过 QSS 属性选择器控制透明度，避免布局抖动
- 完善 `ChatPage` 聊天页面：
  - 实现发送按钮点击事件，解析 QTextDocument 提取文本和图片
  - 支持混合消息发送（文本 + 图片自动拆分为多条消息）
  - 实现加载历史消息功能，滚动到顶部触发加载
  - 加载时显示 LoadingItem 动画，保持滚动位置补偿
- `LoadingItem` 组件支持自定义提示文本
- 新增 `MsgDirection` 枚举区分消息方向
- 新增 `UIConstants` 常量：头像尺寸、三角宽度、气泡圆角半径
- 新增密码加密函数 `encryPassword()`，使用 SHA-256 + 固定盐值
- 优化 `MainWindow` 窗口尺寸管理，切换页面时设置 minimumSize 避免窗口过小
- 完善 `chat_window.qss` 样式表，新增气泡、昵称等组件样式
- CMakeLists 更新：移除 chatmsgitem.ui，新增气泡组件源文件，增加 Qt Wayland 的 LSAN 抑制规则

### [2026-01-08] 侧边栏与多页面导航

- 新增侧边栏组件体系：
  - `SideBarWidget`：侧边栏容器，包含头像、聊天/联系人/设置三个导航项
  - `SideBarItem`：侧边栏导航项，支持 normal/hover/selected 三态图标切换和小红点提示
  - `SideBarItemType` 枚举：定义侧边栏功能项类型
- 新增联系人模块占位组件：
  - `ContactList`：联系人列表，继承 `QListWidget`
  - `ContactItem`：联系人列表项（占位）
- 新增搜索模块占位组件：
  - `SearchUserList`：搜索结果列表，继承 `QListWidget`
  - `SearchUserItem`：搜索结果列表项（占位）
- 新增 `SettingDialog` 设置对话框，支持 Ctrl+Q 快捷键关闭
- 重构 `ChatDialog` 聊天页面：
  - 将测试数据和加载逻辑下沉到 `ChatUserList`，遵循单一职责原则
  - 使用 `QPointer<QWidget>` 管理搜索状态的返回页面，避免悬空指针
  - 新增搜索状态切换逻辑：输入搜索时切换到搜索列表，清空时返回原页面
  - 侧边栏切换时正确处理搜索状态下的返回目标
- 完善 `ChatUserList`：
  - 新增 `addUserItem()` 公共方法
  - 内置测试数据和加载逻辑（showLoading/hideLoading）
- 新增资源文件：侧边栏图标（chat/contact/setting 各三态）、小红点图标
- 完善 `chat_window.qss` 样式表：
  - 新增 SideBarWidget、SideBarItem 样式
  - 统一 ChatUserList、SearchUserList、ContactList 三个列表的通用样式

### [2026-01-09] 搜索用户交互

- 新增查找结果对话框组件：
  - `FindSuccessDialog`：查找成功对话框，显示用户头像和昵称，支持添加到通讯录
  - `FindFailedDialog`：查找失败对话框，显示错误提示
  - 两个对话框均支持 Ctrl+Q 快捷键关闭，采用无边框样式
- 完善 `SearchUserItem` 搜索用户列表项组件：
  - 新增 `SetHead/SetName/SetDesc/SetUuid` 方法设置用户信息
  - 新增 `UpdateItem()` 批量更新方法
  - 新增 `GetUuid()` 获取用户唯一标识
- 完善 `SearchUserList` 搜索结果列表组件：
  - 新增 `addSearchItem()` 添加搜索结果项
  - 新增 `clearResults()` 清空搜索结果
  - 新增 `sig_item_clicked(uuid)` 信号，点击时发送用户 UUID
  - 支持鼠标悬停显示滚动条
- 完善 `ChatDialog` 聊天页面：
  - 新增事件过滤器，点击非搜索区域时自动清空搜索框并失去焦点
  - 新增 `slot_search_item_clicked()` 槽函数处理搜索项点击事件
  - 点击聊天区域时同步清空搜索状态
- 完善 `ChatMsgList` 和 `ChatPage`：
  - 新增 `sig_clicked()` 信号，用于通知父组件点击事件
- 新增 `UIConstants::SearchUserItemHeight` 常量
- 完善 `chat_window.qss` 样式表，新增 SearchUserItem 组件样式
- CMakeLists 更新：新增 static 资源目录复制
- 重构 qss，使用类名 + 属性选择器统一管理列表组件样式
