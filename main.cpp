#include "chessboard.h"
#include "choosedlg.h"
#include <QApplication>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ChooseDlg dlg;
    if(dlg.exec() != QDialog::Accepted)
        return 0;//选择游戏模式
    ChessBoard w(dlg._selected);
    w.show();

    return a.exec();
}
