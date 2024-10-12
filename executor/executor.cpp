#include <iostream>  
#include <arpa/inet.h>  
#include <unistd.h>  
#include <string.h>  

#include <wait.h>

#define PORT 6666


#define PHOTOPROGRAMPATH "./photo/photo"

using namespace std;


//每个executor都会有一个expector连接，用于接收distributor的消息


int main() {  

    int exector_listen_fd,new_socket;   //new_socket才是用来和distributor通信的socket

    struct sockaddr_in address;  
    int opt = 1;  
    int addrlen = sizeof(address);  
    char buffer[1024] = {0};  
    
    // 创建 socket 文件描述符  
    if ((exector_listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {  
        std::cerr << "Socket creation error" << std::endl;  
        return -1;  
    }  

    // 绑定 socket  
    if (setsockopt(exector_listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {  
        std::cerr << "Set socket options error" << std::endl;  
        return -1;  
    }  
    
    address.sin_family = AF_INET;  
    address.sin_addr.s_addr = INADDR_ANY;     //监听所有的ip地址
    address.sin_port = htons(PORT); 
    
    if (bind(exector_listen_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {  
        std::cerr << "Binding failed" << std::endl;  
        return -1;  
    }  
    
    

    
    

    while(true){

        // 监听连接  
        if (listen(exector_listen_fd, 3) < 0) {  
            std::cerr << "Listening failed" << std::endl;  
            return -1;  
        }  
        std::cout << "executor监听已启动，等待连接..." << std::endl;  

        // 接受连接  
        if ((new_socket = accept(exector_listen_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {  
            std::cerr << "Accept failed" << std::endl;  
            return -1;  
        }
        
        int len=read(new_socket, buffer, 1024);    //expector只会接收消息，不会发送消息

        if(len<=0){
            cout<<"服务器断开连接"<<endl;
            break;
        }

        cout<<"收到消息: "<<buffer<<endl;

        if(strcmp(buffer, "Photo")==0){  //照相，等等
            int pid=fork();

            if(pid==0){

                //调用照相程序
                execl(PHOTOPROGRAMPATH, NULL);
            }

            else if(pid>0){
                //父进程
                //等待子进程结束

                close(new_socket);    //关闭和distributor的连接
                wait(NULL);  //等待子进程结束再继续接收消息(如果是照相程序结束，那么就可以接收下一个照相请求)

                //当然，这仅仅是executor只能实现一种功能的情况，如果executor有多种功能，主进程不应该等待子进程结束才能继续接收消息
            }
        }

        close(new_socket);

    }

    // 关闭 socket  

    close(exector_listen_fd);
      
    return 0;  
}