#include "chessboard.h"
#include <QDebug>
#include <queue>
#include<QTime>
#include<QMessageBox>


ChessBoard::ChessBoard(int mode, QWidget *parent) : QWidget(parent)
{
    qDebug()<<mode;
    this->setGeometry(0,0,column_pix+100,600);
    Go = new GoGameBoard();
    currentstone=BLACK;
    this->mode=mode;
}
void ChessBoard::mouseReleaseEvent(QMouseEvent *ev)
{
    if(isEnd()){
        if(blackwin()==1)QMessageBox::question(this, tr("游戏结束"),
                                        QString(tr("恭喜黑方获得胜利")),
                                        QMessageBox::Yes);
        else if(blackwin()==-1)QMessageBox::question(this, tr("游戏结束"),
                                        QString(tr("恭喜白方获得胜利")),
                                        QMessageBox::Yes);
        else QMessageBox::question(this, tr("游戏结束"),
                                        QString(tr("双方平局")),
                                        QMessageBox::Yes);
        this->close();
    }
    if(mode==1){//电脑下棋移动
        point p = computermove();
        if(Go->putStone(p.x,p.y,currentstone)){//放置棋子
            if(currentstone==WHITE)
                currentstone=BLACK;
            else if(currentstone==BLACK)
                currentstone=WHITE;
            this->x=row;
            this->y=column;
        }
        update();
        check();
        update();
    }

}

void ChessBoard::mousePressEvent(QMouseEvent *ev)
{
    int GridSize=(row_pix-padding*2)/(row-1);//获取棋盘间距
    size_t row=(ev->x()-padding+interval/2)/GridSize;
    size_t column=(ev->y()-padding+interval/2)/GridSize;//获取行列值

    if(Go->putStone(row,column,currentstone)){//放置棋子
        if(currentstone==WHITE)
            currentstone=BLACK;
        else if(currentstone==BLACK)
            currentstone=WHITE;
        this->x=row;
        this->y=column;
    }
    update();
    check();//检查没有气的棋子
    update();
}

void ChessBoard::paintEvent(QPaintEvent *){
    //画棋盘边框
    QPainter painter(this);
    QPen pen;
    pen.setColor(Qt::black);
    pen.setWidth(4);
    painter.setPen(pen);
    painter.drawRect(padding,padding,interval * (row -1),interval * (row -1));
    //画棋盘线
    pen.setWidth(1);
    painter.setPen(pen);
    for(size_t i=0;i<row ;i++){
        painter.drawLine(padding, padding+i*interval,column_pix - padding, padding+i*interval);
    }
    for(size_t j=0;j<column ;++j){
        painter.drawLine(padding+j*interval, padding, padding + j*interval, row_pix - padding);
    }

    //画天元
    QBrush brush;
    brush.setColor(Qt::black);
    brush.setStyle(Qt::SolidPattern);
    painter.setBrush(brush);
    int center=(row -1)/2;
    painter.drawEllipse(QPoint(padding+center*interval,padding+center*interval),4,4);
    painter.drawEllipse(QPoint(padding+3*interval,padding+3*interval),4,4);
    painter.drawEllipse(QPoint(padding+3*interval,padding+(row-4)*interval),4,4);
    painter.drawEllipse(QPoint(padding+(row-4)*interval,padding+(row-4)*interval),4,4);
    painter.drawEllipse(QPoint(padding+(row-4)*interval,padding+3*interval),4,4);


    painter.drawEllipse(QPoint(padding+3*interval,padding+center*interval),4,4);
    painter.drawEllipse(QPoint(padding+(row-4)*interval,padding+center*interval),4,4);
    painter.drawEllipse(QPoint(padding+center*interval,padding+3*interval),4,4);
    painter.drawEllipse(QPoint(padding+center*interval,padding+(row-4)*interval),4,4);


    //画棋子
    for(int i=0;i<row ;i++){
        for(int j=0;j<row ;j++){
            StoneType current;
            current= Go->checkStone(i,j);
            if(current==BLACK){
                brush.setColor(Qt::black);
                painter.setBrush(brush);
                painter.drawEllipse(QPoint((i)*interval+padding,(j)*interval+padding),15,15);
            }
            else if(current==WHITE){
                brush.setColor(Qt::white);
                painter.setBrush(brush);
                painter.drawEllipse(QPoint((i)*interval+padding,(j)*interval+padding),15,15);
            }
        }
    }
}

