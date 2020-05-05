#include "stdio.h"
#include "stdlib.h"
#include "time.h"

#define MAX 20  //页面序列
#define WORK 3  //工作集大小
#define PAGE 7  //页面数量

typedef struct pagelist{
    int page[MAX];
    int type[MAX];  //0为读，1为读写
}LIST;

LIST list;  //页面序列
int worklist[WORK];  //工作集

void print(int i,int j,int x)
{
    printf("第%d次访问：",i+1);
    if(x==1)
        printf("页号:%d 状态位:%d ",list.page[i],x);  //状态位为1在内存，0在外存
    else
        printf("将%d号页面淘汰出内存!换入页面：页号:%d 状态位:%d ",worklist[j],list.page[i],x);

    if(list.type[i]==0)
        printf("访问方式:r\n");
    else
        printf("访问方式:rw\n");
    return;
}

void init_work()
{
    list.page[0]=7;list.page[1]=4;list.page[2]=1;
    worklist[0]=7;worklist[1]=4;worklist[2]=1;
}

void init_list()
{
    double t;
    srand(time(NULL));
    init_work();
    for(int i=3;i<MAX;i++)
    {
        t=rand()/(RAND_MAX+1.0);
        list.page[i]=rand()%PAGE+1;  //随机生成页面
        if(t>=0.7)
            list.type[i]=1;  //读写
        else
            list.type[i]=0;  //读
    }
    printf("本次生成的页面序列：");
    for(int i=0;i<MAX;i++)
    {
        printf("%d ",list.page[i]);
    }
    printf("\n");
    printf("本次生成的访问限制：");
    for(int i=0;i<MAX;i++)
    {
        printf("%d ",list.type[i]);
    }
    printf("\n");

    return;
}

//================最佳淘汰算法================
void OPT() 
{
    int lossnum=0;  //缺页数量
    int flag=0;
    int tmp=-1;
    init_work();
    printf("***********最佳淘汰算法模拟***********\n");
    for(int i=0;i<MAX;i++){
        flag=0;
        for(int j=0;j<WORK;j++){
            if(list.page[i]==worklist[j]){
                print(i,0,1);
                flag=1;
                break;
            }
        }
        if(flag==1)
            continue;       
        tmp=0;
        int g[WORK]={0};
        int g1=0;
        for(int j=i+1;j<MAX;j++){
            for(int k=0;k<WORK;k++){
                if(list.page[j]==worklist[k]){
                    g[k]=1;
                    tmp++;
                    if(tmp==2){
                        g1=1;
                        break;
                    }
                }
            }
            if(g1==1)
                break;
        }
        for(int j=0;j<WORK;j++){
            if(g[j]==0){
                print(i,j,0);
                worklist[j]=list.page[i];
                g1=1;
                break;
            }
        }
        if(g1==0)
            print(i,tmp,0);
        lossnum++;
    }
    printf("本次淘汰率为%.2lf%\n",lossnum*1.0/MAX*100);
    return;
}

//================先进先出淘汰算法================
void FIFO()
{
    int lossnum=0;  //缺页数量
    int flag=0;
    int ptr=0;  //头指针

    init_work();
    printf("********先进先出淘汰算法模拟********\n");
    for(int i=0;i<MAX;i++)
    {
        flag=0;
        for(int j=0;j<WORK;j++)
        {
            if(list.page[i]==worklist[j])
            {
                print(i,0,1);
                flag=1;
                break;
            }
        }
        if(flag==1)  //已经找到页
            continue;
        
        //换入
        print(i,ptr,0);
        worklist[ptr]=list.page[i];
        ptr=(ptr+1)%WORK;
        lossnum++;
    }
    printf("本次淘汰率为%.2lf%\n",lossnum*1.0/MAX*100);
    return;
}

//================最近最久未使用淘汰算法================
void LRU()
{
    int lossnum=0;  //缺页数量
    int flag=0;
    int tmp=0; 
    init_work();
    printf("*******最近最久未使用淘汰算法模拟*******\n");
    for(int i=0;i<MAX;i++){
        flag=0;
        for(int j=0;j<WORK;j++){
            if(list.page[i]==worklist[j]){
                print(i,0,1);
                flag=1;
                break;
            }
        }
        if(flag==1)  //已经找到页
            continue;
        tmp=0;
        int g[WORK]={0};
        int g1=0;
        for(int j=i-1;j>=0;j--){
            for(int k=0;k<WORK;k++){
                if(list.page[j]==worklist[k]){
                    g[k]=1;
                    tmp++;
                    if(tmp==2){
                        g1=1;
                        break;
                    }
                }
            }
            if(g1==1)
                break;
        }
        for(int j=0;j<WORK;j++){
            if(g[j]==0){
                print(i,j,0);
                worklist[j]=list.page[i];
                g1=1;
                break;
            }
        }
        lossnum++;
    }
    printf("本次淘汰率为%.2lf%\n",lossnum*1.0/MAX*100);
    return;
}

