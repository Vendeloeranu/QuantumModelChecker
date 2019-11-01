#ifndef PROJECT_H
#define PROJECT_H

#include <QString>
#include <QVariant>

#define JANI_FILE 0
#define PRISM_FILE 1

#define UNSERILIZE_FAILED -1
#define UNSERILIZE_SUCCESS 0


#define ELEMENT_NUMBER 2

class Project: public QObject
{
    Q_OBJECT
private:
    QString path;
    QString modelFile;
    QString formuleFile;
    int modelType = PRISM_FILE;
    QString name;

    bool isSetModelFile = false;
    bool isSetFormuleFile = false;
    bool isSetModelType = false;

    QString serilize();
    int unserilize(const QString & project);

signals:
    void closeFile();
    void saveProject();

public:
    Project(const QString & path, const QString & name);
    void setModelFile(const QString & modelFileName);
    void setFormuleFile(const QString & formuleFileName);
    void setModelType(int modelType);

    QString getModelFileSuffix();
    QString getFormuleFileSuffix();

    int getModelType();
    QString getModelFile();
    QString getFormuleFile();
    QString getModelFileName();
    QString getFormuleFileName();
    QString getPath();
    QString getName();

    bool isValidModelFile();
    bool isValidFormuleFile();
    bool isValidModelType();

    void save();
    void open(QString projectFile);
    QVariant get(int column) const;
    int count() const;
};

#endif // PROJECT_H
