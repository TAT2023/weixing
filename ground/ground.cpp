#include <iostream>  
#include <arpa/inet.h>  
#include <unistd.h>  
#include <string.h>  

#define PORT 7777   ////distributor暴露给ground的端口号


#define DISTRIBUTORIP "127.0.0.1"

using namespace std;

int main() {  
    int sock = 0;  
    struct sockaddr_in distributor_addr;  
    char buffer[1024] = {0};  

    // 创建 socket  
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {  
        std::cerr << "Socket creation error" << std::endl;  
        return -1;  
    }  

    distributor_addr.sin_family = AF_INET;  
    distributor_addr.sin_port = htons(PORT);  

    // 转换 IPv4 和 IPv6 地址从文本到二进制  
    if (inet_pton(AF_INET, DISTRIBUTORIP, &distributor_addr.sin_addr) <= 0) {    //这里的ip地址是distributor的ip地址
        std::cerr << "Invalid address/ Address not supported" << std::endl;  
        return -1;  
    }

    while(true){
        char linkStart=0;
        cout<<"是否开始连接distributor(y|n): "<<endl;

        cin>>linkStart;

        if(linkStart=='y'){
            // 连接到distributor  
            if (connect(sock, (struct sockaddr *)&distributor_addr, sizeof(distributor_addr)) < 0) {  
                std::cerr << "Connection failed" << std::endl;  
                continue;  
            }
            else{
                cout<<"连接distributor成功!"<<endl;
                break;
            }
        }
        

    }

    

    while(true){

        std::string message;
        std::cout << "请输入要发送的消息: ";
        std::cin >> message;
        
        send(sock, message.c_str(), message.size(), 0);

    }

    // 关闭 socket  
    close(sock);  
    return 0;  
}