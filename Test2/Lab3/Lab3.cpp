//读者优先的读写程序
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define READ 9
#define WRITE 5

int readercount=0;  //读者数量
pthread_mutex_t wmutex;  //文件访问互斥信号量
pthread_mutex_t rmutex;  //读者互斥信号量

void *write(void *args)
{
    char *id=(char *)args;  //写者编号

    sleep(1);

    pthread_mutex_lock(&wmutex);
    printf("写者%c写文件!\n",*id);
    pthread_mutex_unlock(&wmutex);

}

void *read(void *args)
{
    char *id=(char *)args;  //读者编号
    
    sleep(1.5);
    pthread_mutex_lock(&rmutex);
    if(readercount==0)  //第一个读者和写着竞争写互斥信号量
    {
        pthread_mutex_lock(&wmutex);
    }
    readercount++;
    pthread_mutex_unlock(&rmutex);
    //读
    printf("读者%c读取文件!\n",*id);

    pthread_mutex_lock(&rmutex);
    readercount--;
    if(readercount==0)
    {
        pthread_mutex_unlock(&wmutex);
    }
    pthread_mutex_unlock(&rmutex);
}

int main()
{
    pthread_t wid[WRITE],rid[READ];
    char r[READ]={'1','2','3','4','5','6','7','8','9'};
    char w[WRITE]={'1','2','3','4','5'};

    pthread_mutex_init(&rmutex,NULL);
    pthread_mutex_init(&wmutex,NULL);
  
    for(int i=0;i<READ;i++)
    {
        pthread_create(&rid[i],NULL,read,(void*)(&r[i]));
    }

    for(int i=0;i<WRITE;i++)
    {
        pthread_create(&wid[i],NULL,write,(void*)(&w[i]));
    }

    for(int i=0;i<WRITE;i++)
    {
        pthread_join(wid[i], NULL);
    }
    for(int i=0;i<READ;i++)
    {
        pthread_join(rid[i], NULL);
    }
    return 0;
}