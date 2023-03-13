#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include <windows.h>
#define MAX 20//存储空间初始分配量
#define MORE 10//存储空间配增量

typedef struct{
    int i;
    int j;
}PosType;
//当前的坐标位置

typedef struct{
    int ord;//通道在路径上的序号
    PosType seat;//在迷宫中的坐标位置
    int di;//从此通道块走向下一通道块的方向
	//1为向上2为向右3为向下4为向左
}SElemType;
//栈的结点

typedef struct{
    SElemType *base;//在栈构造之前和销毁之后，base值为NULL
    SElemType *top;//栈顶指针
    int stacksize;//当前已分配的存储空间，以元素为单位
}Stack;
//栈

//初始化栈
Stack InitStack(Stack S)
{
    S.base = (SElemType*)malloc(MAX * (sizeof(SElemType)));
    if(! S.base)//存储空间分配失败
        exit(0);
    S.top = S.base;
    S.stacksize = MAX;
    return S;
}

//若栈不空，则删除S的栈顶元素，用e返回其值
int IsEmpty(Stack S){
    if(S.top == S.base) //如果空栈，报错
        return 1;
    return 0;
}

//释放内存
void DestroyStack(Stack S){
    if(!IsEmpty(S))
        free(S.base);
}

//入栈
Stack Push(Stack S, SElemType e)
{
    if(S.top - S.base >= S.stacksize){//栈满
        S.base = (SElemType*)realloc(S.base, (S.stacksize + MORE)*sizeof(SElemType));
        if(!S.base)
            exit(0);
        S.top = S.base + S.stacksize;
        S.stacksize += MORE;
    }

    *(S.top) = e;
    S.top++;
    return S;
}

//出栈
Stack Pop(Stack S){
    if(S.top == S.base)
        exit(0);
    S.top--;
    return S;
}

//返回栈顶元素
SElemType GetTop(Stack S){
    if(S.top == S.base)
        exit(0);
    return *(S.top - 1);
}

//创建迷宫，设置障碍物
void Create_Maze(int **maze, int line, int row){
    int Fi,Fj;//用于随机产生有障碍的位置
    int i,j;
    
    for(i = 0; i < line; i++)
        for(j = 0; j < row; j++)
            if(i == 0 || j == 0 || i == line - 1 || j == row - 1)//围墙
                maze[i][j] = 0;
            else
                maze[i][j] = 1;//可通为1，有障碍为0

    /*随机产生障碍物*/
    srand((unsigned)time(NULL));
    for(i = (line + row) * 2; i > 0; i--){//随机生成line+row次
        Fi=rand()%line;
        Fj=rand()%row;
        maze[Fi][Fj] = 0;
    }
}

void Print_Maze(int **maze, int line, int row){//打印迷宫
    int i,j;
    //输出列坐标
    printf("  ");
    for(i = 0; i < row; i++)
        printf("%-2d",i);
    printf("\n");

    for(i = 0; i < line; i++){
        printf("%-2d",i);//输出行坐标
        for(j = 0; j < row; j++){
            switch(maze[i][j]){
                case 0:printf("* ");break;//0是障碍物
                case 3://3是指走过，但不通的地方
                case 1:printf("  ");break;//1是通的地方
                case 2:printf("+ ");break;//2是通路
                case 4:printf("Q ");break;//4是起点
                case 5:printf("Z ");break;//5是终点
            }
        }
        printf("\n");
    }
}
//下一步位置xy坐标的变化
PosType NextPos(PosType pos, int di){
    switch(di){
        case 1:pos.j++;break;
        case 2:pos.i++;break;
        case 3:pos.j--;break;
        case 4:pos.i--;break;
    }
    return pos;
}

