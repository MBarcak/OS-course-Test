//哲学家进餐问题
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM 5  //筷子和哲学家数量

pthread_mutex_t chopstick[NUM];  //筷子互斥信号量
pthread_mutex_t mutex;  //拿筷子互斥信号量

void *philosopher(void *args)
{
    char *id=(char *)args;  //哲学家编号
    int int_id=*id;
    int_id-='0';
    int i=5; //每个哲学家进行五次思考吃饭

    while(i)
    {
        printf("%c号哲学家正在思考......\n",*id);
        sleep(1);  //思考
        pthread_mutex_lock(&mutex);  //尝试拿筷子
        {
            pthread_mutex_lock(&chopstick[int_id]);  //左手筷子
            pthread_mutex_lock(&chopstick[(int_id+1)%NUM]);  //右手筷子
        }
        pthread_mutex_unlock(&mutex); 
        //吃饭
        printf("%c号哲学家正在吃饭......\n",*id);
        sleep(2);
        pthread_mutex_unlock(&chopstick[int_id]);  //放左手筷子
        pthread_mutex_unlock(&chopstick[(int_id+1)%NUM]);  //放右手筷子
        i--;
    }
}

int main()
{
    pthread_t tid[5]; //5个哲学家
    char id[NUM]={'1','2','3','4','5'};

    pthread_mutex_init(&mutex,NULL);
	for(int i = 0; i < 5; ++i)
	{
		pthread_mutex_init(&chopstick[i],NULL);
		pthread_create(&tid[i],NULL,philosopher,(void*)&id[i]);
	}
 
	for(int i = 0; i < 5; ++i)
	{
		pthread_join(tid[i],NULL);
    }
}