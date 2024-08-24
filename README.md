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
    #### 2-1.登录  
    - 首先判断是否已经登陆，如已经登陆则返回错误提示信息  
    - 登陆成功，记录用户的连接信息，更新用户的状态  
    - 向redis订阅channel(id)  
    - 将离线消息封装到json消息中，然后调用offlinemessageModel中的remove方法删除读过的离线消息  
    - 显示好友列表：通过friendModel中的query方法，通过用户id查找所有的好友id，返回对应的好友user对象  
    - 显示群组列表：同显示好友列表，通过groupuserModel中的query方法，返回这个用户所在的所有群组的信息  
    - 登陆失败，提示错误信息（用户不存在或密码错误）  
    #### 2-2.注册  
    - 根据json中的相关信息，新建一个user对象，调用userModel中的insert方法插入到数据库中  
    #### 2-3.注销  
    - 加锁，删掉userConnMap中的对应的id的TCP连接信息  
    - 调用userModel中的updateState更新用户的状态信息  
    #### 2-4.客户端异常退出  
    - 加锁，删掉对应的连接信息  
    - 取消redis的订阅通道  
    - 更新当时客户端登陆的用户的状态信息
    #### 2-5.添加好友  
    - 解析json中的字段，调用friendModel的insert方法
    #### 2-6.创建群组  
    - 调用groupModel中的createGroup和addGroup方法，创建群，然后将当前创建人的权限改为创建者
    #### 2-7.添加群组  
    - 同理
    #### 2-8.好友聊天  
    - 查询对方id是否在线（首先在本服务器中的userConnMap中查，如在，则将请求的json消息推给该用户即可；如果不在，需要在数据库中查是否在线，如果在线则在redis中发布对方id的channel， 推送消息）  
    - 如果不在线，直接存为离线消息
    #### 2-9.群组聊天  
    - 类似好友聊天，区别在于需要获取当前群id的所有user，然后根据他们在线与否，选择直接推送还是存为离线消息。（当不在本机找不到时，通过redis推送）
    #### 2-10.从redis中获取订阅消息  
    - 订阅的channel有消息了，查看对应id的TCP连接，然后通过绑定的回调函数处理(service直接把Json推送到对应的客户端)  

## 四. 项目目录结构
```
bin：可执行文件，这里放的是example编译好的二进制文件
build：项目编译文件
lib：项目库文件
src：源文件
test：放的是一些util和net的测试代码
example：框架代码使用范例
CMakeLists.txt：顶层的cmake文件
README.md：项目自述文件
autobuild.sh：一键编译脚本
```
- `autobuild.sh`是项目的一键编译脚本  
- 项目最终会编译完成后提供给用户的是一个个静态库和头文件目录，都放在lib下面  
  - `libmprpc.a`：rpc库
  - `include目录`

## 五. 项目分支
目前项目有三个分支：

  - `master`，该分支是基于muduo网络库作为通信基础的，所以在使用该分支代码的时候还需要安装muduo，安装方法参考链接: [https://blog.csdn.net/QIANGWEIYUAN/article/details/89023980](https://blog.csdn.net/QIANGWEIYUAN/article/details/89023980)  
  - `master_net`，该分支网络库是自己基于reactor模式开发的一套单线程c++网络库，部分代码参考muduo，可以供学习muduo。
  - `release/1.0`，基于master分支拉出来的一个稳定分支。


