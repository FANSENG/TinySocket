# TinySocket
一个小型Socket服务器通信项目

## 实现内容
- 目前实现了简单的 Server 和 client 的通信
- 简单模拟了 TCP 三次握手, 还没有对互相传输内容进行Check.

## 接下来任务
- [ ] 修改 Server 端代码, 使其适用于接下来的任务.
- [ ] 整理Client代码.
- [ ] 构建一个线程池, 负责线程管理.
- [ ] 构建一个 Socket连接池, 支持同时服务多个client.
- [ ] 在代码中使用 RAII 机制, 防止内存泄漏.

## REFERENCE
1. https://blog.csdn.net/qq_44918555/article/details/125928332
2. https://blog.csdn.net/youzhangjing_/article/details/124631229
3. https://zhuanlan.zhihu.com/p/414922365