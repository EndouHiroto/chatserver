# ChatServer
## 一. 介绍
在nginx tcp负载均衡环境中的集群聊天服务器和客户端源码 基于moduo库实现。

## 二. 技术栈
项目开发涉及到以下几个方面
- Json序列化和反序列化
- Nginx源码编译安装和环境部署
- Nginx的TCP负载均衡器配置
- Redis缓存服务器编程
- muduo网络库编程
- 基于发布-订阅的服务器中间件Redis消息队列编程
- MySQL数据库编程
- github管理项目
- CMake构建编译环境

## 三. 模块设计  
采用分层的设计思想，分为网络I/O模块、业务模块、数据库模块  
  ### 1.网络I/O模块
  - 基于muduo库采用的网络并发模型为one loop per thread，即通过一个main reactor来负责accept连接，然后该连接的I/O事件，计算任务等都在sub reactor中完成。多个连接可能被分派到多个线程中，来充分利用CPU。  
  - 利用muduo库写网络模块的代码非常简洁，只需要在定义的ChatServer中定义muduo的TcpServer和EventLoop，然后注册好onConnection和onMessage两个回调，就可以很方便的起一个服务。  
  - onConnection是建立连接。  
  - onMessage则是根据不同的json消息中的msgid字段，来在业务层调用不同的回调函数来处理请求。
  
  ### 2.业务模块  
  基于单例模式设计一个负责处理业务的ChatService类，成员：所有的数据库操作类的对象，一个用来映射msgid到回调函数的map，一个互斥锁保证map的访问安全，还有一个存储在线用户的通信连接的map，redis的操作对象。然后所有的业务处理函数都在构造函数中注册到了对应的map上。  

  ### 3.数据库模块  
  为user, gourpuser, group表设计一个ORM类，为每一个ORM类添加一个数据操作类，操作ORM类对象。这样的设计将复杂的业务逻辑和数据处理逻辑分离，降低系统的耦合度，有利于扩展。

## 四. 项目目录结构
```
bin：可执行文件
build：项目编译文件
include：头文件
src：源文件
test：json和muduo库的测试代码
thirdparty：json头文件
CMakeLists.txt：顶层的cmake文件
README.md：项目自述文件
autobuild.sh：一键编译脚本
```
- `autobuild.sh`是项目的一键编译脚本  
- 项目最终会编译完成后提供给用户的是ChatClient和ChatServer，都放在bin下面  



