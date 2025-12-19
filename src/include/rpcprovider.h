#ifndef RPCPROVIDER_H
#define RPCPROVIDER_H
#include "google/protobuf/service.h"
#include <memory>
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpConnection.h>
//框架提供的专门发布rpc服务的网络对象类
class RpcProvider{
public:
    //这里是框架提供的给外部使用的，可以发布rpc方法的函数接口
    void NotifyService(google::protobuf::Service* servicer);

    //启动rpc服务节点，开始提供rpc远程网络调用服务
    void Run();
private:
    //组合TcpServer
    //std::unique_ptr<muduo::net::TcpServer> m_tcpserverPtr;
    //组合EventLoop
    muduo::net::EventLoop m_eventLoop;

    //新的socket连接回调
    void OnConnection(const muduo::net::TcpConnectionPtr&);
    //消息读写回调
    void onMessage(const muduo::net::TcpConnectionPtr&,muduo::net::Buffer*,muduo::Timestamp);
};

#endif