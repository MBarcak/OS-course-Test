#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "time.h"

#define MAX 100  //最大磁盘访问序列

int tracklist[MAX]={0};  //磁盘访问序列
int Ptr=0;  //当前磁头所在
int dir;  //磁头移动方向，0——负向；1——正向

//==============随机发生访问序列==============
void init_list()
{
    srand(time(NULL));

    for(int i=0;i<MAX;i++)
    {
        tracklist[i]=rand()%199+1;  //随机生成磁道
    }
    Ptr=rand()%40+80;  //随机生成磁头位置
    dir=rand()%2;
    printf("本次生成的磁盘访问序列：\n");
    for(int i=0;i<MAX;i++)
    {
        printf("%d\t ",tracklist[i]);
        if((i+1)%10==0)
            printf("\n");
    }
    printf("\n");
    printf("当前磁头位置：%d号磁道\n",Ptr);
    if(dir==0)
        printf("当前磁头移动方向：负向\n");
    else
        printf("当前磁头移动方向：正向\n");
    
    return;
}

//================排序函数================

int comp(const void*a,const void*b)
{
    return *(int*)a-*(int*)b;
}

void sort(int *src,int *des,int length)
{
    for(int i=0;i<length;i++)
    {
        des[i]=src[i];
    }
    qsort(des,length,sizeof(int),comp);
    /*
    for(int i=0;i<length;i++)
    {
        printf("%d  ",des[i]);
    }
    printf("\n");
    */
    return;
}

//================先来先服务调度算法================
void FCFS() 
{
    int num=0;  //缺页数量
    int list[MAX]={0};  //访问队列
    int ptr=Ptr;  //当前访问磁道

    //按先后顺序进入访问队列
    for(int i=0;i<MAX;i++)
        list[i]=tracklist[i];

    printf("***********先来先服务算法模拟***********\n");
    for(int i=0;i<MAX;i++)
    {
        printf("第%d次访问：%d号磁道\t",i+1,list[i]);
        num+=abs(ptr-list[i]);
        ptr=list[i];
        if((i+1)%4==0)
            printf("\n");
    }
    printf("本次平均寻道数为%.2lf\n",num*1.0/MAX);
    return;
}

//=============最短寻道时间优先调度算法=============
void SSTF()
{
    int num=0;  //缺页数量
    int list[MAX]={0};  //访问队列
    int ptr=Ptr;  //当前访问磁道
    int flag[MAX]={0};  //访问标志：0——未访问，1——已访问
    int left,right;
    int l_f,r_f;
    l_f=r_f=0;
    int ptr_num;  //磁头在访问队列中的位置
    int j=0;
    sort(tracklist,list,MAX);  //生成访问队列
    for(int i=0;i<MAX;i++)  //找到磁头位置
        if(ptr<=list[i])
        {
            ptr_num=i;
            break;
        }
    //printf("\n%d\n",ptr_num);
    printf("********最短寻道时间优先调度算法模拟********\n");
    for(int i=0;i<MAX;i++)
    {
        if(l_f==0&&r_f==0) //未到队首队尾
        {
            j=0;
            while(1)  //向队首找
            {
                if(ptr_num-j-1==-1)  //已到队首
                    break;
                if(flag[ptr_num-j-1]==0)
                {
                    left=ptr_num-j-1;
                    break;
                }
                j++;      
            }
            j=0;
            while(1)  //向队尾找
            {
                if(ptr_num+j==MAX)  //已到队尾
                    break;
                if(flag[ptr_num+j]==0)
                {
                    right=ptr_num+j;
                    break;
                }
                j++;     
            }
            if(abs(ptr-list[left])<=abs(ptr-list[right]))  //左边较小
            {
                printf("第%d次访问：%d号磁道\t",i+1,list[left]);
                num+=abs(ptr-list[left]);
                ptr=list[left];
                flag[left]=1;
                if(left==0)  //已到队首
                    l_f=1;
                if((i+1)%4==0)
                    printf("\n");
            }
            if(abs(ptr-list[left])>abs(ptr-list[right]))  //右边较小
            {
                printf("第%d次访问：%d号磁道\t",i+1,list[right]);
                num+=abs(ptr-list[right]);
                ptr=list[right];
                flag[right]=1;
                if(right==MAX-1)  //已到队尾
                    r_f=1;
                if((i+1)%4==0)
                    printf("\n");
            }
        }
        else if(l_f==1&&r_f==0)  //左边已经寻完
        {
            j=0;
            while(1)
            {
                if(ptr_num+j==MAX)  //已到队尾
                    break;
                if(flag[ptr_num+j]==0)
                {
                    for(int k=ptr_num+j;k<MAX;k++,i++)
                    {
                        printf("第%d次访问：%d号磁道\t",i+1,list[k]);
                        num+=abs(ptr-list[k]);
                        ptr=list[k];
                        flag[k]=1;
                        if((i+1)%4==0)
                            printf("\n");
                    }            
                    r_f=1;  //已到队尾
                    break;
                } 
                j++;
            }
        }
        else if(l_f==0&&r_f==1)  //右边已被寻完
        {
            j=0;
            while(1)
            {
                if(ptr_num-j-1==-1)  //已到队尾
                    break;
                if(flag[ptr_num-j-1]==0)
                {
                    for(int k=ptr_num-j-1;k>=0;k--,i++)
                    {
                        printf("第%d次访问：%d号磁道\t",i+1,list[k]);
                        num+=abs(ptr-list[k]);
                        ptr=list[k];
                        flag[k]=1;
                        if((i+1)%4==0)
                            printf("\n");
                    }
                    l_f=1;  //已到队首
                    break;
                } 
                j++;
            }
        }
        else
            break; 
    }
    printf("本次平均寻道数为%.2lf\n",num*1.0/MAX);
    return;
}

