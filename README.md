# TinySocket
一个小型Socket服务器通信项目

## 运行方法
1. 进入项目根目录, 或执行项目根目录的 `build.sh`(`bash build.sh`).
2. 新建两个终端, 进入 `build` 目录, 先运行 `./server` 进入监听状态, 然后运行 `./client 127.0.0.1` 连接本地服务.

## 实现内容
- 目前实现了简单的 Server 和 client 的通信
- 简单模拟了 TCP 三次握手, 还没有对互相传输内容进行Check.

## 接下来任务
- [x] 修改 Server 端代码, 使其适用于接下来的任务.
- [x] 整理Client代码.
- [ ] 构建一个线程池, 负责线程管理.
- [ ] 构建一个 Socket连接池, 支持同时服务多个client.
- [ ] 在代码中使用 RAII 机制, 防止内存泄漏.

## REFERENCE
1. https://blog.csdn.net/qq_44918555/article/details/125928332
2. https://blog.csdn.net/youzhangjing_/article/details/124631229
3. https://zhuanlan.zhihu.com/p/414922365