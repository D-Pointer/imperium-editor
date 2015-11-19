#include <QApplication>
#include "EditorMainWindow.hpp"

int main(int argc, char *argv[]) {

    // random seed
    qsrand( time( 0 ) );

    QApplication a(argc, argv);
    EditorMainWindow w;
    w.show();
    
    return a.exec();
}
