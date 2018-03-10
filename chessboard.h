#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <QObject>
#include <QLabel>
#include <QPushButton>
#include <QPainter>
#include <QPixmap>
#include <QMouseEvent>
#include <QImage>
#include <queue>
#include "gameboard.h"

class ChessBoard : public QWidget
{
    Q_OBJECT
public:
    explicit ChessBoard(int mode,QWidget *parent = 0);
    ~ChessBoard(){}
    void setStone(StoneType stone);//设置棋手的棋子颜色
    StoneType getStone();//获取棋子类型

    void setRow(size_t row);
    void setcolumn(size_t column);

    void putStone(size_t x,size_t y,StoneType stone);//在x,y坐标放棋子
    void removeStone(size_t x,size_t y);//拿掉没有气的棋子

    void setCurrentStone(StoneType stone);//设置当前下棋人的棋子
    StoneType getCurrentStone();//获取当前下棋人的棋子

    void mouseReleaseEvent(QMouseEvent *ev);
    void mousePressEvent(QMouseEvent *ev);
    void paintEvent(QPaintEvent *);
    void check();
    bool go_bfs(int x,int y,StoneType current,std::queue<point> &save);
    point computermove();
    point getrandom();
    int getcount();
    bool isEnd();
    int blackwin();//1 黑胜 0 平局 -1 白胜
signals:
    void clicked(size_t, size_t);

private:
    size_t row = 19;
    size_t column = 19;

    const size_t padding = 30;
    const size_t interval = 30;
    size_t row_pix = padding + interval * (row -1) +padding;//棋盘高度
    size_t column_pix = padding + interval * (column -1) +padding;//棋盘宽度

//    QPushButton* go_back;

    StoneType currentstone;//记录当前下的棋子颜色，控制当前用户是否下棋
    GoGameBoard *Go;

    std::queue<point> q;
    bool vis[20][20];
    int count;
    size_t x;//当前坐标
    size_t y;
    int mode;//默认人人对战，0为人机对战
};

#endif // CHESSBOARD_H
