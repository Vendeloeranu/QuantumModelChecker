#include "choosepath.h"
#include "ui_choosepath.h"
#include "QString"
#include "QDir"
#include "QFileDialog"
#include "QDateTime"

choosePath::choosePath(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::choosePath)
{
    ui->setupUi(this);
    this->setWindowTitle("Choose Project Path");
}

choosePath::~choosePath()
{
    delete ui;
}

void choosePath::on_choose_clicked()
{
    QDir dir;
    QString PathName = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                 "E:/Myworkspace/data/",
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);

    ui->text->setText(PathName);    //文件名称显示
    dir.mkdir(PathName);    //在目录下创建新文件夹
}


void choosePath::on_result_accepted()
{
    value = ui->text->text();
    this->accept();
}

void choosePath::on_result_rejected()
{
    this->reject();
}
QString choosePath::getValue(){
    return this->value;
}
