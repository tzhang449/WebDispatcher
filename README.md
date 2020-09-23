# Reactor多线程Server
参考muduo实现的多线程reactor，用于学习和测试  

## 模型：

>main reactor -> thread pool -> sub reactor 1
>                            -> sub reactor 2
>                            ...
>                            -> sub reactor n

main reactor接受tcp连接，将新连接以round rubin方式加入线程池中的一个sub reactor，sub reactor负责处理该连接

## 细节：
Eventloop  
*    基于IO multiplexing的事件分发loop，作为各个reactor的核心部分
*    使用eventfd实现了Eventloop的异步唤醒
*    除了IO事件分发，还实现了用户回调的同步执行
*    每次loop做三件事情：
        - 调用epoll_wait，获取活跃事件
        - 调用事件的callback
        - 执行用户添加的任务回调

Channel
*    保存fd的回调函数，作为连接reactor和用户的连接器

Epoller
*    使用epoll实现IO复用，这里采用了边沿触发，这要求回调函数读取描述符直至EAGAIN

Acceptor
*    listening socket的封装，用于接受新连接

Server
*    TCP server的封装，其拥有acceptor，接受新连接，并且将新连接加入线程池中的一个sub reactor

ThreadPool 
*    线程池，拥有若干个Eventloop线程，用于接受main reactor的新连接

LoopThread 
*    Eventloop线程的封装

Connection
*    简易的Tcp连接处理，这里只实现了echo功能，未来扩展其为一个http消息处理对象

Logger
*    一个异步日志库，有一个后台线程负责将日志写入磁盘，使用时前端只需轻松的写入log即可
*    格式
*    YYYY-MM-DD HH:MM:SS.MILISECOND tid [level] "Message" - file(func):line 

## 难点
Connection对象的生命期管理
这里采用了shared_ptr管理connection的生命期，Connection的Channel的回调函数bind了该shared_ptr，
因此Connection在它的Channel的callback被销毁时析构。注意销毁时要用shared_from_this()保护Connection，
否则提前析构会造成UD

## To do:
(1) add timer in Eventloop  
(2) http parser, respond to http requests.  
