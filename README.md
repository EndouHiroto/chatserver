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
利用muduo库写网络模块的代码非常简洁，只需要在定义的ChatServer中定义muduo的TcpServer和EventLoop，然后注册好onConnection和onMessage两个回调，就可以很方便的起一个服务。  
onConnection是建立连接。  
onMessage则是根据不同的json消息中的msgid字段，来在业务层调用不同的回调函数来处理请求。
  
  ### 2.业务模块  
基于单例模式设计一个负责处理业务的ChatService类，成员：所有的数据库操作类的对象，一个用来映射msgid到回调函数的map，一个互斥锁保证map的访问安全，还有一个存储在线用户的通信连接的map，redis的操作对象。然后所有的业务处理函数都在构造函数中注册到了对应的map上。

下载地址:https://pan.baidu.com/s/1yPl1aNGuC7OVabs5icw1Ag?pwd=92xe 提取码: 92xe
```
1、下载zookeeper-3.4.10.tar.gz解压后进入zookeeper-3.4.10
2. cd zookeeper-3.4.10/conf
3. mv zoo_sample.cfg zoo.cfg
4. 进入bin目录，启动zkServer， ./zkServer.sh start
5. 可以通过netstat查看zkServer的端口，在bin目录启动zkClient.sh链接zkServer，熟悉zookeeper怎么组织节点
```
- **zk的原生开发API（c/c++接口）**

进入上面解压目录src/c下面，zookeeper已经提供了原生的C/C++和Java API开发接口，需要通过源码编译生成，过程如下：
```
~/package/zookeeper-3.4.10/src/c$ sudo ./confifigure
~/package/zookeeper-3.4.10/src/c$ sudo make
~/package/zookeeper-3.4.10/src/c$ sudo make install
```
主要关注zookeeper怎么管理节点，zk-c API怎么创建节点，获取节点，删除节点以及watcher机制的API编程。
- zk客户端常用命令
  
  ``ls、get、create、set、delete``

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


