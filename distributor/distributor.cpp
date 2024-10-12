
#include <iostream>  
#include <arpa/inet.h>  
#include <unistd.h>  
#include <string.h>  


#define MYPORT 7777   //distributor暴露给ground的端口号

#define EXECUTORPORT 6666   //exector暴露给distributor的端口号   

using namespace std;


#define EXECUTOR1 "127.0.0.1"    //executor1的ip地址

#define EXECUTOR2 "127.0.0.1"    //executor2的ip地址，根据实际情况修改

int main() {  

    //distributor有三种socket，一个监听socket，一个是和ground通信的socket, 一个是和executor通信的socket

    int distributor_listen_fd, new_socket;  //new_socket是用来和ground通信的socket
    struct sockaddr_in address;  
    int opt = 1;  
    int addrlen = sizeof(address);  
    char buffer[1024] = {0};  
    
    // 创建 socket 文件描述符  
    if ((distributor_listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {  
        std::cerr << "Socket creation error" << std::endl;  
        return -1;  
    }  

    // 绑定 socket  
    if (setsockopt(distributor_listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {  
        std::cerr << "Set socket options error" << std::endl;  
        return -1;  
    }  
    
    address.sin_family = AF_INET;  
    address.sin_addr.s_addr = INADDR_ANY;  
    address.sin_port = htons(MYPORT);  
    
    if (bind(distributor_listen_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {  
        std::cerr << "Binding failed" << std::endl;  
        return -1;  
    }  
    
    // 监听连接  
    if (listen(distributor_listen_fd, 3) < 0) {  
        std::cerr << "Listening failed" << std::endl;  
        return -1;
    }  
    std::cout << "distributor监听已启动，等待连接..." << std::endl;


    // new_socket接受连接  
    if ((new_socket = accept(distributor_listen_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {  
        std::cerr << "Accept failed" << std::endl; 
        return -1;
    }

    
    

    while (true) { 

        

        // 接收数据  
        int valread = read(new_socket, buffer, 1024); 
        
        if(valread>0){
            cout<<"收到消息: "<<buffer<<endl;

            if(strcmp(buffer, "Photo")==0){  //地面要求照相, 通知executor

                cout<<"地面要求照相"<<endl;

                int whodo=rand()%2;    //随机选择一个executor

                char executor_ip[20]={0};
                if(whodo){
                    strcpy(executor_ip, EXECUTOR1);
                }else{
                    strcpy(executor_ip, EXECUTOR2);
                }

                
                int sock = 0;     //和executor通信的socket

                struct sockaddr_in executor_addr;  

                // 创建 socket  
                if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {  
                    std::cerr << "Socket creation error" << std::endl;  
                    return -1;  
                }  

                executor_addr.sin_family = AF_INET;  
                executor_addr.sin_port = htons(EXECUTORPORT);  

                // 转换 IPv4 和 IPv6 地址从文本到二进制  

                if (inet_pton(AF_INET,executor_ip, &executor_addr.sin_addr) <= 0) {  
                    std::cerr << "Invalid address/ Address not supported" << std::endl;  
                    return -1;
                }  

                // 连接到executor  
                if (connect(sock, (struct sockaddr *)&executor_addr, sizeof(executor_addr)) < 0) {  
                    std::cerr << "Connection failed" << std::endl;  
                    return -1;  
                }

                send(sock, "Photo", 5, 0);    //通知executor要照相

                close(sock);

            }
        }
    }  

    // 关闭 socket  
    close(new_socket);
    close(distributor_listen_fd);  
    return 0;  
}