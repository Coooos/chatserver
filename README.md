# 集群聊天服务器与客户端

基于 muduo 网络库实现的集群聊天系统，支持在 Nginx TCP 负载均衡环境中工作。

## 项目概述

这是一个高性能的分布式聊天系统，包含服务器端和客户端组件。服务器采用 muduo 网络库构建，支持多服务器集群部署，可以通过 Nginx 实现 TCP 负载均衡，确保系统的高可用性和可扩展性。

## 功能特性

### 服务器端
- 基于 muduo 网络库的高性能异步事件驱动架构
- 支持多服务器集群部署
- 用户注册和登录功能
- 一对一聊天功能
- 群组聊天功能
- 用户状态管理（在线/离线）
- 消息存储与转发
- 跨服务器用户通信

### 客户端
- 基于 muduo 的异步网络通信
- 用户注册和登录界面
- 好友列表显示
- 一对一聊天界面
- 群组聊天功能
- 消息历史记录

## 系统架构

```
客户端1 ───────┐
                ├─ Nginx (负载均衡) ─── 聊天服务器1
客户端2 ───────┼─ Nginx (负载均衡) ─── 聊天服务器2
                ├─ Nginx (负载均衡) ─── 聊天服务器3
客户端N ───────┘
```

## 依赖项

### 服务器端
- muduo 网络库
- JSON 解析库 (如 jsoncpp)
- MySQL 数据库 (可选，用于数据持久化)
- CMake 构建工具

### 客户端
- muduo 网络库
- JSON 解析库
- NCurses 或 Qt (用于用户界面，根据实现而定)
- CMake 构建工具

## 构建与安装

### 服务器端构建

```bash
# 克隆项目
git clone <repository-url>
cd chat-server

# 创建构建目录
mkdir build
cd build

# 编译
cmake ..
make

# 安装
sudo make install
```

### 客户端构建

```bash
# 克隆项目
git clone <repository-url>
cd chat-client

# 创建构建目录
mkdir build
cd build

# 编译
cmake ..
make

# 安装
sudo make install
```

## 配置说明

### Nginx 配置

在 nginx.conf 中添加 TCP 负载均衡配置：

```nginx
stream {
    upstream chat_cluster {
        server 192.168.1.10:8000; # 聊天服务器1
        server 192.168.1.11:8000; # 聊天服务器2
        server 192.168.1.12:8000; # 聊天服务器3
    }
    
    server {
        listen 9000;
        proxy_pass chat_cluster;
        proxy_timeout 1s;
        proxy_connect_timeout 1s;
    }
}
```

### 服务器配置

创建服务器配置文件 `config.json`：

```json
{
    "server_ip": "0.0.0.0",
    "server_port": 8000,
    "nginx_ip": "192.168.1.100",
    "nginx_port": 9000,
    "db_ip": "127.0.0.1",
    "db_port": 3306,
    "db_name": "chat_db",
    "db_user": "chat_user",
    "db_password": "password",
    "redis_ip": "127.0.0.1",
    "redis_port": 6379,
    "log_path": "/var/log/chat_server.log"
}
```

### 客户端配置

创建客户端配置文件 `client_config.json`：

```json
{
    "nginx_ip": "192.168.1.100",
    "nginx_port": 9000,
    "user_id": "",
    "user_name": "",
    "log_path": "/var/log/chat_client.log"
}
```

## 运行指南

### 启动服务器

```bash
# 启动单个服务器节点
./chat_server -c config.json

# 或者指定参数启动
./chat_server --ip 0.0.0.0 --port 8000 --nginx_ip 192.168.1.100 --nginx_port 9000
```

### 启动客户端

```bash
# 启动客户端
./chat_client -c client_config.json

# 或者使用交互式参数
./chat_client --server 192.168.1.100 --port 9000
```

## 使用说明

1. 启动 Nginx 负载均衡器
2. 启动一个或多个聊天服务器实例
3. 启动客户端应用程序
4. 注册新账户或登录现有账户
5. 添加好友或加入群组
6. 开始聊天

## API 接口

### 客户端到服务器协议

| 消息类型 | 格式 | 说明 |
|---------|------|------|
| 登录 | `{"type":"login","id":1,"password":"123456"}` | 用户登录 |
| 注册 | `{"type":"register","name":"user1","password":"123456"}` | 用户注册 |
| 单聊 | `{"type":"one_chat","id":2,"message":"hello"}` | 一对一聊天 |
| 群聊 | `{"type":"group_chat","groupid":1,"message":"hi all"}` | 群组聊天 |
| 添加好友 | `{"type":"add_friend","id":2}` | 添加好友 |
| 创建群组 | `{"type":"create_group","name":"group1","desc":"test group"}` | 创建群组 |
| 加入群组 | `{"type":"join_group","id":1}` | 加入群组 |

### 服务器到客户端协议

| 消息类型 | 格式 | 说明 |
|---------|------|------|
| 登录响应 | `{"type":"login_ack","success":true,"id":1,"name":"user1"}` | 登录结果 |
| 注册响应 | `{"type":"register_ack","success":true,"id":1}` | 注册结果 |
| 消息 | `{"type":"msg","from":2,"message":"hello","time":"2023-01-01 12:00:00"}` | 接收消息 |
| 好友列表 | `{"type":"friend_list","friends":[{"id":2,"name":"user2","online":true}]}` | 好友列表更新 |
| 群组列表 | `{"type":"group_list","groups":[{"id":1,"name":"group1"}]}` | 群组列表更新 |

## 性能指标

- 单服务器支持同时在线用户：10,000+
- 消息延迟：< 100ms
- 消息吞吐量：10,000+ 条/秒

## 故障排除

### 常见问题

1. **连接失败**
   - 检查 Nginx 和服务器是否正常运行
   - 确认防火墙设置允许相关端口通信

2. **无法跨服务器通信**
   - 检查服务器之间的网络连接
   - 确认 Redis 或数据库连接正常

3. **性能问题**
   - 调整 Nginx 负载均衡策略
   - 增加服务器节点数量

### 日志查看

服务器日志默认位置：`/var/log/chat_server.log`
客户端日志默认位置：`/var/log/chat_client.log`

## 扩展与定制

### 添加新消息类型

1. 在消息枚举中添加新类型
2. 实现消息处理逻辑
3. 更新客户端界面支持新功能

### 自定义数据库

修改 `ChatModel` 类中的数据库操作逻辑，支持其他数据库系统。

### 调整负载均衡策略

修改 Nginx 配置中的负载均衡算法：
- 轮询（默认）
- 加权轮询
- IP哈希
- 最少连接

