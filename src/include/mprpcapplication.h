#ifndef MPRPCAPPLICATION_H
#define MPRPCAPPLICATION_H

//框架的基础类，负责框架的初始化操作
class MprpcApplication{
public:
    void Init(int argc,char** argv);

    static MprpcApplication& GetInstance();

private:
    MprpcApplication(){}
    MprpcApplication(const MprpcApplication&) = delete;
    MprpcApplication(MprpcApplication&&) = delete;
    MprpcApplication& operator=(const MprpcApplication&) = delete;
    MprpcApplication& operator=(MprpcApplication&&) = delete;
};

#endif