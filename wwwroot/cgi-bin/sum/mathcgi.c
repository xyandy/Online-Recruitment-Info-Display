/*************************************************************************
	> File Name: mathcgi.c
	> Author: xuyang
	> Mail: xy913741894@gmail.com 
	> Created Time: 2017-07-07 15:46:59
 ************************************************************************/

#include<stdio.h>
#include<string.h>
#include<strings.h>
#include<stdlib.h>


int main()
{
    printf("sum is :\n");
    
    if (strcasecmp("GET", getenv("METHOD")) == 0)
    {
        char* s = getenv("QUERY_STR");
        //printf("s=%s, size=%d\n", s, strlen(s));    
        
        char* datas[3]; 
        char buf[1024] = {0};
        strcpy(buf, s);
        int i = 0;
        int j = 1;
        while (buf[i] != '\0')
        {
            if (buf[i] == '=')
                datas[j++] = &buf[i+1];
            else if (buf[i] == '&')
                buf[i] = '\0';
            i++;
        }
        
        int data1 = atoi(datas[1]);
        int data2 = atoi(datas[2]);
        printf("%d + %d = %d\n", data1, data2, data1 + data2);
    }
    else 
    {

    }
    
    return 0;
}
