#include"httpd.h"

static void usage(const char* arg)
{
    printf("correct usage : %s [ip] [port]\n", arg);
}

int main(int argc, char* argv[])
{
    //if (argc != 3)
    //{
    //    usage(argv[0]);
    //    return 1;
    //}
    
    int port = 8080;
    char ip[] = "192.168.1.105";

    int listen_sock = start_up(ip, port);
    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    
    while (1)
    {
        int new_sock = accept(listen_sock, (struct sockaddr*)&client, &len);
        if (new_sock < 0)
        {
            perror("accept failure"); 
            continue;
        }
        printf("client [%s][%d] connects the server\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
        
        pthread_t pid;
        int flag = pthread_create(&pid, NULL, handle_request, (void*)new_sock);
        if (flag < 0)
        {
            perror("pthread_create failure"); 
            continue;
        }
    }
    
    close(listen_sock);
    return 0;
}
