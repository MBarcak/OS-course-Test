// 读者优先的读写进程
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define READ 9
#define WRITE 5

int readercount,writercount=0;
pthread_mutex_t S;  //第一个写者和读者竞争的互斥信号量
pthread_mutex_t mutex;  //写者互斥信号量
pthread_mutex_t wmutex;  //文件访问互斥信号量
pthread_mutex_t rmutex;  //读者互斥信号量

void *read(void *args)
{
    char *id=(char *)args;  //读者编号
    
    //srand((unsigned)time(NULL));
    sleep(1.5);

    pthread_mutex_lock(&S);
    pthread_mutex_lock(&rmutex);
    if(readercount==0)
    {
        pthread_mutex_lock(&wmutex);
    }
    readercount++;
    pthread_mutex_unlock(&rmutex);
    pthread_mutex_unlock(&S);
    //读
    printf("读者%c读取文件!\n",*id);

    pthread_mutex_lock(&rmutex);
    readercount--;
    if(readercount==0)
    {
        pthread_mutex_unlock(&wmutex);
    }
    pthread_mutex_unlock(&rmutex);

    //sleep(100);
    //pthread_exit((void *) 0);  //销毁线程
}

void *write(void *args)
{
    char *id=(char *)args;  //写者编号

    //srand((unsigned)time(NULL));
    sleep(1);

    pthread_mutex_lock(&mutex);
    if(writercount==0)
    {
        pthread_mutex_lock(&S);
    }
    writercount++;
    pthread_mutex_unlock(&mutex);
    pthread_mutex_lock(&wmutex);
    //写
    printf("写者%c写文件!\n",*id);
    pthread_mutex_unlock(&wmutex);
    pthread_mutex_lock(&mutex);
    writercount--;
    if(writercount==0)
    {
        pthread_mutex_unlock(&S);
    }
    pthread_mutex_unlock(&mutex);


    //pthread_exit((void *) 0);  //销毁线程
}

int main()
{
    pthread_t wid[WRITE],rid[READ];
    char r[READ]={'1','2','3','4','5','6','7','8','9'};
    char w[WRITE]={'1','2','3','4','5'};

    pthread_mutex_init(&S,NULL);
    pthread_mutex_init(&mutex,NULL);
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