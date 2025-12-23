#ifndef MPRPCCONTROLLER_H
#define MPRPCCONTROLLER_H
#include <google/protobuf/service.h>
#include <string>

class MprpcController : public google::protobuf::RpcController{
public:
    MprpcController();
    ~MprpcController(){}

    void Reset();
    bool Failed() const;
    std::string ErrorText() const;
    void SetFailed(const std::string& reason);

    //以下方法在本框架中暂未给出具体实现
    void StartCancel();
    bool IsCanceled() const;
    void NotifyOnCancel(google::protobuf::Closure* callback);


private:
    bool m_failed; //记录错误状态
    std::string m_errText; //记录错误信息
};

#endif