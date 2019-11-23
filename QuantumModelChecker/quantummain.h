#ifndef QUANTUMMAIN_H
#define QUANTUMMAIN_H

#include <QMainWindow>
#include <QFileInfo>
#include "highlighter.h"
#include "textedit.h"
#include "backend.h"
#include "project.h"
#include "projectmodel.h"
#include "file.h"

#define CREATE_FILE_SUCCESS 0
#define CREATE_FILE_FAILED -1


QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QTextEdit;
class QSessionManager;
class QDirModel;
class QAbstractItemModel;
QT_END_NAMESPACE
class TextEdit;


QT_BEGIN_NAMESPACE
namespace Ui { class QuantumMain; }

QT_END_NAMESPACE
class QuantumMain : public QMainWindow
{
    Q_OBJECT

public:
    explicit QuantumMain(QWidget *parent = nullptr);
    ~QuantumMain();
    void loadFile(const QString &fileName);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void newModelFile();
    void newFormuleFile();
    void openModelFile();
    void openFormuleFile();
    void open();
    QString save();
    QString saveAs();
    void about();
    void documentWasModified();
    void removeSubTab(int index);
    void changeSubTab(int index);
    void treeViewDoubleClick(const QModelIndex & index);
    void changeFileType(QAction *a);
    int newProject();
    int openProject();
    void closeProject();
    void Run();
    void on_readoutput(char * out);


    void on_filterLevel_activated(int index);

    void on_File_tabCloseRequested(int index);

private:
    Ui::QuantumMain *ui;
    BackEnd * backend;
    QAbstractItemModel *modelFromFile(const QString& fileName,QCompleter *completer);
    void initUI();

    bool newFile(QFileInfo info, Project * p);
    int createFile(QString filePath,QString fileName);
    void createActions();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    bool maybeSave();
    int isLoaded(QFileInfo & file);
    void addLoadedFile(QFileInfo  file);
    QString saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    void textEditReconnect();
    QString strippedName(const QString &fullFileName);
    QList<QString> filter(QString out);

    Highlighter *highlighter;
    QDirModel *dirModel=nullptr;
    QAction *copyAct=nullptr,*cutAct=nullptr,*pasteAct=nullptr;
    TextEdit *curtextEdit=nullptr;
    QString curFile="";
    int num=1;

    QList<File> * files = new QList<File>();
    int currentProject = -1;
    QList<Project*> projects;
    ProjectModel * projectModel;

    bool isFinalResult = false;

    /**************************ui****************************************/

    QAction * runAction;

    int filterLevel = 0;

};
#endif // QUANTUMMAIN_H
