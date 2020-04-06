#include "msh.h"

char cwd[80];  //当前工作目录
char *PATH[100];
int num_path=0;  //环境变量数
char src_path[400];  //原环境变量


//==========================环境变量==============================

/*
    分割环境变量字符串
*/
int split_path(char *buf,char **argv)
{
    int i=0;  //buf指针
    int j=0;  //字符串头位置指针
    int argc=0;

    while(*(buf+i)==' ')  //找到第一个不为0的字符
        i++;
    j=i;
    while(1)
    {
        if(*(buf+i)==':')
        {
            argv[argc++]=(buf+j);  //将这个字串的头指针赋给
            *(buf+i)='\0';  //将这个字符串的末尾加0分割
            i++;
            while(*(buf+i)==' ')  //找到下一个不为0的字符
                i++;
            j=i;
        }
        else if(*(buf+i)=='\0')
        {
            argv[argc++]=(buf+j);
            break;
        }
        else
            i++;
    }
    return argc;
}

/*
    初始化环境变量
*/
void init_path()
{
    char *buf;

    buf=getenv("PATH");
    strcpy(src_path,buf);
    num_path=split_path(src_path,PATH);
    return;
}

/*
    改变环境变量
*/
void export_path(char *now)
{
    char *newpath[30];  //新增环境变量指针
    int newnum;  //新增环境变量数量
    char *buf;

    for(int i=0;i<num_path;i++)
        PATH[i]=NULL;
    buf=getenv("PATH");
    strcpy(src_path,buf);
    num_path=split_path(src_path,PATH);
    newnum=split_path(now,newpath);  //分割
    if(strcmp(newpath[0],"$PATH")==0)  //如果保留原环境变量，将新增加在末尾
    {
        for(int i=0;i<newnum-1;i++)
        {
            PATH[num_path+i]=newpath[i+1];
        }
        num_path+=(newnum-1);
    }
    else if(strcmp(newpath[newnum-1],"$PATH")==0)  //保留原环境变量，将新增加在开头
    {
        for(int i=0;i<num_path;i++)
        {
            PATH[num_path-i+newnum-2]=PATH[num_path-1-i];
        }
        for(int i=0;i<newnum-1;i++)
        {
            PATH[i]=newpath[i];
        }
        num_path+=(newnum-1);
    }
    else  //清除原环境变量，替换为新增
    {
        for(int i=0;i<num_path;i++)
        {
            PATH[i]=NULL;
        }
        for(int i=0;i<newnum;i++)
        {
            PATH[i]=newpath[i];
        }
        num_path=newnum;
    }

    // 连接新环境变量为字符串
    char tmp[600];
    strcpy(tmp,PATH[0]);
    for(int i=1;i<num_path;i++)
    {
        strcat(tmp,":");
        strcat(tmp,PATH[i]);
    }
    setenv("PATH",tmp,1);  //设置新的环境变量

    return;
}

char* split_pathhead(char *str)
{
    int i=0;
    while(*(str+i)!='=')
        i++;
    str=str+i+1;
    return str;
}

//==========================外部变量==============================

/*
    在环境变量中寻找外部命令
*/
/*
int find_file(char *argv)
{
    int i=0;
    char tmp[50];
    strcpy(tmp,argv);
    while(*(argv+i)!='\0')  //找到"/"，不是文件名，返回
    {
        if(*(argv+i)=='/')  
        {
            return 0;  //路径
        }
        else
        {
            i++;
        }       
    }
    DIR *dirp; 
    struct dirent *dp;

    for(int i=0;i<num_path;i++)
    {
        dirp = opendir(PATH[i]); //打开目录指针
        while ((dp = readdir(dirp)) != NULL) { //通过目录指针读目录
            if(strcmp(dp->d_name,argv)==0)
            {
                strcpy(tmp,PATH[i]);
                strcat(tmp,"/");
                strcat(tmp,argv);
                strcpy(argv,tmp);
                (void) closedir(dirp); //关闭目录
                return 1;  //找到文件
            }
        }      
        (void) closedir(dirp); //关闭目录
    }
    return 2;  //未找到
}
*/
int bulidout_commend(char **argv)
{
    pid_t fpid; //fpid表示fork函数返回的值  
    /*
    int flag=0;
    char *envp[3] = { "", "TERM=console", NULL };  //环境
    // 连接新环境变量为字符串
    char tmp[400];
    strcpy(tmp,"PATH=");
    strcat(tmp,PATH[0]);
    for(int i=1;i<num_path;i++)
    {
        strcat(tmp,":");
        strcat(tmp,PATH[i]);
    }
    envp[0]=tmp;

    char filename[20];
    strcpy(filename,argv[0]);
    int flag;
    flag=find_file(filename);
    */
        //argv[0]=filename;
    fpid = fork();
    if(fpid == 0)
    {
        if(execvp(argv[0], argv)<0)
        {
            printf("%s:command not found.\n",argv[0]);
            exit(0);
        }
    }
    else
        wait(NULL);
    return 0;
}


