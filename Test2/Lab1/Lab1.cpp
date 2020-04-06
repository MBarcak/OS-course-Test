#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define THREAD_NUM 3  //生产者消费者数量
#define BUF_NUM 8  //缓存区大小

int in = 0;   // 生产在者指针
int out = 0; // 消费者指针
int buf[BUF_NUM] = { 0 };  // 缓冲区
sem_t empty; // 空缓存区信号量
sem_t full;   // 满缓存区信号量
pthread_mutex_t mutexP; // 生产者互斥信号量
pthread_mutex_t mutexC; // 消费者互斥信号量

void *product(void *args)
{
    int i=5;  //每个生产者生产5个产品
    int *id=(int*)args;

    while(i)
    {
        sem_wait(&empty);  //等待空信号量
        pthread_mutex_lock(&mutexP);  //等待生产者互斥信号量
        buf[in]=1;  //生产数据放入缓存区，这里用1指代
        printf("生产者%d：生产数据放入缓存区%d中！\n",*id,in);
        in=(in+1)%BUF_NUM;  //指针前进
        pthread_mutex_unlock(&mutexP);  //释放生产者互斥信号量
        sem_post(&full);  //释放满缓存区信号量
        i--;
    }

}

void *customer(void *args)
{
    int i=5;  //每个消费者者消费5个产品
    int *id=(int*)args;

    while(i)
    {
        sem_wait(&full);  //等待满信号量
        pthread_mutex_lock(&mutexC);  //等待消费者互斥信号量
        buf[out]=0;  //消费数据移出缓存区，这里用0指代
        printf("消费者%d：消费数据从缓存区%d中！\n",*id,out);
        out=(out+1)%BUF_NUM;  //指针前进
        pthread_mutex_unlock(&mutexC);  //释放消费者互斥信号量
        sem_post(&empty);  //释放空缓存区信号量
        i--;
    }

}

int main()
{
    pthread_t Ptid[THREAD_NUM];  //生产者标识
    pthread_t Ctid[THREAD_NUM];  //消费者标识
    int id[THREAD_NUM]={1,2,3};  //生产者消费者编号
    int ret[THREAD_NUM];
    //初始化信号量
    sem_init(&empty, 0, BUF_NUM);
	sem_init(&full, 0, 0);
    //初始化互斥信号量
    pthread_mutex_init(&mutexP, NULL);
    pthread_mutex_init(&mutexC, NULL);

    for(int i=0;i<THREAD_NUM;i++)
    {
        ret[i]=pthread_create(&Ptid[i], NULL, product, (void*)&id[i]);
        if (ret[i] != 0)
		{
			printf("生产者%d号线程创建失败! \n", i+1);
			exit(0);
		}
    }

    for(int i=0;i<THREAD_NUM;i++)
    {
        ret[i]=pthread_create(&Ctid[i], NULL, customer, (void*)&id[i]);
        if (ret[i] != 0)
		{
			printf("消费者%d号线程创建失败! \n", i+1);
			exit(0);
		}
    }

    for(int i=0;i<THREAD_NUM;i++)
	{
		pthread_join(Ptid[i],NULL);
		pthread_join(Ctid[i],NULL);
	}
    return 0;
}