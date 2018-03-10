#ifndef GAMEBOARD_H
#define GAMEBOARD_H
#include<QDebug>
enum StoneType {
    NONE,BLACK, WHITE
};
struct point{
    int x;
    int y;
};
template <int N>
class GameBoard
{   
public:
    StoneType board[N][N];
    int size;
    GameBoard();
    bool putStone(int x, int y, StoneType stone);
    bool removeStone(int x, int y);
    StoneType checkStone(int x, int y) const;

    static int boardSize() ;
    bool go_bfs(int x,int y,StoneType current,std::queue<point> &save);
    bool go_bfs(int x, int y, StoneType current);
};

typedef GameBoard<19> GoGameBoard;

template <int N>
GameBoard<N>::GameBoard()
{
    for(int i=0;i<N;i++){
        for(int j=0;j<N;j++){
            board[i][j]=NONE;
        }
    }
}
template <int N>
bool GameBoard<N>::putStone(int x, int y, StoneType stone){
    if(x<N && y<N){
        if(checkStone(x,y) == NONE){//当前位置没有棋子
            if(stone==NONE)board[x][y]=stone;
            else{
                if(go_bfs(x,y,stone)){
                    board[x][y]=stone;
                }else return false;
            }
            return true;
        }
    }
    return false;
}

template <int N>
bool GameBoard<N>::removeStone(int x, int y){
    if(x<N&&y<N){
        board[x][y]=NONE;
        return true;
    }
    return false;
}

template <int N>
StoneType GameBoard<N>::checkStone(int x, int y) const{
    return board[x][y];//获取当前位置棋子类型
}


template <int N>
int GameBoard<N>::boardSize(){
    return N;
}

template<int N>
bool GameBoard<N>::go_bfs(int x, int y, StoneType current)
{
    //广度优先搜索，如果当前一片区域的棋子没有气，则remove
    int dir[4][2]= {{-1,0},{1,0},{0,-1},{0,1}};//记录四个方向
    point temp;
    std::queue<point> save;
    std::queue<point> q;
    bool vis[20][20];
    for(int i=0;i<20;i++)
        for(int j=0;j<20;j++)
            vis[i][j]=false;
    //把当前坐标加入队列
    temp.x=x,temp.y=y;
    q.push(temp);
    save.push(temp);
    while(!q.empty())
    {
        //获取队头坐标
        temp=q.front();
        q.pop();
        x=temp.x;
        y=temp.y;
        vis[x][y]=true;
        //对当前坐标的周围进行查看
        for(int i=0; i<4; i++)
        {
            int u,v;
            u=x+dir[i][0];
            v=y+dir[i][1];
            //坐标 u,v 没有棋子 代表有气存在 返回true
            if(checkStone(u,v)!=BLACK&&checkStone(u,v)!=WHITE)
                return true;
            //否则 将坐标u，v加入队列
            else if(checkStone(u,v)==current&&!vis[u][v])
            {
                temp.x=u;
                temp.y=v;
                q.push(temp);
                save.push(temp);
            }
        }//end for
    }
    return false;
}

#endif // GAMEBOARD_H
