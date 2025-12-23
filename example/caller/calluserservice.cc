#include "mprpcapplication.h"
#include "user.pb.h"
#include "friend.pb.h"

#include <iostream>

void UserLoginServiceTest()
{
     // //--远程调用Login
    fixbug::UserServiceRpc_Stub stub(new MprpcChannel());
    fixbug::LoginRequest request;
    request.set_name("zhang san");
    request.set_pwd("123456");

    fixbug::LoginResponse response;

    //执行rpc远程调用
    stub.Login(nullptr, &request, &response,nullptr);

    // 一次rpc远程调用结束
    if(response.result().errcode() == 0){
        std::cout << "rpc login response success: " << response.success() << std::endl;
    }else {
        std::cout << "rpc login response error: " << response.result().errmsg() << std::endl;
    }
}

void UserRegisterServiceTest()
{
     // //--远程调用Login
    fixbug::UserServiceRpc_Stub stub(new MprpcChannel());
    fixbug::RegisterRequest request;
    request.set_id(1);
    request.set_name("zhang san");
    request.set_pwd("123456");

    fixbug::RegisterResponse response;

    //执行rpc远程调用
    stub.Register(nullptr, &request, &response,nullptr);

    // 一次rpc远程调用结束
    if(response.result().errcode() == 0){
        std::cout << "rpc register response success: " << response.success() << std::endl;
    }else {
        std::cout << "rpc register response error: " << response.result().errmsg() << std::endl;
    }
}

void GetFriendListServiceTest()
{
    // //--远程调用Login
    fixbug::GetFriendListSerciveRpc_Stub stub(new MprpcChannel());
    fixbug::GetFriendListRequest request;
    request.set_id(1);
    fixbug::GetFriendListResponse response;
    //执行rpc远程调用
    stub.GetFriendList(nullptr,&request,&response,nullptr);
    // 一次rpc远程调用结束
    if(response.result().errcode() == 0){
        std::cout << "rpc GetFriendsList response success!" << std::endl;
        int size = response.friends_size();
        for(int i = 0;i<size;++i){
            std::cout << "index:" << (i+1) << " name:" << response.friends(i) << std::endl;
        }
    }else {
        std::cout << "rpc register response error: " << response.result().errmsg() << std::endl;
    }
}

int main(int argc, char** argv)
{
    //调用框架的初始化操作
    MprpcApplication::Init(argc,argv);
    UserRegisterServiceTest();
    UserLoginServiceTest();
    GetFriendListServiceTest();
    // 演示调用远程发布的rpc方法Register
    return 0;
}