//查找路径
void FindOut(int **maze, PosType Start, PosType End){
    Stack S;
    S = InitStack(S);
    PosType curpos;//当前位置
    curpos.i = Start.i;
    curpos.j = Start.j;

    SElemType e;//当前通道块

    int dex = 1;//探索第一步；

    do{
        if(maze[curpos.i][curpos.j] == 1){
            maze[curpos.i][curpos.j] = 2;//表示走过的路径
            e.di = 1;
            e.seat.i = curpos.i;
            e.seat.j = curpos.j;
            e.ord = dex;

            S = Push(S, e);//入栈

            if(curpos.i == End.i && curpos.j == End.j)//到达终点
                break;
            curpos = NextPos(curpos, 1);
            dex++;//到达终点所用的步数
        }
        else{
            if(!IsEmpty(S)){
                e = GetTop(S);//由于Pop的不完善，所以这两步加起来才是真正的Pop功能，下同
                S = Pop(S);
				dex--;
                while(e.di == 4 && !IsEmpty(S)){
                    maze[e.seat.i][e.seat.j] = 3;//3表示不能通过的标记
                    e = GetTop(S);
                    S = Pop(S);
                    dex--;
                }
                if(e.di < 4){
                    e.di++;
                    S = Push(S, e);
                    dex++;
                    curpos = NextPos(e.seat, e.di);
                }
            }
        }
    }while(!IsEmpty(S));

    if(IsEmpty(S)){
        maze[Start.i][Start.j] = 1;//因为无论如何起点都会留下足迹
        printf("\n无解");
    }
    else{//打印路径
        PosType Path[dex];
        int  i;
        while(!IsEmpty(S)){ 
            e = GetTop(S);
            S = Pop(S);
            Path[e.ord - 1].i = e.seat.i;//ord是从1开始计算的，而数组是从0开始
            Path[e.ord - 1].j = e.seat.j;
        }
        for(i = 0; i < dex - 1; i++)//最后一个单独打印
            printf("(%d,%d) -> ",Path[i].i,Path[i].j);
        printf("(%d,%d)\n",Path[i].i,Path[i].j);
    }
    DestroyStack(S);
}

//保存文件
void keepfile(int **maze,int M,int N)
{
	FILE* fp=fopen("maze.txt","w"); //打开文件
        if(fp==NULL)
        {
            printf("无文件");
        }
		for (int i = 0; i < M; i++)
		{
			for (int j = 0; i < N; i++)
			{
				printf("%d ",maze[i][j]);
			}
			printf("\n");
		}
	fclose(fp);
}
int main(){
    int Si,Sj,Ei,Ej;//分别是起点终点坐标
    int **maze;//迷宫二维数组
    int LSize, RSize;//迷宫大小，可以自定义行和列
    Stack S;
    system("color F0");
    printf("迷宫游戏v1.0\n");
    printf("西邮网络2101雷梓炳制作\n");
    Sleep(800);
    system("cls");


    //自定义迷宫大小
    printf("请输入迷宫大小：");
    scanf("%d %d",&LSize, &RSize);
    do{
        maze = (int**)malloc(sizeof(int*) * LSize);
    }while(maze == NULL);
    for(int i = 0; i < LSize; i++)
        do{
            *(maze + i) = (int*)malloc(sizeof(int) * RSize);
        }while(maze + i == NULL);

    //创建并输出迷宫
    Create_Maze(maze, LSize, RSize);
    Print_Maze(maze, LSize, RSize);

    printf("\n");

    //输入起点、终点
    printf("请输入起点：");
    scanf("%d %d",&Si,&Sj);
    while(Si > LSize - 1 || Sj > RSize - 1 || maze[Si][Sj] == 0){//最后一项表达式不能放到前面，不然会越界
        printf("起点有错，请重新输入");
        scanf("%d %d",&Si,&Sj);
    }
    printf("请输入终点:");
    scanf("%d %d",&Ei,&Ej);
    while(Ei > LSize - 1 || Ej > RSize - 1 || maze[Ei][Ej] == 0){
        printf("终点有错，请重新输入");
        scanf("%d %d",&Ei,&Ej);
    }

    //创建起点终点QZ
    PosType Start = {Si,Sj};
    PosType End = {Ei, Ej};

    printf("\n");
    FindOut(maze, Start, End);

    printf("\n");
    //起点终点标记符
    maze[Start.i][Start.j] = 4;
    maze[End.i][End.j] = 5;
    printf("*是墙壁, +是通路\n");
    Print_Maze(maze, LSize, RSize);

	printf("是否将结果输入到文件\n");
	printf("1/是，0/否\n");
	int num;
	scanf("%d",&num);
	if (num==1)
		keepfile(maze,LSize,RSize);
    free(maze);//释放动态二维数组

    return 0;
}