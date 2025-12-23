#include "rpcprovider.h"
#include "mprpcapplication.h"


//这里是框架给外部使用的，可以发布rpc方法的函数接口
void RpcProvider::NotifyService(google::protobuf::Service* servicer)
{
    ServiceInfo service_info;
    // 获取服务对象的描述信息
    const google::protobuf::ServiceDescriptor* pserviceDesc = servicer->GetDescriptor();
    //获取服务的名字
    std::string serivce_name = pserviceDesc->name();
    //获取服务的方法数量
    int methodCount = pserviceDesc -> method_count();
    for(int i =0;i<methodCount;++i){
        const google::protobuf::MethodDescriptor* pmethodDesc = pserviceDesc->method(i);
        std::string method_name = pmethodDesc->name();
        service_info.m_methodMap.insert({method_name,pmethodDesc});
    }
    service_info.m_service = servicer;
    m_serviceMap.insert({serivce_name,service_info});
}

void RpcProvider::Run()
{
    std::string ip = MprpcApplication::GetConfig().Load("rpcserverip");
    uint16_t port = stoi(MprpcApplication::GetConfig().Load("rpcserverport"));
    std::string zk_host = MprpcApplication::GetConfig().Load("zookeeperip");
    std::string zk_port = MprpcApplication::GetConfig().Load("zookeeperport");


    //创建InetAddress对象,调查发现需要传入string ip和unit_16 port
    muduo::net::InetAddress adress(ip,port);
    //创建TcpServer对象
    muduo::net::TcpServer server(&m_eventLoop,adress,"RpcProvider");
    //绑定连接回调和消息读写回调方法(分离网络代码和业务代码)
    server.setConnectionCallback(std::bind(&RpcProvider::OnConnection, this, std::placeholders::_1));
    //绑定读写的回调
    server.setMessageCallback(std::bind(&RpcProvider::onMessage,this,std::placeholders::_1,
        std::placeholders::_2,std::placeholders::_3));

    //把当前rpc节点上要发布的服务全部注册到zk上面，让rpc client可以从zk上发现服务
    //session timeout   30s     zkclient 网络I/O线程  1/3 * timeout 时间发送ping消息
    m_zkClient.Start(zk_host,zk_port);
    // service_name为永久性节点    method_name为临时性节点
    for(auto &sp : m_serviceMap)
    {
        // /service_name /UserServiceRpc
        std::string service_path = "/" + sp.first;
        m_zkClient.Create(service_path.c_str(),nullptr,0);
        for(auto &mp : sp.second.m_methodMap)
        {
            // /service_name/method_name /UserServiceRpc/Login 存储当前这个rpc服务节点主机的ip和port
            std::string method_path = service_path + "/" + mp.first;
            char method_path_data[128] = {0};
            sprintf(method_path_data,"%s:%d",ip.c_str(),port);
            // ZOO_EPHEMERAL表示znode是一个临时性节点
            m_zkClient.Create(method_path.c_str(),method_path_data,strlen(method_path_data),ZOO_EPHEMERAL);
        }
    }

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
    if(!conn->connected()){
        conn->shutdown();
    }
}
void RpcProvider::onMessage(const muduo::net::TcpConnectionPtr& conn,muduo::net::Buffer* buffer,muduo::Timestamp)
{
    std::string recv_buf = buffer->retrieveAllAsString();
    // 读取4字节的头部大小
    uint32_t header_size = 0;
    recv_buf.copy((char*)&header_size,4,0);
    // 根据头部大小获取头部内容
    std::string rpc_header_str = recv_buf.substr(4,header_size);
    ber_mprpc::RpcHeader rpcHeader;
    std::string service_name;
    std::string method_name;
    uint32_t args_size;
    if(rpcHeader.ParseFromString(rpc_header_str)){
        // 反序列化成功
        service_name = rpcHeader.service_name();
        method_name = rpcHeader.method_name();
        args_size = rpcHeader.args_size();
    }else{
        std::cout << "rpc_header_str: " << rpc_header_str << ", parse error!" << std::endl;
        return;
    }
    std::string args_str = recv_buf.substr(4+header_size,args_size);

    // 获取service对象和method对象
    auto it = m_serviceMap.find(service_name);
    if(it == m_serviceMap.end()){
        std::cout << service_name << " is not exist! " << std::endl;
        return;
    }
    auto mit =  it->second.m_methodMap.find(method_name);
    if (mit == it->second.m_methodMap.end()) {
        std::cout << service_name << ":" << method_name <<  " is not exist! " << std::endl;
        return;
    }
    google::protobuf::Service* service = it->second.m_service;
    const google::protobuf::MethodDescriptor* method = mit->second;

    // 生成rpc方法调用请求request和响应response参数
    google::protobuf::Message* request = service->GetRequestPrototype(method).New();
    if(!request->ParseFromString(args_str)){
        std::cout << "request parse error, content: " << args_str << std::endl;
        return;
    }
    google::protobuf::Message* response = service->GetResponsePrototype(method).New();
    // 给下面的method方法的调用。绑定一个Closure的回调函数
    google::protobuf::Closure* done = google::protobuf::NewCallback<RpcProvider, 
                                                                    const muduo::net::TcpConnectionPtr&, 
                                                                    google::protobuf::Message*>
                                                                    (this, &RpcProvider::SendRpcResponse, conn, response);


    service->CallMethod(method,nullptr,request,response,done);
}
// Closure的回调操作，用于序列化rpc的响应和网络发送
void RpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr& conn, google::protobuf::Message* response){
    std::string response_str;
    if (response->SerializeToString(&response_str)) {
        conn->send(response_str);
        // 模拟http的短链接服务，由rpcprovider主动断开链接
        conn->shutdown();
    } else {
        std::cout << "serialize response_str error!" << std::endl;
    }

    conn->shutdown();
}