//==================电梯调度算法==================
void SCAN()
{
    int num=0;  //缺页数量
    int list[MAX]={0};  //访问队列
    int ptr=Ptr;  //当前访问磁道
    int ptr_num;  //磁头在访问队列中的位置

    sort(tracklist,list,MAX);  //生成访问队列
    for(int i=0;i<MAX;i++)  //找到磁头位置
        if(ptr<=list[i])
        {
            ptr_num=i;
            break;
        }
    //printf("\n%d\n",ptr_num);
    printf("*******电梯调度算法模拟*******\n");
    for(int i=0;i<MAX;i++)
    {
        if(dir==1)  //当前磁头移动方向为正向
        {
            for(int j=ptr_num;j<MAX;j++,i++)
            {
                printf("第%d次访问：%d号磁道\t",i+1,list[j]);
                num+=abs(ptr-list[j]);
                ptr=list[j];
                if((i+1)%4==0)
                    printf("\n");
            }
            for(int j=ptr_num-1;j>=0;j--,i++)
            {
                printf("第%d次访问：%d号磁道\t",i+1,list[j]);
                num+=abs(ptr-list[j]);
                ptr=list[j];
                if((i+1)%4==0)
                    printf("\n");
            }
        }
        else
        {
            for(int j=ptr_num-1;j>=0;j--,i++)
            {
                printf("第%d次访问：%d号磁道\t",i+1,list[j]);
                num+=abs(ptr-list[j]);
                ptr=list[j];
                if((i+1)%4==0)
                    printf("\n");
            }       
            for(int j=ptr_num;j<MAX;j++,i++)
            {
                printf("第%d次访问：%d号磁道\t",i+1,list[j]);
                num+=abs(ptr-list[j]);
                ptr=list[j];
                if((i+1)%4==0)
                    printf("\n");
            }     
        }
    }
    printf("本次平均寻道数为%.2lf\n",num*1.0/MAX);
    return;
}

//================循环式单向电梯调度算法================
void CSCAN()
{
    int num=0;  //缺页数量
    int list[MAX]={0};  //访问队列
    int ptr=Ptr;  //当前访问磁道
    int ptr_num;  //磁头在访问队列中的位置

    sort(tracklist,list,MAX);  //生成访问队列
    for(int i=0;i<MAX;i++)  //找到磁头位置
        if(ptr<=list[i])
        {
            ptr_num=i;
            break;
        }
    //printf("\n%d\n",ptr_num);
    printf("********循环式单向电梯调度算法模拟********\n");
    for(int i=0;i<MAX;i++)
    {
        if(dir==1)  //当前磁头移动方向为正向
        {
            for(int j=ptr_num;j<MAX;j++,i++)
            {
                printf("第%d次访问：%d号磁道\t",i+1,list[j]);
                num+=abs(ptr-list[j]);
                ptr=list[j];
                if((i+1)%4==0)
                    printf("\n");
            }
            for(int j=0;j<ptr_num;j++,i++)
            {
                printf("第%d次访问：%d号磁道\t",i+1,list[j]);
                num+=abs(ptr-list[j]);
                ptr=list[j];
                if((i+1)%4==0)
                    printf("\n");                
            }
        }
        else
        {
            for(int j=ptr_num-1;j>=0;j--,i++)
            {
                printf("第%d次访问：%d号磁道\t",i+1,list[j]);
                num+=abs(ptr-list[j]);
                ptr=list[j];
                if((i+1)%4==0)
                    printf("\n");
            }
            for(int j=MAX-1;j>=ptr_num;j--,i++)
            {
                printf("第%d次访问：%d号磁道\t",i+1,list[j]);
                num+=abs(ptr-list[j]);
                ptr=list[j];
                if((i+1)%4==0)
                    printf("\n");                
            }
        }        
    }
    printf("本次平均寻道数为%.2lf\n",num*1.0/MAX);
    return;
}

