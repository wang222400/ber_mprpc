#include "mprpcapplication.h"
#include "user.pb.h"


int main(int argc, char** argv)
{
    //调用框架的初始化操作
    MprpcApplication::GetInstance().Init(argc,argv);
    
    // //--远程调用Login
    // fixbug::UserServiceRpc_Stub
}