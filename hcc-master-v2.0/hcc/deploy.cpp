#include "deploy.h"
#include <stdio.h>
#include<iostream>
#include<algorithm>
#include<string>
#include <sstream>
#include<set>
#include<vector>
#include<stack>
#include<map>
#include<queue>
#include<deque>
#include<cstdlib>
#include<cstring>
#include<cmath>
#include<ctime>
#include<functional>
#include"mcmf.h"
using namespace std;

#define INF 100000000
#define N 100000
int netNodeNum,edgeNum,cusNum,serverPrice;//网络节点个数，边个数，视频服务器价格
map<int,int> netToCus;
map<int,int> netIDtoMyID;
map<int,int> myIDtoNetID;
std::vector<int> cusNode;
///模拟退火
//求随机数
int randomi(int a, int b)
{
    int c=rand()%(b-a+1)+a;
    return c;
}
double randomf(double a, double b)
{
    double c = (double)(rand()%((int)b-(int)a)) + a + (double)(rand()/(RAND_MAX + 1.0));
    return c;
}
//判断是否接受新解
bool accept(int deta,float T)
{
    if (deta<0)
    {
        cout<<"---Find a better valid answer---"<<endl;
        return 1;
    }
    else if(((deta>0) && (exp(-deta/T)>randomf(0,1))))
    {
        double aa =exp(-deta/T);
        cout<<"deta= "<<deta<<" "<<"T= "<<T<<" "<<"(exp(-deta/T) = "<<aa<<endl;
        cout<<"****accept a not better ans****"<<endl;
        return 1;
    }
    return 0;
}
//初始解
bool compare(int a, int b)
{
    return (a>b);
}
void initAns(int netNodeOutDegree[], int initAns[],int netNodeNum)
{
//    ///initialize the solve according to the weight of out-degree
//    int netNodeOutDegreeTmp[netNodeNum];
//    memcpy(netNodeOutDegreeTmp,netNodeOutDegree,sizeof(netNodeOutDegree));
//    sort(netNodeOutDegreeTmp, netNodeOutDegreeTmp+netNodeNum,compare);
//    for(int i=0; i<netNodeNum*0.02;i++)
//    {
//        int outDegree = netNodeOutDegreeTmp[i];
//        int pos = find(netNodeOutDegree, netNodeOutDegree+netNodeNum, outDegree)-netNodeOutDegree;
//        initAns[pos]=1;
//    }

    ///initialize the direct solve
      for(int i=0;i<netNodeNum;i++)
      {
          if(find(cusNode.begin(), cusNode.end(),i) != cusNode.end())
              initAns[i]=1;
      }

//    initAns[netIDtoMyID[0]]=1;
//    initAns[netIDtoMyID[3]]=1;
//    initAns[netIDtoMyID[22]]=1;

//    initAns[0]=1;
//    initAns[8]=1;
//    initAns[17]=1;

//    initAns[netIDtoMyID[0]]=1;
//    initAns[netIDtoMyID[8]]=1;
//    initAns[netIDtoMyID[17]]=1;
}
//根据权重计算新解
void deployCal(int netNodeWeight[], int deployAns[], int netNodeNum)
{

    for(int i=0;i<netNodeNum;i++)
    {
        randomi(0,100)%10==0? deployAns[i]=1: deployAns[i]=0;
    }
//    int pos1 = randomi(0,netNodeNum-1);
//    //cout<<"pos1 = "<<pos1<<endl;
//    deployAns[pos1]==1 ? deployAns[pos1]=0 : deployAns[pos1]=1;

//    int pos2 = randomi(0,netNodeNum-1);
//    //cout<<"pos2 = "<<pos2<<endl;
//    deployAns[pos2]==1 ? deployAns[pos2]=0 : deployAns[pos2]=1;

//    int pos3 = randomi(0,netNodeNum-1);
//    //cout<<"pos3 = "<<pos3<<endl;
//    deployAns[pos3]==1 ? deployAns[pos3]=0 : deployAns[pos3]=1;

//    int pos4 = randomi(0,netNodeNum-1);
//    //cout<<"pos4 = "<<pos4<<endl;
//    deployAns[pos4]==1 ? deployAns[pos4]=0 : deployAns[pos4]=1;

//    int pos5 = randomi(0,netNodeNum-1);
//    //cout<<"pos4 = "<<pos4<<endl;
//    deployAns[pos5]==1 ? deployAns[pos5]=0 : deployAns[pos5]=1;

//    int pos6 = randomi(0,netNodeNum-1);
//    //cout<<"pos4 = "<<pos4<<endl;
//    deployAns[pos6]==1 ? deployAns[pos6]=0 : deployAns[pos6]=1;

//    int ranNum = randomi(0,netNodeWeight[netNodeNum-1]);
//    int pos= 0;
//    int ld = 0;
//    int rd = netNodeNum - 1;
//    if(ranNum < netNodeWeight[0])
//        pos = 0;
//    else
//    {
//        //binary search
//        while(rd - ld >1)
//        {
//            int mid  = (rd + ld)/2;
//            if(netNodeWeight[mid] <= ranNum)
//                ld = mid;
//            else
//                rd = mid;
//        }
//        pos = ld;
//    }
//    deployAns[pos]==1 ? deployAns[pos]=0 : deployAns[pos]=1;
}