//==========================辅助函数==============================

/*
    分割命令字符串
*/
int split(char *buf,char **argv)
{
    int i=0;  //buf指针
    int j=0;  //字符串头位置指针
    int argc=0;

    while(*(buf+i)==' ')  //找到第一个不为0的字符
        i++;
    j=i;
    while(1)
    {
        if(*(buf+i)==' ')
        {
            argv[argc++]=(buf+j);  //将这个字串的头指针赋给
            *(buf+i)='\0';  //将这个字符串的末尾加0分割
            i++;
            while(*(buf+i)==' ')  //找到下一个不为0的字符
                i++;
            j=i;
        }
        else if(*(buf+i)=='\0')
        {
            argv[argc++]=(buf+j);
            argv[argc]=NULL;  //最后一个指针为空
            break;
        }
        else
            i++;
    }
    return argc;
}

//==========================内部命令==============================

/*
    touch命令实现函数
*/
void touch_cmd(int argc,char **argv)
{
    FILE *fp;

    fp=fopen(argv[1],"w");
    fclose(fp);
}

void date_cmd(int argc,char **argv)
{
    time_t timep;
    struct tm *up,* gp;
    time (&timep);
    up=gmtime(&timep);
    gp=gmtime(&timep);
    if(argc==1)
    {
        printf("%d年 %d月 %d日 %d:%d:%d UTC\n",1900+up->tm_year,1+up->tm_mon,
        up->tm_mday,8+up->tm_hour,up->tm_min,up->tm_sec);
    }
    else if(argc==2&&strcmp(argv[1],"-u")==0)
    {
        printf("%d年 %d月 %d日 %d:%d:%d CST\n",1900+gp->tm_year,1+gp->tm_mon,
        gp->tm_mday,gp->tm_hour,gp->tm_min,gp->tm_sec);
    }
    else
    {
        printf("date:commend error!\n");
    }
    return;
}

int bulidin_commend(char **argv,int argc)
{    
    if(strcmp("exit",argv[0])==0)
    {
        exit(0);
    }
    else if(strcmp("pwd",argv[0])==0)
    {
        getcwd(cwd,sizeof(cwd));
        printf("%s\n",cwd);
        return 0;
    }
    else if(strcmp("touch",argv[0])==0)
    {
        touch_cmd(argc,argv);
        return 0;
    }
    else if(strcmp("cd",argv[0])==0)
    {
        if(chdir(argv[1])==0)
            return 0;
        else
        {
            printf("cd:no such file or directory\n");
            return 0;
        }
    }
    else if(strcmp("export",argv[0])==0)
    {
        argv[1]=split_pathhead(argv[1]);
        export_path(argv[1]);
        return 0;
    }
    else if(strcmp("date",argv[0])==0)
    {
        date_cmd(argc,argv);
        return 0;
    }
     else if(strcmp("whoami",argv[0])==0)
    {
        uid_t id;
        struct passwd * pa;
        id=geteuid();
        pa=getpwuid(id);
        printf("%s\n",pa->pw_name);
        return 0;
    }
    else if((strcmp("echo",argv[0])==0)&&(strcmp("$PATH",argv[1])==0))
    {
        char *nn;
        nn=getenv("PATH");
        printf("PATH=%s\n",nn);
        return 0;
    }
    else
        return 1;  //未找到匹配命令
}

//==========================工作函数==============================

void work(char *str)
{
    char *argv[5];
    int argc=0;
    argc=split(str,argv);
    if(bulidin_commend(argv,argc)==0)  //内部命令执行
    {
        return;
    }
    else
    {
        bulidout_commend(argv);  //外部命令执行
    }
    return;
}


//==========================main==============================

int main()
{
    char cmd[100];  //命令
    init_path();
    while(1)
    {
        getcwd(cwd,sizeof(cwd));
        printf("msh_Shell:%s>",cwd);
        gets(cmd);  //读取输入字符串
        work(cmd);  //工作函数
    }
    return 0;
}

//export PATH="/usr/local/sbin:/bin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin"
///java/jdk-11.0.4/bin:/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin
