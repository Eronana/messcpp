#include <cstdio>
#include <cstring>
#include <algorithm>
#include <queue>
using namespace std;
const int N=100005;
const int M=N;
const int INF=0x3f3f3f3f;
#define CLR(a,v) memset(a,v,sizeof(a))

struct
{
    int v,w,next;
}edge[M*2];
int ehead[N];
int ecnt;

void addEdge(int u,int v,int w)
{
    edge[ecnt]={v,w,ehead[u]};
    ehead[u]=ecnt++;
    edge[ecnt]={u,w,ehead[v]};
    ehead[v]=ecnt++;
}


struct Edge
{
    int u,v,w;
    bool operator<(const Edge &rhs) const
    {
        return w<rhs.w;
    }
}edge_mst[M];

int enemy[N];
int n,m,k;
int dji_m=0,mst_m=0;
int dist[N];
bool vis[N];
#define PUSH(x,w) q.push(make_pair(-(dist[x]=w),x))
int Dijkstra(int m)
{
    CLR(vis,false);
    CLR(dist,INF);
    priority_queue<pair<int,int> > q;
    PUSH(1,0);
    while(q.size())
    {
        int u=q.top().second;q.pop();
        if(!u)break;
        vis[u]=true;
        for(int i=ehead[u];~i;i=edge[i].next)
        {
            int v=edge[i].v;
            int w=edge[i].w;
            if(w>m)continue;
            int ndist=dist[u]+w;
            if(!vis[v]&&ndist<dist[v])PUSH(v,ndist);
        }
    }
    return dist[0];
}
int Int()
{
    int x;
    return scanf("%d",&x),x;
}
int Father[N];
int Find(int a)
{
    return Father[a]?Father[a]=Find(Father[a]):a;
}
int main()
{
    scanf("%d%d%d",&n,&m,&k);
    for(int i=1;i<=k;i++)enemy[Int()]=i;
    CLR(ehead,-1);
    int maxw=0;
    for(int i=0;i<m;i++)
    {
        int u,v,w;
        scanf("%d%d%d",&u,&v,&w);
        maxw=max(maxw,w);
        if(enemy[u]&&enemy[v])edge_mst[mst_m++]={enemy[u],enemy[v],w};
        else if(!enemy[u]&&!enemy[v])addEdge(u,v,w);
        else if(enemy[u]&&!enemy[v])addEdge(0,v,w);
        else addEdge(u,0,w);
    }
    dji_m=m-mst_m;
    int max_mst_w=0,mst=0;
    int x_node_cnt=k-1;
    sort(edge_mst,edge_mst+mst_m);
    for(int i=0;i<mst_m;i++)
    {
        int x=Find(edge_mst[i].u);
        int y=Find(edge_mst[i].v);
        if(x==y)continue;
        Father[x]=y;
        mst+=edge_mst[i].w;
        max_mst_w=max(max_mst_w,edge_mst[i].w);
        x_node_cnt--;
    }
    if(x_node_cnt)return puts("we need skill SSGZ"),0;
    int ans_min=INF,ans_total;
    int l=max_mst_w,r=maxw;
    while(l<=r)
    {
        int m=(l+r)>>1,ans=Dijkstra(m);
        if(ans!=INF)
        {
            ans_min=m;
            ans_total=ans;
            r=m-1;
        }
        else l=m+1;
    }
    if(ans_min==INF)puts("we need skill SSGZ");
    else printf("%d %d\n",ans_min,ans_total+mst);
    return 0;
}
