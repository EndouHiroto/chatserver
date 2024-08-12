#include "chatserver.hpp"
#include "chatservice.hpp"

#include<iostream>
#include<signal.h>
#include<stdlib.h>
#include<errno.h>
#include<stdint.h>
#include<climits>
using namespace std;

//处理服务器ctrl+c结束后，重置User的状态信息
void resetHandler(int)
{
    ChatService::instance()->reset();
    exit(0);
}

int main(int argc,char **argv)
{
    if(argc < 3)
    {
        cerr << "command invalid!example: ./ChatServer 127.0.0.1 6000" << endl;
        exit(-1);
    }

    //解析通过命令行参数传递的ip和port
    char *ip = argv[1];
    /*
        uint16_t表示一个无符号的 16 位整型,存储的值的范围是 0 到 65535。
        atoi函数是将字符串转变为整数，但并不检查转换结果是否超出了 uint16_t 的范围。
        如果 argv[2] 包含的数值超出了 uint16_t 能表示的范围（即大于 65535），atoi 函数将返回一个超出范围的整数。
        但这个整数在赋值给 port 时会发生隐式类型转换，可能导致数据丢失（即高位的值会被截断）。
        这是一个需要留意的点。
    */
    //uint16_t port = atoi(argv[2]);

    //为了避免以上问题，使用更安全的转换函数，strtol 或 strtoul，并检查转换结果是否在 uint16_t 的有效范围内
    char *end;
    long long_port = strtoul(argv[2],&end,10);  //使用base10转换
    if (*end != '\0' || long_port > UINT16_MAX)
    {
        // 处理错误：值超出范围，不是有效的数字，或者字符串中有非数字字符
        cerr << "The value is out of range, not a valid number, or there are non numeric characters in the string" << endl;
        exit(-1);
    }
    else
    {
        uint16_t port = (uint16_t)long_port;
        // 使用 port ...
        signal(SIGINT,resetHandler);

        EventLoop loop;
        InetAddress addr(ip, port);
        ChatServer server(&loop, addr, "ChatServer");

        server.start();
        loop.loop();

        return 0;
    }
}