void ChessBoard::check()
{
    int dir[4][2]= {{-1,0},{1,0},{0,-1},{0,1}};//记录四个方向
    int u,v;
    count=1;
    //初始化vis
    for(int i=0;i<19;i++)
        for(int j=0;j<19;j++)
            vis[i][j]=false;

    std::queue<point> save;//保存要拿走的棋子

    //1.先看周围的对手的棋子是否有 没有气的区域，有的话则拿走
    for(int i=0; i<4; i++)
    {
        u=x+dir[i][0];
        v=y+dir[i][1];
        if(u<0||u>19||v<0||v>19)
            continue;

        //如果是没有访问过的对手的棋子，进行广搜
        if(Go->checkStone(u,v)!=NONE&&this->Go->checkStone(x,y)!=this->Go->checkStone(u,v)&&!vis[u][v]){

            if(go_bfs(u,v,this->Go->checkStone(u,v),save)){//当前区域有气，则把save和q清空
                while(!save.empty())
                    save.pop();

                while(!q.empty())
                    q.pop();
            }else
            {//当前区域没有气，则将save中的坐标加入json串中并拿走棋子
                while(!save.empty())
                {
                    point temp=save.front();
                    save.pop();
                    Go->removeStone(temp.x,temp.y);
                }
            }
        }
    }
    //2.再查看当前棋子的区域有没有气，没有气则移除
    if(go_bfs(x,y,this->Go->checkStone(x,y),save)){
        while(!save.empty())
            save.pop();
        while(!q.empty())
            q.pop();
    }
    while(!save.empty())
    {
        point temp=save.front();
        save.pop();
        Go->removeStone(temp.x,temp.y);
    }
}

bool ChessBoard::go_bfs(int x, int y, StoneType current, std::queue<point> &save)
{
    //广度优先搜索，如果当前一片区域的棋子没有气，则remove
    int dir[4][2]= {{-1,0},{1,0},{0,-1},{0,1}};//记录四个方向
    point temp;

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
            if(this->Go->checkStone(u,v)!=BLACK&&this->Go->checkStone(u,v)!=WHITE)
                return true;
            //否则 将坐标u，v加入队列
            else if(this->Go->checkStone(u,v)==current&&!vis[u][v])
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

point ChessBoard::computermove()
{
    point rp;
    rp.x=rp.y=-1;
    int max=0,t;
    for(int i =0;i<19;i++){
        for(int j=0;j<19;j++){
            if(Go->board[i][j]==NONE){
                Go->putStone(i,j,currentstone);
                t=getcount();
                Go->removeStone(i,j);
                if(t>max){
                    max=t;
                    rp.x=i;
                    rp.y=j;
                }
            }
        }
    }
    if(max>0)return rp;
    return getrandom();
}

point ChessBoard::getrandom()
{
    int x,y;
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    x=qrand()%19;
    y=qrand()%19;
    while(!Go->putStone(x,y,NONE)){
        x=qrand()%19;
        y=qrand()%19;
    }
    point p;
    p.x=x;
    p.y=y;
    return p;
}

int ChessBoard::getcount()
{
    int dir[4][2]= {{-1,0},{1,0},{0,-1},{0,1}};//记录四个方向
    int u,v;
    int record =0;
    //初始化vis
    for(int i=0;i<19;i++)
        for(int j=0;j<19;j++)
            vis[i][j]=false;

    std::queue<point> save;//保存要拿走的棋子

    //1.先看周围的对手的棋子是否有 没有气的区域，有的话则拿走
    for(int i=0; i<4; i++)
    {
        u=x+dir[i][0];
        v=y+dir[i][1];
        if(u<0||u>19||v<0||v>19)
            continue;

        //如果是没有访问过的对手的棋子，进行广搜
        if(Go->checkStone(u,v)!=NONE&&this->Go->checkStone(x,y)!=this->Go->checkStone(u,v)&&!vis[u][v]){

            if(go_bfs(u,v,this->Go->checkStone(u,v),save)){//当前区域有气，则把save和q清空
                while(!save.empty())
                    save.pop();

                while(!q.empty())
                    q.pop();
            }else
            {//当前区域没有气
                record+=save.size();
                while(!save.empty())
                {
                    save.pop();
                }
            }
        }
    }
    //2.再查看当前棋子的区域有没有气，没有气则移除
    if(go_bfs(x,y,this->Go->checkStone(x,y),save)){
        while(!save.empty())
            save.pop();
        while(!q.empty())
            q.pop();
    }
    while(!save.empty())
    {
        save.pop();
    }
    return record;
}

bool ChessBoard::isEnd()
{

    for(int i=0;i<19;i++){
        for(int j=0;j<19;j++){
            if(Go->board[i][j]==NONE){
                std::queue<point> save;
                if(go_bfs(i,j,currentstone,save)){
                    return false;
                }
            }
        }
    }
    return true;
}

int ChessBoard::blackwin()
{
    int b,w;
    b=w=0;
    for(int i=0;i<19;i++){
        for(int j=0;j<19;j++){
            if(Go->board[i][j]==BLACK)b++;
            else if(Go->board[i][j]==WHITE)w++;
        }
    }
    if(b>w)return 1;
    else if(b==w)return 0;
    else return -1;
}
void ChessBoard::setRow(size_t row){
    this->row = row;
}
void ChessBoard::setcolumn(size_t column){
    this->column = column;
}

void ChessBoard::removeStone(size_t x, size_t y){
    Go->removeStone(x,y);
    update();
}

void ChessBoard::setCurrentStone(StoneType stone){
    this->currentstone=stone;
}
StoneType ChessBoard::getCurrentStone(){
    return currentstone;
}
