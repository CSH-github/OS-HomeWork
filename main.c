#include <stdlib.h>
#include <stdio.h>

//用双向链表表示空闲存储区表
typedef struct map{
    int address; //空闲区起始地址
    int size;   //空闲区大小
    struct map *next;
    struct map *prior;
} mapNode, *mp;

mp head;   //链表头，但并不指向实际的节点
mp last;    //链表尾地址
mp last_find;   //查找指针
int len=1;      //空闲分区个数，初始为1

void Lmalloc(int); //申请内存空间
void Lfree(int,int); //释放
void show();    //打印表格
void initial_map(int); //初始化表格

/**
 * 初始化表格
 * 设置内存大小为Max_length
 */
void initial_map(int Max_length)
{
    head = (mp)malloc(sizeof(mapNode));
    last =  (mp)malloc(sizeof(mapNode));
    head->prior = NULL;
    head->next = last;
    head->address=0;
    last->prior = head;
    last->next = head;
    last->address = 0;
    last->size = Max_length;
    printf("初始内存大小%d\n",Max_length);
}

/**
 * 向系统申请大小为size的内存空间
 * 用循环首次适应法分配
 */
void Lmalloc(int size)
{   int i=0;
    if(size<=0)
        {printf("内存大小不能小于等于0");
         return;}
    if(last_find)
    {
        for(;i<=len+1;i++) //循环查找
        {
            if(last_find->size==size) //刚好等于该空闲区大小，删除链表中该项
            {
                last_find->prior->next=last_find->next;
                last_find->next->prior=last_find->prior;
                last_find = last_find->next;
                len--;
                return;
            }
            else if(last_find->size>size)
            {
                last_find->size-=size;
                last_find->address+=size;
                return;
            }
            last_find=last_find->next;
            //表头并不指向实际节点
            if(last_find==head)
                last_find=last_find->next;
        }
        printf("没有足够大小的空闲区！"); //已找遍所有空闲区
    }
    else    //首次分配
    {
        mapNode *p = head->next;
        if(p->size==size)
            {   p->size=0;
                p->address=size;
                last_find = p->next;
                len--;
                return;
            }
        else if(p->size>size)
            {
                p->size-=size;
                p->address+=size;
                last_find=p;
                return;
            }
        printf("没有足够大小的空闲区!\n");
        
    }   
 
}
//释放内存，从地址addr开始释放大小size的空间
void Lfree(int addr, int size)
{
    mapNode *p = head;
    mp tmp = (mp)malloc(sizeof(mapNode));
    for(;p->address<=addr&&p->size;p=p->next);
    if(p->prior->address+p->prior->size==addr)  //情况1、2
    {   
        p->prior->size+=size;      //情况1,释放区前空闲区相连
        if(addr+size==p->address)   //情况2，与前后都相连
        {   /*此时要删除后空闲区
              若查找指针指向后空闲区，
              则调整其位置*/
            if (last_find == p) last_find = p->prior;

            p->prior->size+=p->size;
            p->prior->next=p->next;
            p->next->prior=p->prior;
            len--;
        }
    }
    else{
        if(addr+size==p->address&&p->size)  //情况3，与后空闲区相连
        {
            p->address-=size;
            p->size+=size;
        }
        else{          //情况4，与前后空闲区都不相连,在链表中添加一项
            tmp->address=addr;
            tmp->size=size;
            tmp->prior=p->prior;
            tmp->next=p;
            p->prior->next=tmp;
            p->prior=tmp;
            len++;
        }
    }
}












//打印空闲分区表
void show()
{
    int flag=0;
    printf("\n");
    printf("空闲分区表：\n");
    mapNode *q=head->next;
    printf("分区号\t起始地址 空闲区大小\n\n");
    for(int i=0;i<len;i++)
    {
        printf("%d\t",flag);
        flag++;
        printf("%d\t",q->address);
        printf("%dKB\t",q->size);
        q=q->next;
        printf("\n");
    }
    printf("\n");
}

int main(){
    initial_map(1000);  //内存大小初始化为1000
    int msize,address;
    int size = 1024;
    char* buff = (char*)malloc(size);
    char* delim=" ";

    while(1){
    gets(buff);  //得到用户输入的一行字符串
    char* cmdchar=strtok(buff,delim); //第一次调用strtok得到命令

    if(strcmp(cmdchar,"malloc")==0)
        {
            msize = atoi(strtok(NULL,delim));
            Lmalloc(msize);
            show();
                    }

    else if(strcmp(cmdchar,"free")==0)
        {
            msize = atoi(strtok(NULL,delim));
            address = atoi(strtok(NULL,delim));
            Lfree(address,msize);
            show();
                    }
    else if(strcmp(cmdchar,"quit")==0)  //结束
        return 0;
        
    //输入不符合要求，打印提示
    else printf("申请内存：molloc <size>  释放内存:free <size> <addr> 退出：quit\n");
     
    }
    
}