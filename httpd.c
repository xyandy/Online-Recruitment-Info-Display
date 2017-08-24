#include"httpd.h"

int start_up(const char* ip, int port)
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("socket failure");
        exit(-1);
    }

    int opt = 1;
    int flag = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (flag < 0)
    {
        perror("setsockopt failure");
        exit(-2);
    }
    struct sockaddr_in local;
    local.sin_family = AF_INET;
    local.sin_port = htons(port);
    inet_aton(ip, &local.sin_addr);
    flag =  bind(sock, (struct sockaddr*)&local, sizeof(local));
    if (flag < 0)
    {
        perror("bind failure");
        exit(-3);
    }
    
    flag = listen(sock, 10);
    if (flag < 0)
    {
        perror("listen failure");
        exit(-4);
    }
    return sock; 
}

//读取一行，考虑\n, \r\n, \r
static int read_line(int sock, char* buf, int size)
{
    int len = 0;

    char c = 0;
    int flag = 0;
    while (c != '\n' && len < size)
    {
        flag = recv(sock, &c, 1, 0);
        if (flag > 0)
        {
            if (c == '\r')
            {
                flag = recv(sock, &c, 1, MSG_PEEK);
                if (c == '\n')
                    recv(sock, &c, 1, 0);
                else
                    c = '\n';
            }

            buf[len++] = c;
        }
        else
        {
            break;
        }
    }
    buf[len] = '\0';    
    return len;
}

static void drop_head(int sock)
{
    char buf[BUF_SIZE] = {0};
    while (1)
    {
        memset(buf, 0, sizeof(buf));
        read_line(sock ,buf, sizeof(buf));
        if (strcmp(buf, "\n") == 0)
            break;
    }
}

void echo_www(int sock, char* path, int size)
{
    int fd = open(path, O_RDONLY);
    if (fd < 0)
    {
        not_found(sock);
        return;
    }
    const char* msg1 = "HTTP/1.0 200 OK\r\n";
    const char* msg2 = "\r\n";
    send(sock, msg1, strlen(msg1), 0);
    send(sock, msg2, strlen(msg2), 0);
    
    if (sendfile(sock, fd, NULL, size) < 0) 
    {
        not_found(sock);
    }
    close(fd);
}

void exe_cgi(int sock, char* method, char* path, char* query_str)
{
    //判断是get，post方法
    char method_env[BUF_SIZE] = {0};
    char query_str_env[BUF_SIZE] = {0};
    int content_size = 0;

    if (strcasecmp(method, "GET") == 0)
    {
        sprintf(method_env, "METHOD=%s", "GET");
        sprintf(query_str_env, "QUERY_STR=%s", query_str);
        putenv(query_str_env);
        drop_head(sock);
    }
    else if (strcasecmp(method, "POST") == 0)
    {
        char buf[BUF_SIZE] = {0};
        
        while (1)
        {
            memset(buf, 0, sizeof(buf));
            read_line(sock ,buf, sizeof(buf));
            if (strncasecmp(buf, "Content-Length:", 15) == 0)
            {
                content_size = atoi(&buf[strlen("Content-Length:")]);
                printf("content_size is %d\n", content_size);
            }
            if (strcmp(buf, "\n") == 0)
                break;
        }
    }
    putenv(method_env);

    const char* msg1 = "HTTP/1.0 200 OK\r\n\r\n";
    send(sock, msg1, strlen(msg1), 0);
    
    //printf("query_str = %s", query_str);

    int input[2];
    int output[2];
    if (pipe(input) < 0 || pipe(output) < 0)
    {
        return;
    }
    

    pid_t pid;
    pid = fork();
    if (pid < 0)
    {
        return;
    }
    else if (pid == 0)
    {
        //针对子进程
        close(input[1]);//用于读，关闭写端        
        close(output[0]);//用于写，关闭读端
        
        dup2(input[0], 0);
        dup2(output[1], 1);

        execl(path, path, NULL);
        exit(0);

        //close(input[0]);
        //close(output[1]);
    }
    else 
    {
        //针对父进程
        close(input[0]);//用于写，关闭读端        
        close(output[1]);//用于读，关闭写端
        if (strcasecmp("POST",method) == 0)
        {
            int i = 0;
            char c = 0;

            for (; i < content_size; ++i)
            {
                recv(sock, &c, 1, 0);
                write(input[1], &c, 1);
            }
        }

        char c = 0;
        while (read(output[0], &c, 1) > 0)
        {
            //printf("c=%c", c);
            send(sock, &c, 1, 0);
        }

        close(input[1]);
        close(output[0]);
        waitpid(pid, NULL, 0);
    }

}
 
void not_found(int client)
{
    char buf[1024];
    sprintf(buf, "HTTP/1.0 404 NOT FOUND\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-Type: text/html\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<HTML><TITLE>Not Found</TITLE>\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<BODY><P>Not Found\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "</BODY></HTML>\r\n");
    send(client, buf, strlen(buf), 0);
}



void* handle_request(void* arg)
{
    pthread_detach(pthread_self());
    
    int new_sock = (int)arg;
    int ret = 0;
    char buf[BUF_SIZE];
    memset(buf, 0, BUF_SIZE);
    //处理第一行
    int len = read_line(new_sock, buf, sizeof(buf)); 
    if (len < 0)
    {
        perror("read_line");
        close(new_sock);
        ret = -1;
        goto end;
    }
    else
    {
        char method[BUF_SIZE/10] = {0};
        char url[BUF_SIZE] = {0};
        char path[BUF_SIZE] = {0}; 
        
        int i = 0;
        while (!isspace(buf[i]) && i < len)
        {
            method[i] = buf[i];
            i++;
        }
        //跳过空格
        while (isspace(buf[i]))
        {
            i++;
        }
        int j = 0;
        while (!isspace(buf[i]) && i < len)
        {
            url[j++] = buf[i++];
        }
        //跳过空格
        //while (isspace(buf[i]))
        //{
        //    i++;
        //}
        //j = 0;
        //while (!isspace(buf[i]) && i < len)
        //{
        //    http[j++] = buf[i++];
        //}        
        int cgi = 0;
        if (strcasecmp(method, "POST") == 0)
        {
            cgi = 1;          
        }
        //处理path
        j = 0;
        while (url[j] != '\0')
        {
            if (url[j] == '?')
            {
                url[j] = '\0';
                cgi = 1; 
                break;
            }
            ++j;
        }
        char* query_str = &url[j+1];

        sprintf(path, "wwwroot%s", url);
        //附加一个index网页
        if (path[strlen(path)-1] == '/')
        {
            strcat(path, "index.html");
        }
       
        ////准备响应
        
        struct stat path_st;
        if (stat(path, &path_st) != 0) //获取文件信息到path_st中
        {
            drop_head(new_sock);
            not_found(new_sock);
            ret =3;
            goto end;
        }
        else
        {
            if (S_ISDIR(path_st.st_mode)) //如果是目录
            {
                strcat(path, "/index.html");        
            }
            else if ((path_st.st_mode & S_IXUSR) || (path_st.st_mode & S_IXGRP) || (path_st.st_mode & S_IXOTH))
            {
                cgi = 1;
            }
            else {}
        }


        if (cgi == 1)
        {
            exe_cgi(new_sock, method, path, query_str);
        }
        else
        {
            //printf("method=%s, url=%s, path=%s, query_str=%s\n", method, url, path, query_str);
            drop_head(new_sock);
            echo_www(new_sock, path, path_st.st_size);
        }
    
    }//else
    

end:
    printf("client quits\n");
    close(new_sock);
    return (void*)ret;
}







