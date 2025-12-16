#include "mprpcapplication.h"

#include <iostream>
#include <unistd.h>
#include <string>
void ShowArgsHelp()
{
    std::cout<< "format:  command -i <configure>" << std::endl;
}

void MprpcApplication::Init(int argc,char** argv)
{
    if(argc < 2){
        ShowArgsHelp();
        exit(EXIT_FAILURE);
    }

    int c=0;
    std::string config_file;
    while((c = getopt(argc, argv,"i:")) != -1){
        switch(c)
        {

        }
    }
}

MprpcApplication& MprpcApplication::GetInstance()
{
    static MprpcApplication instance;
    return instance;
}