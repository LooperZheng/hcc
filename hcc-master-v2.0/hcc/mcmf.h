#ifndef __MCMF_H__
#define __MCMF_H__
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

using namespace std;
#define INF 100000000
#define N 100000
#define PATHMAX 50000
extern map<int,int> myIDtoNetID;
extern  map<int,int> netToCus;

struct result
{
  int pathNum;
  int cost;
  bool isValid;
  vector<vector<int>> path;
};
struct Edge
{
    int from,to,cap,flow,cost;
    Edge(int u,int v,int ca,int f,int co){from=u; to=v; cap=ca; flow=f; cost=co;}
    //Edge(int u,int v,int ca,int f,int co):from(u),to(v),cap(ca),flow(f),cost(co){}
};

struct MCMF
{

    vector<vector<int>> path;
    int pathNum;
    int netNodeNum;
    int cost;
    int flow;
    bool isNetNumValid = true;
    int n,m,s,t;
    vector<Edge> edgesTmp;
    vector<Edge> edges;
    vector<int> G[N]; //网络,临接表.存放vector的数组
    int inq[N];//是否在队列中
    int d[N];//距离
    int p[N];//上一条弧
    int a[N];//可改进量

    void init(int n,int netNodeNum);//初始化
    void addedge(int from,int to,int cap,int cost);//加边
    void deletedge(int from,int to);//删除边
    bool SPFA(int s,int t);//寻找最小费用的增广路，使用引用同时修改原flow,cost
    void saveEdges();
    result MincotMaxflow(int s,int t,int flowSum);
};

void MCMF::saveEdges()
{
    edgesTmp.assign(edges.begin(),edges.end());
    //cout<<"edgesTmp.size() "<<edges.size()<<" "<<edgesTmp.size()<<endl;
}

void MCMF::init(int n,int netNodeNum)//初始化
{

    this->pathNum = 0;
    this->flow =0;
    this->cost =0;
    this->isNetNumValid =true;
    this->path.resize(PATHMAX);
    this->n=n;
    this->netNodeNum = netNodeNum;

    for(int i=0;i<n;i++)
        G[i].clear();
    edges.clear();
}

void MCMF::addedge(int from,int to,int cap,int cost)//加边
{
    edges.push_back(Edge(from,to,cap,0,cost));
    edges.push_back(Edge(to,from,0,0,-cost));//??? cap == 0?
    //vector的size为每个点的出度，每个value为对应Edges存储边的下标
    int m=edges.size();
    G[from].push_back(m-2);
    G[to].push_back(m-1);
}

void MCMF::deletedge(int from,int to)//删除边
{
//    edges.pop_back();
//    edges.pop_back();
    G[from].pop_back();
    G[to].pop_back();
}

bool MCMF::SPFA(int s,int t)//寻找最小费用的增广路，使用引用同时修改原flow,cost
{
    for(int i=0;i<n;i++)
        d[i]=INF;

    memset(inq,0,sizeof(inq));
    d[s]=0;inq[s]=1;p[s]=0;a[s]=INF;

    queue<int> Q;
    Q.push(s);
    while(!Q.empty())
    {
        int u=Q.front();
        Q.pop();
        inq[u]--;
        //广度优先 BFS
        for(int i=0;i<G[u].size();i++)
        {
            Edge& e=edges[G[u][i]];
            if(e.cap>e.flow && d[e.to]>d[u]+e.cost)//满足可增广且可变短
            {
                d[e.to]=d[u]+e.cost;
                p[e.to]=G[u][i];
                //可改进量
                a[e.to]=min(a[u],e.cap-e.flow);
                //是否在队列中
                if(!inq[e.to])
                {
                    inq[e.to]++;
                    Q.push(e.to);
                }
            }
        }
    }

    //DFS

    if(d[t]==INF) return false;//汇点不可达则退出
    flow+=a[t];
    cost+=d[t]*a[t];//
    int u=t;
    path[pathNum].push_back(a[t]); //该条路径上的带宽
    bool flag=0;
    while(u!=s)
    {
        //更新正向边和反向边
        edges[p[u]].flow+=a[t];
        edges[p[u]^1].flow-=a[t]; // ^ 按位异或.整数与1按位异或，偶数减一，奇数加一
        u=edges[p[u]].from;

        //保存本次路径上的各个网络节点
        if(!flag)
        {
            path[pathNum].push_back(netToCus[u]);
            path[pathNum].push_back(myIDtoNetID[u]);
            flag=1;
        }
        else if(u<netNodeNum)
        {
            path[pathNum].push_back(myIDtoNetID[u]);
        }
    }
    reverse(path[pathNum].begin(),path[pathNum].end());

    if(path[pathNum].size()>1001)
    {
        isNetNumValid = false;
        return false;
    }
    else
        return true;
}

result MCMF::MincotMaxflow(int s,int t,int flowSum)
{
    //cout<<"-----Edges.size() "<<edges.size()<<endl;
    this->pathNum = 0;
    this->flow =0;
    this->cost =0;
    this->isNetNumValid =true;
    this->path.clear();
    this->path.resize(PATHMAX);

    result res;
    while(SPFA(s,t))
    {
        pathNum++;
        if(pathNum>50000)
        {
            res.isValid = false;
            this->edges.clear();
            this->edges.assign(edgesTmp.begin(),edgesTmp.end());
            return res;
        }
    }

    this->edges.clear();
    this->edges.assign(edgesTmp.begin(),edgesTmp.end());
    //cout<<"-----after clear edges.size() "<<edges.size()<<endl;

    res.cost =cost;
    res.path = path;
    res.pathNum =pathNum;
    if( isNetNumValid && flowSum == flow)
    {
        res.isValid=true;
    }
    else
    {
        res.isValid=false;
    }

    return res;
}



#endif
