#include "rpcprovider.h"
#include "mprpcapplication.h"
#include <string>
#include <functional>
#include <iostream>
void RpcProvider::NotifyService(google::protobuf::Service* servicer)
{

}

void RpcProvider::Run()
{
    std::string ip = MprpcApplication::GetConfig().Load("rpcserverip");
    uint16_t port = stoi(MprpcApplication::GetConfig().Load("rpcserverport"));
    //创建InetAddress对象,调查发现需要传入string ip和unit_16 port
    muduo::net::InetAddress adress(ip,port);
    //创建TcpServer对象
    muduo::net::TcpServer server(&m_eventLoop,adress,"RpcProvider");
    //绑定连接回调和消息读写回调方法(分离网络代码和业务代码)
    server.setConnectionCallback(std::bind(&RpcProvider::OnConnection, this, std::placeholders::_1));
    //绑定读写的回调
    server.setMessageCallback(std::bind(&RpcProvider::onMessage,this,std::placeholders::_1,
        std::placeholders::_2,std::placeholders::_3));
    //设置muduo库的线程数量
    server.setThreadNum(4);
    
    std::cout << "RpcProvider start service ip at:"<<ip<<std::endl;
    std::cout << "RpcProvider start service port at:"<<port<<std::endl;

    //启动网络服务
    server.start();
    m_eventLoop.loop();
}
//新的socket连接回调
void RpcProvider::OnConnection(const muduo::net::TcpConnectionPtr& conn)
{

}
void RpcProvider::onMessage(const muduo::net::TcpConnectionPtr&,muduo::net::Buffer*,muduo::Timestamp)
{

}