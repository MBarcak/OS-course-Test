#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <time.h>

#define TNUM 6  //线程数
#define SNUM 3  //资源数

int Available[SNUM]={10,8,6};  //系统现有可用资源
int Max[TNUM][SNUM]={{2,2,3},{6,1,1},{3,2,3},{6,2,4},{3,1,1},{1,1,2}};  //最大资源分配矩阵
int Allocation[TNUM][SNUM]={{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}};  //已分配资源矩阵
int Need[TNUM][SNUM]={{2,2,3},{6,1,1},{3,2,3},{6,2,4},{3,1,1},{1,1,2}};  //需求矩阵
bool Finish[TNUM]={0};

pthread_mutex_t mutex; // 执行互斥信号量
pthread_cond_t cond; 

int safe(int id)
{
    int work[SNUM];  //资源量
    int tmpfinish[TNUM]={0};
    int flag=1;
    int i;

    for(int j=0;j<SNUM;j++)
        work[j]=Available[j];
    while(flag)
    {
        flag=0;
        for(i=0;i<TNUM;i++)
        {
            if(tmpfinish[i]==0&&Finish[i]==0)
            {
                flag=1;
                for(int j=0;j<SNUM;j++)
                {
                    if(Need[i][j]>work[j])
                    {
                        flag=0;
                        break;
                    }
                }
                if(flag==1)
                {
                    break;
                }
            }
        }
        if(flag==1)
        {
            for(int j=0;j<SNUM;j++)
            {
                work[j]+=Allocation[i][j];
            }
            tmpfinish[i]=1;
        }
    }
    for(int j=0;j<TNUM;j++)
    {
        if(tmpfinish[i]==0&&Finish[i]==0)
        {
            printf("进程%d本次申请不安全!!\n",id+1);
            return 1;
        }
    }
    return 0;
}

int Banker(int id ,int *req)
{
    //检查分配数据合法性
    for(int i=0;i<SNUM;i++)
    {
        if(req[i]<=Need[id][i])
        {
            if(req[i]>Available[i])
            {
                printf("进程%d所申请资源%d系统无法满足!!\n",id+1,i+1);
                return 1;
            }
        }
        else
        {
            printf("进程%d所申请资源%d大于其所需!!\n",id+1,i+1);
            return 1;
        }
    }
    //预分配
    for(int i=0;i<SNUM;i++)
    {
        Available[i]-=req[i];
        Need[id][i]-=req[i];
        Allocation[id][i]+=req[i];
    }
    //安全检查算法
    if(safe(id)==1)  //不安全
    {
        for(int i=0;i<SNUM;i++)
        {
            Available[i]+=req[i];
            Need[id][i]+=req[i];
            Allocation[id][i]-=req[i];
        }
        return 1;
    }
    return 0;
}

void *process(void *args)
{
    int id=*(int *)args;  //设置进程号
    int req[SNUM]={0};  //资源申请数
    srand((int)time(0));  //设置随机数种子
    sleep(10-id);
    while(1)
    {
        for(int i=0;i<SNUM;i++)  //随机生成申请资源数量
        {
            if(Need[id][i]==0)
                req[i]=0;
            else
                req[i]=rand()%Need[id][i]+1; 
        }
        //申请执行信号量
        pthread_mutex_lock(&mutex);
        //打印本次分配信息
        printf("进程%d申请资源----",id+1);
        for(int i=0;i<SNUM;i++)
        {
            printf("资源%d：%d  ",i+1,req[i]);
        }
        printf("\n");

        if(Banker(id,req)==1)
            pthread_cond_wait(&cond,&mutex);  //等待下一次分配
        else
        {
            for(int i=0;i<SNUM;i++)
                req[i]=0;  //清空分配数组
        }
        bool finish=true;  //当前进程是否执行完毕
        for(int i=0;i<SNUM;i++)
        {
            if(Need[id][i]!=0)
            {
                finish=false;
                break;
            }
        }
        if(finish)  //当前进程执行完毕
        {
            printf("进程%d执行完毕!\n",id+1);
            Finish[id]=1;  //该线程执行完毕，标识符改变
            for(int i=0;i<SNUM;i++)  //释放资源
            {
                Available[i]+=Allocation[id][i];
                Allocation[id][i]=0;
            }
            pthread_cond_broadcast(&cond);
            pthread_mutex_unlock(&mutex);
            pthread_exit(NULL);
        }
        pthread_mutex_unlock(&mutex);
        sleep(1);
    }
}

int main()
{
    pthread_t tid[TNUM];  //进程标识
    int id[TNUM];

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    for(int i=0;i<TNUM;i++)
    {
        id[i]=i;
        pthread_create(&tid[i], NULL, process, (void*)&id[i]);
    }
    for(int i=0;i<TNUM;i++)
	{
		pthread_join(tid[i],NULL);
	}
    return 0;
}