//================简单时钟淘汰算法================
void NRU()
{
    int lossnum=0;  //缺页数量
    int last[WORK]={0};  //访问位，1为最近访问过，0为最近未访问
    int flag=0;
    int ptr=0; 

    init_work();
    printf("********简单时钟淘汰算法模拟********\n");
    for(int i=0;i<MAX;i++)
    {
        flag=0;
        for(int j=0;j<WORK;j++)
        {
            if(list.page[i]==worklist[j])
            {
                print(i,0,1);
                last[j]=1;
                flag=1;
                break;
            }
        }
        if(flag==1)  //已经找到页
            continue;

        //换入
        while(1)
        {
            if(last[ptr]==0)
            {
                print(i,ptr,0);
                worklist[ptr]=list.page[i];
                last[ptr]=1;
                ptr=(ptr+1)%WORK;
                lossnum++;
                break;
            }
            else
            {
                last[ptr]=0;
                ptr=(ptr+1)%WORK;
            }
        }
    }
    printf("本次淘汰率为%.2lf%\n",lossnum*1.0/MAX*100);
    return;
}

//================改进时钟淘汰算法================
void proclock()
{
    int lossnum=0;  //缺页数量
    int last[WORK]={0};  //访问位，1为最近访问过，0为最近未访问
    int modify[WORK]={0};  //修改位，若修改为1，否则为0
    int flag=0;
    int ptr=0; 

    init_work();
    printf("********改进时钟淘汰算法模拟********\n");
    for(int i=0;i<MAX;i++){
        flag=0;
        for(int j=0;j<WORK;j++){
            if(list.page[i]==worklist[j]){
                if(list.type[i]==1){  //若为读写，则修改
                    modify[j]=1;
                }
                print(i,0,1);
                last[j]=1;
                flag=1;
                break;
            }
        }
        if(flag==1)  //已经找到页
            continue;
        //淘汰
        int flag1=0;
        for(int j=0;j<WORK;j++){
            if(last[ptr+j]==0&&modify[ptr+j]==0){  //如果未访问且未修改，淘汰
                print(i,ptr+j,0);
                worklist[ptr+j]=list.page[i];
                if(list.type[i]==1)
                    modify[ptr+j]=1;
                last[ptr+j]=1;
                ptr=(ptr+1)%WORK;
                flag1=1;
                lossnum++;
                break;
            }
        }
        if(flag1==1)
            continue;
        int flag2=0;
        for(int j=0;j<WORK;j++){
            if(last[ptr+j]==0&&modify[ptr+j]==1){  //如果未访问但是修改，淘汰
                print(i,ptr+j,0);
                worklist[ptr+j]=list.page[i];
                if(list.type[i]==1)
                    modify[ptr+j]=1;
                last[ptr+j]=1;
                ptr=(ptr+1)%WORK;
                flag2=1;
                lossnum++;
                break;
            }
            last[ptr+j]=0;
        }
        if(flag2==1)
            continue;
        flag=0;
        for(int j=0;j<WORK;j++){
            if(last[ptr+j]==0&&modify[ptr+j]==0){  //如果未访问且未修改，淘汰
                print(i,ptr+j,0);
                worklist[ptr+j]=list.page[i];
                if(list.type[i]==1)
                    modify[ptr+j]=1;
                last[ptr+j]=1;
                ptr=(ptr+1)%WORK;
                flag1=1;
                lossnum++;
                break;
            }
        }
        if(flag1==1)
            continue;
        flag2=0;
        for(int j=0;j<WORK;j++){
            if(last[ptr+j]==0&&modify[ptr+j]==1){  //如果未访问但是修改，淘汰
                print(i,ptr+j,0);
                worklist[ptr+j]=list.page[i];
                if(list.type[i]==1)
                    modify[ptr+j]=1;
                last[ptr+j]=1;
                ptr=(ptr+1)%WORK;
                flag2=1;
                lossnum++;
                break;
            }
            last[ptr+j]=0;
        }
        if(flag2==1)
            continue;
    }
    printf("本次淘汰率为%.2lf%\n",lossnum*1.0/MAX*100);
    return;
}

int main()
{
    int in;
    while(1)
    {
        printf("============页面淘汰算法模拟============\n");
        printf("1、生成页面队列\n");
        printf("2、最佳淘汰算法\n");
        printf("3、先进先出淘汰算法\n");
        printf("4、最近最久未使用淘汰算法\n");
        printf("5、简单Clock 淘汰算法\n");
        printf("6、改进型 Clock 淘汰算法\n");
        printf("0、退出\n");
        printf("输入序号：");
        scanf("%d",&in);
        switch(in)
        {
            case 1:init_list();break;
            case 2:OPT();break;
            case 3:FIFO();break;
            case 4:LRU();break;
            case 5:NRU();break;
            case 6:proclock();break;
            case 0:return 0;
        }
    }
}
