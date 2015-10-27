#include "stdio.h"
#include "stdlib.h"
//#include "conio.h"
#include "math.h"
#include "time.h"
#define num_C  10   //城市个数
//#define N      100  //群体规模为100
#define pc     0.9  //交叉概率为0.9
#define pm     0.1  //变异概率为10%
#define ps     0.6  //进行选择时保留的比例
#define genmax 200  //最大代数200
int RandomInteger(int low,int high);
void Initial_gen(struct unit group[100]);
void Sort(struct unit group[100]);
void Copy_unit(struct unit *p1,struct unit *p2);
int search_son(int son[num_C],int k);
void Cross(struct unit *p1,struct unit *p2);
void Varation(struct unit group[100],int i);
void Evolution(struct unit group[100]);
void Calculate_cost(struct unit *p);
void Print_optimum(struct unit group[100]);
/* 定义个体信息 */
typedef struct unit
{
    int path[num_C]; //个体的路径信息
    int cost;        //个体代价值
};
struct unit group[100]; //种群变量group
int num_gen=0;        //记录当前达到第几代
/***************************************************************************/
/* 城市间的距离信息：                                                      */
/*           北京  天津  武汉  深圳  长沙  成都  杭州  西安  拉萨  南昌    */
/*           (0)   (1)   (2)   (3)   (4)   (5)   (6)   (7)   (8)   (9)     */
/*  北京(0)   0    118   1272  2567  1653  2097  1425  1177  3947  1574    */
/*  天津(1)  118    0    1253  2511  1633  2077  1369  1157  3961  1518    */
/*  武汉(2)  1272  1253   0    1462  380   1490  821   856   3660  385     */
/*  深圳(3)  2567  2511  1462   0    922   2335  1562  2165  3995  933     */
/*  长沙(4)  1653  1633  380   922    0    1700  1041  1135  3870  456     */
/*  成都(5)  2097  2077  1490  2335  1700   0    2311  920   2170  1920    */
/*  杭州(6)  1425  1369  821   1562  1041  2311   0    1420  4290  626     */
/*  西安(7)  1177  1157  856   2165  1135  920   1420   0    2870  1290    */
/*  拉萨(8)  3947  3961  3660  3995  3870  2170  4290  2870   0    4090    */
/*  南昌(9)  1574  1518  385   993   456   1920  626   1290  4090   0      */
/***************************************************************************/
int Cost_table[10][10]= {{0,118,1272,2567,1653,2097,1425,1177,3947,1574},
    {118,0,1253,2511,1633,2077,1369,1157,3961,1518},
    {1272,1253,0,1462,380,1490,821,856,3660,385},
    {2567,2511,1462,0,922,2335,1562,2165,3995,933},
    {1653,1633,380,922,0,1700,1041,1135,3870,456},
    {2097,2077,1490,2335,1700,0,2311,920,2170,1920},
    {1425,1369,821,1562,1041,2311,0,1420,4290,626},
    {1177,1157,856,2165,1135,920,1420,0,2870,1290},
    {3947,3961,3660,3995,3870,2170,4290,2870,0,4090},
    {1574,1518,385,993,456,1920,626,1290,4090,0}
};

