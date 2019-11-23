#include "quantummain.h"

#include <QApplication>
#include <QDir>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QuantumMain w;
    QDir dir(qApp->applicationDirPath());
    if(!dir.cd("lib")){
        QMessageBox::warning(&w, "Worning", "does not have \"\\lib\" directory!");
        return -7;
    }
    QFileInfo info(dir.absoluteFilePath("epmc-qmc.jar"));
    if(!info.isFile()){
        QMessageBox::warning(&w, "Worning", "lack of \"epmc-qmc.jar\" in \"\\lib\"!");
        return -7;
    }
    w.show();
    return a.exec();
}