//================双队列单向电梯调度算法================
void FSCAN()
{
    int num=0;  //缺页数量
    int list1[MAX/2]={0};  //第一条访问队列
    int list2[MAX/2]={0};  //第二条访问队列
    int ptr=Ptr;  //当前访问磁道
    int ptr_num;  //磁头在访问队列中的位置

    sort(tracklist,list1,MAX/2);  //生成访问队列
    sort(tracklist+MAX/2,list2,MAX/2);  //生成访问队列

    printf("********双队列单向电梯调度算法模拟********\n");
    /*第一次寻道*/
    for(int i=0;i<MAX/2;i++)  //找到磁头在队列1的位置
        if(ptr<=list1[i])
        {
            ptr_num=i;
            break;
        }
    for(int i=0;i<MAX/2;i++)
    {
        if(dir==1)  //当前磁头移动方向为正向
        {
            for(int j=ptr_num;j<MAX/2;j++,i++)
            {
                printf("第%d次访问：%d号磁道\t",i+1,list1[j]);
                num+=abs(ptr-list1[j]);
                ptr=list1[j];
                if((i+1)%4==0)
                    printf("\n");
            }
            for(int j=0;j<ptr_num;j++,i++)
            {
                printf("第%d次访问：%d号磁道\t",i+1,list1[j]);
                num+=abs(ptr-list1[j]);
                ptr=list1[j];
                if((i+1)%4==0)
                    printf("\n");                
            }
        }
        else
        {
            for(int j=ptr_num-1;j>=0;j--,i++)
            {
                printf("第%d次访问：%d号磁道\t",i+1,list1[j]);
                num+=abs(ptr-list1[j]);
                ptr=list1[j];
                if((i+1)%4==0)
                    printf("\n");
            }
            for(int j=MAX/2-1;j>=ptr_num;j--,i++)
            {
                printf("第%d次访问：%d号磁道\t",i+1,list1[j]);
                num+=abs(ptr-list1[j]);
                ptr=list1[j];
                if((i+1)%4==0)
                    printf("\n");                
            }
        }
    }
    /*第二次寻道*/
    for(int i=0;i<MAX/2;i++)  //找到磁头在队列1的位置
        if(ptr<=list2[i])
        {
            ptr_num=i;
            break;
        }
    for(int i=MAX/2;i<MAX;i++)
    {
        if(dir==1)  //当前磁头移动方向为正向
        {
            for(int j=ptr_num;j<MAX/2;j++,i++)
            {
                printf("第%d次访问：%d号磁道\t",i+1,list2[j]);
                num+=abs(ptr-list2[j]);
                ptr=list2[j];
                if((i+1)%4==0)
                    printf("\n");
            }
            for(int j=0;j<ptr_num;j++,i++)
            {
                printf("第%d次访问：%d号磁道\t",i+1,list2[j]);
                num+=abs(ptr-list2[j]);
                ptr=list2[j];
                if((i+1)%4==0)
                    printf("\n");                
            }
        }
        else
        {
            for(int j=ptr_num-1;j>=0;j--,i++)
            {
                printf("第%d次访问：%d号磁道\t",i+1,list2[j]);
                num+=abs(ptr-list2[j]);
                ptr=list2[j];
                if((i+1)%4==0)
                    printf("\n");
            }
            for(int j=MAX/2-1;j>=ptr_num;j--,i++)
            {
                printf("第%d次访问：%d号磁道\t",i+1,list2[j]);
                num+=abs(ptr-list2[j]);
                ptr=list2[j];
                if((i+1)%4==0)
                    printf("\n");                
            }
        }
    }
    printf("本次平均寻道数为%.2lf\n",num*1.0/MAX);
    return;
}

int main()
{
    int in;
    while(1)
    {
        printf("==========移动头磁盘调度算法模拟==========\n");
        printf("1、生成磁盘访问队列\n");
        printf("2、先来先服务调度算法\n");
        printf("3、最短寻道时间优先调度算法\n");
        printf("4、电梯调度算法\n");
        printf("5、循环式单向电梯调度算法\n");
        printf("6、双队列单向电梯调度算法\n");
        printf("0、退出\n");
        printf("输入序号：");
        scanf("%d",&in);
        switch(in)
        {
            case 1:init_list();break;
            case 2:FCFS();break;
            case 3:SSTF();break;
            case 4:SCAN();break;
            case 5:CSCAN();break;
            case 6:FSCAN();break;
            case 0:return 0;
            default:printf("输入错误，请重新输入\n");break;
        }
    }
}
