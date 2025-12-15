#include "test.pb.h"
#include <iostream>
#include <string>

using namespace fixbug;

int main(){
    // LoginResponse rsp;
    // ResultCode *rc = rsp.mutable_result_code();
    // rc->set_errcode(1);
    // rc->set_errmg("登录处理失败了");
    GetFriendListsResponse rsp;
    rsp.mutable_result_code()->set_errcode(0);
    User* user1 = rsp.add_friend_list();
    user1->set_age(70);
    user1->set_name("XXX");
    user1->set_sex(User::MAN);

    User* user2 = rsp.add_friend_list();
    user2->set_age(18);
    user2->set_name("XXX");
    user2->set_sex(User::WOMAN);

    std::cout << rsp.friend_list(0).age() << std::endl;
    std::cout << rsp.friend_list(0).name() << std::endl;
    std::cout << rsp.friend_list(1).sex() << std::endl;
    return 0;
}

int main1()
{
    //封装login请求对象的数据
    LoginRequest req;
    req.set_name("zhang san");
    req.set_pwd("123456");

    std::string send_str;

    //对象数据序列化
    if(req.SerializeToString(&send_str)){
        std::cout<<send_str.c_str()<<std::endl;
    }

    //从send_str反序列化一个login请求对象
    LoginRequest reqB;
    if(reqB.ParseFromString(send_str)){
        std::cout << reqB.name() << std::endl;
        std::cout << reqB.pwd() << std::endl;
    }


    return 0;
}