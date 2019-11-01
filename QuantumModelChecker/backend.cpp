#include "backend.h"
#include<QFileInfo>
#include<QDebug>
BackEnd::BackEnd(const QString jarFileName)
{
    this->jarFile = jarFileName;
    cmd = new QProcess(this);
    connect(cmd , SIGNAL(readyReadStandardOutput()) , this , SLOT(on_readoutput()));
    connect(cmd , SIGNAL(readyReadStandardError()) , this ,SLOT(on_readerror()));
    //cmd->setWorkingDirectory("~");
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
   // QString result =QString(cmd->readAllStandardOutput().data());
    emit sendOut(cmd->readAllStandardOutput().data());
    //out.append(cmd->readAllStandardOutput().data());
    //qDebug() << result;
    //this->isBusy = false;
}

void BackEnd::on_readerror()
{
    //QString result = QString(cmd->readAllStandardError().data());
    emit sendOut(cmd->readAllStandardError().data());
    //qDebug() << result;
    //out.append(cmd->readAllStandardError().data());
    //this->isBusy = false;
}
int BackEnd::run(){
    if(this->isBusy){
        return BUSY_STATUS;
    }

    this->isBusy = true;

    QString command = "java -jar "+this->jarFile +" check --model-input-files "+ this->currentModel +
                                    " --property-input-files "+ this->currentFormule +" --model-input-type "+ this->currentFileType;
    qDebug() << command;
    cmd->write((command).toLocal8Bit() + '\n');

    //cmd->execute(command);
    this->isBusy = false;
    return RUN_SUCCESS;
}
