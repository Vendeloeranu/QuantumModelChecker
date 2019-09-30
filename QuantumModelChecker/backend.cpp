#include "backend.h"
#include<QFileInfo>
#include<QDebug>
BackEnd::BackEnd(const QString jarFileName)
{
    this->jarFile = jarFileName;
    cmd = new QProcess(this);
    connect(cmd , SIGNAL(readyReadStandardOutput()) , this , SLOT(on_readoutput()));
    connect(cmd , SIGNAL(readyReadStandardError()) , this ,SLOT(on_readerror()));

    cmd->start("bash");
    cmd->waitForStarted();
}
BackEnd::~BackEnd(){
    if(cmd){
        cmd->close();
        cmd->waitForFinished();
    }
}
bool BackEnd::getIsBusy(){
    return this->isBusy;
}
int BackEnd::setCurrentModel(const QString modelFile){
    QFileInfo fi(modelFile);
    if(!fi.isFile()) return IS_NOT_FILE;
    this->currentModel = modelFile;
    return RUN_SUCCESS;
}
int BackEnd::setCurrentFormule(const QString formuleFile){
    QFileInfo fi(formuleFile);
    if(!fi.isFile()) return IS_NOT_FILE;
    this->currentFormule = formuleFile;
    return RUN_SUCCESS;
}
void BackEnd::setCurrentFileType(const QString fileType){

    this->currentFileType = fileType;
}
QString BackEnd::getOutput(){
    return out;
}
void BackEnd::on_readoutput()
{
    emit sendOut(cmd->readAllStandardOutput().data());
    //out.append(cmd->readAllStandardOutput().data());
    //qDebug(cmd->readAllStandardOutput().data());
    //this->isBusy = false;
}

void BackEnd::on_readerror()
{
    emit sendOut(cmd->readAllStandardError().data());
    //out.append(cmd->readAllStandardError().data());
    //this->isBusy = false;
}
int BackEnd::run(){
    if(this->isBusy){
        return BUSY_STATUS;
    }

    this->isBusy = true;
    QString command = "java -jar "+this->jarFile +" "+ this->currentModel +" "+ this->currentFormule +" "+ this->currentFileType;
    cmd->write((command).toLocal8Bit() + '\n');
    this->isBusy = false;
    return RUN_SUCCESS;
}