int main()
{
    srand((int)time(100ULL)); //初始化随机数发生器
    Initial_gen(group);     //初始化种群
    Evolution(group);       //进化：选择、交叉、变异
    getch();
    return 0;
}
/* 初始化种群 */
void Initial_gen(struct unit group[100])
{
    int i,j,k;
    struct unit *p;
    for(i=0; i<=100-1; i++) //初始化种群里的100个个体
    {
        p=&group[i]; //p指向种群的第i个个体
        for(j=0; j<=num_C-1; j++) //生成10个城市间的一个随机路径
        {
            k=0;
            if(j==0)  p->path[j]=RandomInteger(0,num_C-1);
            else
            {
                p->path[j]=RandomInteger(0,num_C-1);
                while(k<j)
                {
                    //与之前城市重复，重新生成一个城市
                    if(p->path[j]==p->path[k])
                    {
                        p->path[j]=RandomInteger(0,num_C-1);
                        k=0;
                    }
                    else k++;
                }//end while
            }
        }//end 生成路径
        Calculate_cost(p); //计算该路径的代价值
    }//end 初始化种群
}
/* 种群进化，进化代数由genmax决定 */
void Evolution(struct unit group[100])
{
    int i,j;
    int temp1,temp2,temp3,temp4,temp5;
    temp1=100*pc/2;
    temp2=100*(1-pc);
    temp3=100*(1-pc/2);
    temp4=100*(1-ps);
    temp5=100*ps;
    for(i=1; i<=genmax; i++)
    {
        //选择
        Sort(group);
        Print_optimum(group,i-1); //输出当代(第i-1代)种群
        for(j=0; j<=temp4-1; j++)
        {
            Copy_unit(&group[j],&group[j+temp5]);
        }
        //交叉
        for(j=0; j<=temp1-1;)
        {
            Cross(&group[temp2+j],&group[temp3+j]);
            j+=2;
        }
        //变异
        Varation(group,i);
    }
    Sort(group);
    Print_optimum(group,i-1); //输出当代(第i-1代)种群
}
/* 交叉 */
void Cross(struct unit *p1,struct unit *p2)
{
    int i,j,cross_point;
    int son1[num_C],son2[num_C];
    for(i=0; i<=num_C-1; i++) //初始化son1、son2
    {
        son1[i]=-1;
        son2[i]=-1;
    }
    cross_point=RandomInteger(1,num_C-1); //交叉位随机生成
    //交叉，生成子代
    //子代1
    //子代1前半部分直接从父代复制
    for(i=0; i<=cross_point-1; i++)  son1[i]=p1->path[i];
    for(i=cross_point; i<=num_C-1; i++)
    {
        for(j=0; j<=num_C-1; j++) //补全p1
        {
            if(search_son(son1,p2->path[j])==1)
            {
                son1[i]=p2->path[j];
                break;
            }
            else ;
        }
    }//end 子代1
    //子代2
    //子代1后半部分直接从父代复制
    for(i=cross_point; i<=num_C-1; i++)  son2[i]=p2->path[i];
    for(i=0; i<=cross_point-1; i++)
    {
        for(j=0; j<=num_C-1; j++) //补全p2
        {
            if(search_son(son2,p1->path[j])==1)
            {
                son2[i]=p1->path[j];
                break;
            }
            else ;
        }
    }//end 子代2
    //end 交叉
    for(i=0; i<=num_C-1; i++)
    {
        p1->path[i]=son1[i];
        p2->path[i]=son2[i];
    }
    Calculate_cost(p1); //计算子代p1的代价
    Calculate_cost(p2); //计算子代p2的代价
}
/* 变异 */
void Varation(struct unit group[100],int flag_v)
{
    int flag,i,j,k,temp;
    struct unit *p;
    flag=RandomInteger(1,100);
    //在进化后期，增大变异概率
    if(flag<=(flag_v>100)?(5*100*pm):(100*pm))
    {
        i=RandomInteger(0,100-1);      //确定发生变异的个体
        j=RandomInteger(0,num_C-1);  //确定发生变异的位
        k=RandomInteger(0,num_C-1);
        p=&group[i];                 //变异
        temp=p->path[j];
        p->path[j]=p->path[k];
        p->path[k]=temp;
        Calculate_cost(p);           //重新计算变异后路径的代价
    }
}
/* 检查k是否在son[num_C]中已出现过 */
int search_son(int son[num_C],int k)
{
    int i;
    for(i=0; i<=num_C-1; i++)
    {
        if(son[i]==k) return -1;
        else ;
    }
    return 1;
}
/* 将种群中个体按代价从小到大排序 */
void Sort(struct unit group[100])
{
    int i,j;
    struct unit temp,*p1,*p2;
    for(j=1; j<=100-1; j++) //排序总共需进行100-1轮
    {
        for(i=1; i<=100-1; i++)
        {
            p1=&group[i-1];
            p2=&group[i];
            if(p1->cost>p2->cost) //代价值大的往后排
            {
                Copy_unit(p1,&temp);
                Copy_unit(p2,p1);
                Copy_unit(&temp,p2);
            }//end if
        }//end 一轮排序
    }//end 排序
}
/* 计算某个路径的代价值 */
void Calculate_cost(struct unit *p)
{
    int j;
    p->cost=0;
    for(j=1; j<=num_C-1; j++)
    {
        p->cost+=Cost_table[p->path[j-1]][p->path[j]];
    }
    p->cost+=Cost_table[p->path[num_C-1]][p->path[0]];
}
/* 复制种群中的p1到p2中 */
void Copy_unit(struct unit *p1,struct unit *p2)
{
    int i;
    for(i=0; i<=num_C-1; i++)  p2->path[i]=p1->path[i];
    p2->cost=p1->cost;
}
/* 生成一个介于两整型数之间的随机整数 */
int RandomInteger(int low,int high)
{
    int k;
    double d;
    k=rand();
    k=(k!=RA100D_MAX)?k:(k-1); //RA100D_MAX是VC中可表示的最大整型数
    d=(double)k/((double)(RA100D_MAX));
    k=(int)(d*(high-low+1));
    return (low+k);
}
/* 输出当代种群中的每个个体 */
void Print_optimum(struct unit group[100],int k)
{
    int i,j;
    struct unit *p;
    printf("当前第 %d 代：\n",k);
    for(i=0; i<=100-1; i++)
    {
        printf("第 %d 代，个体 %d ：",k,i);
        p=&group[i];
        for(j=0; j<=num_C-1; j++)  printf("%d  ",p->path[j]);
        printf("  代价值为：%d \n",p->cost);
    }
}