//你要完成的功能总入口
void deploy_server(char * topo[MAX_EDGE_NUM], int line_num,char * filename)
{
    int flowSum = 0;
	stringstream str;
    str<<topo[0];
	str>>netNodeNum>>edgeNum>>cusNum;
    int sId=netNodeNum+edgeNum;//虚拟源节点ID
    int tId=netNodeNum+edgeNum+1;//虚拟汇节点ID
    cout<<"sId "<<sId<<endl;

    int netNodeOutDegree[netNodeNum];
    memset(netNodeOutDegree,0,sizeof(netNodeOutDegree));
    int netNodeWeight[netNodeNum];
    memset(netNodeWeight,0,sizeof(netNodeWeight));

	MCMF mcmf;
    mcmf.init(netNodeNum+edgeNum+2,netNodeNum);//总的节点数量 = 网络节点个数+为了消除反向边增加的节点+虚拟源节点+虚拟汇节点

	str<<topo[2];
	str>>serverPrice;
    int netNodeNumForID=0;
	for(int i=4;i<4+edgeNum;i++)//读取网络节点边的费用和带宽
	{
		str<<topo[i];
		int a,b,c,d;
        str>>a>>b>>c>>d;
        //convert ID
        auto it=netIDtoMyID.find(a);
        if(it==netIDtoMyID.end())
        {
            netIDtoMyID[a]=netNodeNumForID;
            myIDtoNetID[netNodeNumForID]=a;
            netNodeNumForID++;
        }
        it=netIDtoMyID.find(b);
        if(it==netIDtoMyID.end())
        {
            netIDtoMyID[b]=netNodeNumForID;
            myIDtoNetID[netNodeNumForID]=b;
            netNodeNumForID++;
        }

        mcmf.addedge(netIDtoMyID[a],netIDtoMyID[b],c,d);
        //双向图转换为单向图??? edgeNum-> netNodeNum
        mcmf.addedge(netIDtoMyID[b],netIDtoMyID[b]+netNodeNum,c,0);
        mcmf.addedge(netIDtoMyID[b]+netNodeNum,netIDtoMyID[a],c,d);
        //统计每个节点的出度
        netNodeOutDegree[netIDtoMyID[a]] += c;
        netNodeOutDegree[netIDtoMyID[b]] += c;
	}

	for(int i=5+edgeNum;i<line_num;i++)//读取消费节点需要带宽
	{
		str<<topo[i];
		int a,b,c;
		str>>a>>b>>c;
        mcmf.addedge(netIDtoMyID[b],tId,c,0);
        netToCus[netIDtoMyID[b]]=a;
        flowSum += c;

        cusNode.push_back(netIDtoMyID[b]);
	}
    //save the vector of edges
    mcmf.saveEdges();
    ///模拟退火
    float T = 100;
    float T_time = 0.0001;
    int L =1000;
    int deployAnsLast[netNodeNum];
    memset(deployAnsLast,0,sizeof(deployAnsLast));
    int deployAns[netNodeNum];
    memset(deployAns,0,sizeof(deployAns));
    int costLast = INF;
    int costMin = INF;
    int cdnNum =0;
    result res;
    result resMin;
    resMin.isValid =false;
    //初始化随机数种子
    srand((unsigned)time(NULL));
    //计算每个网络节点的权重（累计）
    netNodeWeight[0]=netNodeOutDegree[0];
    for(int i=1; i<netNodeNum;i++)
    {
        netNodeWeight[i] = netNodeWeight[i-1] + netNodeOutDegree[i];
    }
    //计算初始解,
    initAns(netNodeOutDegree,deployAns, netNodeNum);

    cout<<"intial solution is :"<<endl;
    for(int i=0;i<netNodeNum;i++)
        cout<<deployAns[i]<<" ";
    cout<<endl;

    while (T>T_time)
    {
        for(int i=0; i<L;i++)
        {
            //删除图中视频服务器位置
            for(int i=0;i<netNodeNum;i++)
            {
                if(deployAnsLast[i] == 1)
                {
                    //cout<<"delete net Node : "<<i<<endl;
                    mcmf.deletedge(sId,i);
                }
            }
            //添加视频服务器位置到图中
            for(int i=0;i<netNodeNum;i++)
            {
                if(deployAns[i] == 1)
                {
                    //cout<<"add net Node : "<<i<<endl;
                    mcmf.addedge(sId,i,INF,0);
                }
            }
            //费用函数
            res = mcmf.MincotMaxflow(sId,tId,flowSum);

            if(res.isValid == true)
            {
                //添加视频服务器cost
                //cout<<"---Find an valid answer---"<<endl;
                cdnNum = count(deployAns,deployAns+netNodeNum, 1);
                res.cost += (cdnNum * serverPrice);
                res.cost<costMin ? resMin = res : resMin = resMin;
                res.cost<costMin ? costMin = res.cost : costMin = costMin;
                int deta = res.cost - costLast;
                if(accept(deta, T))
                {
                    cout<<"last ans "<<costLast<<"; new ans "<<res.cost<<endl;
                    costLast = res.cost;
                }
            }
            //产生新解
            memcpy(deployAnsLast, deployAns, sizeof(deployAns));
            deployCal(netNodeWeight, deployAns, netNodeNum);
        }

        T = 0.98*T;
    }

    //打印cost，包括视频服务器cost
    if(resMin.isValid == false)
    {
        cout<<"--can not find answer!!!---"<<endl;
    }
    else
    {
        cout<<"path num is "<<resMin.pathNum<<endl;
        cout<<"The Mincost is: "<<resMin.cost<<endl;
        for(int i=0;i<resMin.pathNum;i++)
        {

            for(int j=0;j<resMin.path[i].size();j++)
            {
                cout<<' '<<resMin.path[i][j];
            }
            cout<<endl;
        }
    }


	// 需要输出的内容

	char * topo_file = (char *)"17\n\n0 8 0 20\n21 8 0 20\n9 11 1 13\n21 22 2 20\n23 22 2 8\n1 3 3 11\n24 3 3 17\n27 3 3 26\n24 3 3 10\n18 17 4 11\n1 19 5 26\n1 16 6 15\n15 13 7 13\n4 5 8 18\n2 25 9 15\n0 7 10 10\n23 24 11 23";

	// 直接调用输出文件的方法输出到指定文件中(ps请注意格式的正确性，如果有解，第一行只有一个数据；第二行为空；第三行开始才是具体的数据，数据之间用一个空格分隔开)
	write_result(topo_file, filename);

}
