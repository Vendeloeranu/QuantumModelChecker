#include "project.h"
#include <QDir>
Project::Project(const QString & path, const QString & name)
{
    this->path = path;
    this->name = name;
}
QVariant Project::get(int column) const
{
    if (column < 0 || column >= ELEMENT_NUMBER)
        return QVariant();
    if(column == 0){
        return this->name;
    }
    else if(column == 1){
        return this->path;
    }
    else{
        return QVariant();
    }
}
int Project::count() const
{
    return ELEMENT_NUMBER;
}

void Project::save()
{
    QString project = this->serilize();
    QDir dir(this->path);
    QFile file(dir.absoluteFilePath(this->name+".pro"));
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    file.write(project.toUtf8());
    file.close();
}

void Project::open(QString projectFile)
{

    QFile file(projectFile);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray t = file.readAll();
    this->unserilize(QString(t));
    file.close();
}

QString Project::serilize()
{
    return this->modelFile+";"+this->formuleFile+";"+QString("%1").arg(this->modelType)+";";
}

QString Project::getModelFileName()
{
    QFileInfo info(this->modelFile);
    return info.fileName();
}
QString Project::getFormuleFileName(){
    QFileInfo info(this->formuleFile);
    return info.fileName();
}

int Project::unserilize(const QString & project)
{
    QStringList list = project.split(";");
    if(list.size()<3) return UNSERILIZE_FAILED;
    this->setModelFile(list[0]);
    this->setFormuleFile(list[1]);
    bool * ok = new bool;
    int type = list[2].toInt(ok);
    if(! *ok) return UNSERILIZE_FAILED;
    this->setModelType(type);
    return UNSERILIZE_SUCCESS;
}

void Project::setModelFile(const QString & modelFileName)
{
    this->modelFile = modelFileName;
    this->isSetModelFile= true;
}
void Project::setFormuleFile(const QString & formuleFileName)
{
    this->formuleFile = formuleFileName;
    this->isSetFormuleFile=true;
}
QString Project::getModelFileSuffix()
{
    if(this->modelType == PRISM_FILE){
        return ".prism";
    }
    else if(this->modelType == JANI_FILE){
        return ".jani";
    }
    else {
        return "";
    }
}
QString Project::getFormuleFileSuffix()
{
    return ".props";
}

QString Project::getModelFile()
{
    QDir dir(this->path);
    return dir.absoluteFilePath(this->modelFile);
}
QString Project::getFormuleFile()
{
    QDir dir(this->path);
    return dir.absoluteFilePath(this->formuleFile);
}

void Project::setModelType(int modelType)
{
    this->modelType = modelType;
    this->isSetModelType = true;
}

int Project::getModelType()
{
    return this->modelType;
}

QString Project::getPath()
{
    return this->path;
}
QString Project::getName()
{
    return this->name;
}
bool Project::isValidModelFile(){
    return this->isSetModelFile;
}
bool Project::isValidFormuleFile(){
    return this->isSetFormuleFile;
}
bool Project::isValidModelType(){
    return this->isSetModelType;
}
