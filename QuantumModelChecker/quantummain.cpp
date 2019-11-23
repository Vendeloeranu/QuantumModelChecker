#include "quantummain.h"
#include "ui_quantummain.h"
#include "textedit.h"
#include<QFileSystemModel>
#include<QDirModel>
#include<QDir>
#include<QDesktopWidget>
#include<QTabWidget>
#include<QLabel>
#include<QFileDialog>
#include<QDesktopServices>
#include<QMessageBox>
#include<QTextStream>
#include<QCompleter>
#include<QAbstractTableModel>
#include<QStringListModel>
#include<QDebug>
#include<QInputDialog>
#include "choosepath.h"
#include "file.h"

QuantumMain::QuantumMain(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QuantumMain)
{
    ui->setupUi(this);
    this->initUI();
    //curtextEdit=ui->defaulttextEdit;

    connect(ui->File,SIGNAL(tabCloseRequested(int)),this,SLOT(removeSubTab(int)));
    connect(ui->File,SIGNAL(currentChanged(int)),this,SLOT(changeSubTab(int)));
    connect(ui->DirTree,SIGNAL(doubleClicked(const QModelIndex)),this,SLOT(treeViewDoubleClick(const QModelIndex)));
    ui->File->setTabsClosable(true);
    createActions();


    QTreeView *treeViewDir=ui->DirTree;
    projectModel = new ProjectModel();
    //dirModel=new QDirModel();
    //dirModel->setSorting(QDir::Type);
    treeViewDir->setModel(projectModel);
    //projectModel->addProject( * new File("path","name"));
    //treeViewDir->setRootIndex(dirModel->index("/home/"));
    treeViewDir->showColumn(0);
    treeViewDir->hideColumn(1);
    //treeViewDir->hideColumn(2);
    QDir dir(qApp->applicationDirPath());
    dir.cd("lib");
   //QFile::copy(info.absoluteFilePath(),"/home/xuechao/xuechao/work/Lab/QuantumProject/epmc.jar");
    backend = new BackEnd(dir.absoluteFilePath("epmc-qmc.jar"));
    connect(backend, SIGNAL(sendOut(char * )), this,SLOT(on_readoutput(char *)) );

}
void QuantumMain::initUI()
{
    setWindowIcon(QIcon(":/new/pic/resources/icon.png"));
    this->setWindowState(Qt::WindowMaximized);
    QSplitter* splitterH = ui->splitter_H;
    QSplitter * splitterV = ui->splitter_V;
    splitterH->setStretchFactor(0,1);
    splitterH->setStretchFactor(1,4);
    splitterV->setStretchFactor(splitterV->indexOf(ui->File),1000);
    splitterV->setStretchFactor(splitterV->indexOf(ui->V_out),1);
}

QuantumMain::~QuantumMain()
{
    delete ui;
}
int QuantumMain::isLoaded(QFileInfo & file){
    for(int i=0;i<this->files->size();i++){
        File tmp = this->files->at(i);
        if(file.absoluteFilePath().compare(tmp.getAbsFileName()) == 0) return i;
    }
    return -1;
}
void QuantumMain::addLoadedFile(QFileInfo file){
    this->files->append(file);
}

void QuantumMain::loadFile(const QString &fileName)
{
    QFile file(fileName);
    QFileInfo fileInfo(file);
    if(fileInfo.isDir()) return;

    if(!newFile(fileInfo, this->projects.at(this->currentProject))) return;
//    QLabel *storePath;
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName), file.errorString()));
        return;
    }

    QTextStream in(&file);
#ifndef QT_NO_CURSOR
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    curtextEdit->setPlainText(in.readAll());
//    ui->editortabWidget->setTabText(ui->editortabWidget->currentIndex(),fileInfo.fileName());
//    storePath=ui->editortabWidget->currentWidget()->findChild<QLabel*>();
//    storePath->setText(fileName);
#ifndef QT_NO_CURSOR
    QGuiApplication::restoreOverrideCursor();
#endif
    file.close();
    statusBar()->showMessage(tr("File loaded"), 2000);
}

void QuantumMain::createActions()
{

    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QToolBar *fileToolBar = addToolBar(tr("File"));
    const QIcon newIcon = QIcon::fromTheme("document-new", QApplication::style()->standardIcon((enum QStyle::StandardPixmap)25));
    QMenu * newMenu = fileMenu->addMenu(tr("&New"));
    newMenu->setIcon(newIcon);
    const QIcon * newModelIcon = new QIcon(":/new/pic/resources/newmodel.png");
    QAction *newModelAct = new QAction(*newModelIcon, tr("&New Model"), this);
    //newModelAct->setShortcuts(QKeySequence::New);
    newModelAct->setStatusTip(tr("Create a model file"));
    connect(newModelAct, &QAction::triggered, this, &QuantumMain::newModelFile);

    const QIcon * newFormuleIcon = new QIcon(":/new/pic/resources/newformule.png");
    QAction *newFormulelAct = new QAction(*newFormuleIcon, tr("& New Property"), this);
    //newFormulelAct->setShortcuts(QKeySequence::New);
    newFormulelAct->setStatusTip(tr("Create a property file"));
    connect(newFormulelAct, &QAction::triggered, this, &QuantumMain::newFormuleFile);
    newMenu->addAction(newModelAct);
    newMenu->addAction(newFormulelAct);
   // fileToolBar->addAction(newAct);

    fileToolBar->addAction(newModelAct);
    fileToolBar->addAction(newFormulelAct);


//! [19]
    const QIcon openIcon = QIcon::fromTheme("document-open", QApplication::style()->standardIcon((enum QStyle::StandardPixmap)42));
    //QAction *openAct = new QAction(openIcon, tr("&Open..."), this);
    //openAct->setShortcuts(QKeySequence::Open);
    //openAct->setStatusTip(tr("Open an existing file"));
    //connect(openAct, &QAction::triggered, this, &QuantumMain::open);

    QMenu * openMenu = fileMenu->addMenu(tr("&Open"));
    openMenu->setIcon(openIcon);
    const QIcon * openModelIcon = new QIcon(":/new/pic/resources/openmodel.png");
    QAction *openModelAct = new QAction(* openModelIcon, tr("&Open Model"), this);
    openModelAct->setShortcuts(QKeySequence::New);
    openModelAct->setStatusTip(tr("Open a new  model file"));
    connect(openModelAct, &QAction::triggered, this, &QuantumMain::openModelFile);
    const QIcon * openFormuleIcon = new QIcon(":/new/pic/resources/openformule.png");
    QAction *openFormuleAct = new QAction(*openFormuleIcon, tr("&Open Property"), this);
    //newFormulelAct->setShortcuts(QKeySequence::New);
    openFormuleAct->setStatusTip(tr("Create a new  property file"));
    connect(openFormuleAct, &QAction::triggered, this, &QuantumMain::openFormuleFile);
    openMenu->addAction(openModelAct);
    openMenu->addAction(openFormuleAct);
    //fileMenu->addAction(openAct);

    fileToolBar->addAction(openModelAct);
    fileToolBar->addAction(openFormuleAct);
//! [18] //! [19]

    const QIcon saveIcon = QIcon::fromTheme("document-save", QApplication::style()->standardIcon((enum QStyle::StandardPixmap)43));
    QAction *saveAct = new QAction(saveIcon, tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, &QAction::triggered, this, &QuantumMain::save);
    fileMenu->addAction(saveAct);
    fileToolBar->addAction(saveAct);

    const QIcon saveAsIcon = QIcon::fromTheme("document-save-as");
    QAction *saveAsAct = fileMenu->addAction(saveAsIcon, tr("Save &As..."), this, &QuantumMain::saveAs);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save the document under a new name"));

//! [20]

    fileMenu->addSeparator();

    const QIcon exitIcon = QIcon::fromTheme("application-exit");
    QAction *exitAct = fileMenu->addAction(exitIcon, tr("E&xit"), this, &QWidget::close);
    exitAct->setShortcuts(QKeySequence::Quit);
//! [20]
    exitAct->setStatusTip(tr("Exit the application"));

//! [21]
    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
    QToolBar *editToolBar = addToolBar(tr("Edit"));
//!
#ifndef QT_NO_CLIPBOARD
    const QIcon cutIcon = QIcon::fromTheme("edit-cut");
    cutAct = new QAction(cutIcon, tr("Cu&t"), this);
//! [21]
    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setStatusTip(tr("Cut the current selection's contents to the "
                            "clipboard"));
    connect(cutAct, &QAction::triggered, curtextEdit, &TextEdit::cut);
    editMenu->addAction(cutAct);
    editToolBar->addAction(cutAct);

    const QIcon copyIcon = QIcon::fromTheme("edit-copy");
    copyAct = new QAction(copyIcon, tr("&Copy"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    connect(copyAct, &QAction::triggered, curtextEdit, &TextEdit::copy);
    editMenu->addAction(copyAct);
    editToolBar->addAction(copyAct);

    const QIcon pasteIcon = QIcon::fromTheme("edit-paste");
    pasteAct = new QAction(pasteIcon, tr("&Paste"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));
    connect(pasteAct, &QAction::triggered, curtextEdit, &TextEdit::paste);
    editMenu->addAction(pasteAct);
    editToolBar->addAction(pasteAct);

    menuBar()->addSeparator();

#endif // !QT_NO_CLIPBOARD

    QMenu *settingMenu = menuBar()->addMenu(tr("&Set"));
    QMenu * fileTypeMenu = new QMenu(tr("FileType"),this);
    QActionGroup *fileTypegroup=new QActionGroup(this);
    QAction * prismType= new QAction(tr("&Prism"),this);
    QAction * janiType= new QAction(tr("&JANI"),this);
    prismType->setCheckable(true);
    janiType->setCheckable(true);
    fileTypegroup->addAction(prismType);
    fileTypegroup->addAction(janiType);
    fileTypeMenu->addAction(prismType);
    fileTypeMenu->addAction(janiType);
    prismType->setChecked(true);
    //newAct->setShortcuts(QKeySequence::New);
    //newAct->setStatusTip(tr("Create a new file"));
    connect(fileTypegroup, SIGNAL(triggered(QAction*)), this, SLOT(changeFileType(QAction*)));
    settingMenu->addMenu(fileTypeMenu);
    //settingMenu->addActions(fileTypeMenu);
   // fileToolBar->addAction(newAct);

    QMenu *runMenu = menuBar()->addMenu(tr("&Run"));
    QToolBar *runToolBar = addToolBar(tr("Run"));
    const QIcon * run = new QIcon(":/new/pic/resources/run.png");
    runAction= new QAction(*run, tr("&Run"),this);
    //QAction * = new QAction(tr("&JSNI"),this);
    runMenu->addAction(runAction);
    connect(runAction, &QAction::triggered, this, &QuantumMain::Run);
    runToolBar->addAction(runAction);

    QMenu *projectMenu = menuBar()->addMenu(tr("&Project"));
    QToolBar *projectToolBar = addToolBar(tr("Project"));
    QAction * newAction= new QAction(tr("&New Project..."),this);
    projectMenu->addAction(newAction);
    connect(newAction, &QAction::triggered, this, &QuantumMain::newProject);
   // projectToolBar->addAction(newAction);

    QAction * openAction= new QAction(tr("&Open Project..."),this);
    projectMenu->addAction(openAction);
    connect(openAction, &QAction::triggered, this, &QuantumMain::openProject);
    //projectToolBar->addAction(openAction);

    QAction * closeAction= new QAction(tr("&Close Project"),this);
    projectMenu->addAction(closeAction);
    connect(closeAction, &QAction::triggered, this, &QuantumMain::closeProject);
    //projectToolBar->addAction(closeAction);


    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    //QAction *aboutAct = helpMenu->addAction(tr("&About"), this, &QuantumMain::about);
    //aboutAct->setStatusTip(tr("Show the application's About box"));

//! [22]

    //QAction *aboutQtAct = helpMenu->addAction(tr("About &QuantumModelChecker"), qApp, &QApplication::aboutQt);
    //aboutQtAct->setStatusTip(tr("Show the QuantumModelChecker About box"));
//! [22]

//! [23]
#ifndef QT_NO_CLIPBOARD
    cutAct->setEnabled(false);
//! [23] //! [24]
    copyAct->setEnabled(false);
    connect(curtextEdit, &TextEdit::copyAvailable, cutAct, &QAction::setEnabled);
    connect(curtextEdit, &TextEdit::copyAvailable, copyAct, &QAction::setEnabled);
#endif // !QT_NO_CLIPBOARD
}

void QuantumMain::Run(){
 if(this->currentProject == -1) return;
 runAction->setEnabled(false);
 ui->filterLevel->setEditable(false);
 backend->setCurrentModel(this->projects.at(currentProject)->getModelFile());
 backend->setCurrentFormule(this->projects.at(currentProject)->getFormuleFile());
 int fileType = this->projects.at(currentProject)->getModelType();
 if(fileType == PRISM_FILE)
 {
     backend->setCurrentFileType("prism-qmc");
 }
 else if(fileType == JANI_FILE){
     backend->setCurrentFileType("jani-qmc");
 }
 ui->outText->setText("");
 ui->resultText->setText("");
 this->isFinalResult = false;
 backend->run();
 //qDebug()<<backend->getOutput()<<endl;
 //ui->outText->setText(backend->getOutput());
ui->filterLevel->setEnabled(true);
 runAction->setEnabled(true);
}
void QuantumMain::on_readoutput(char * out)
{
    QString str(out);
    QList<QString> result = this->filter(str);
    //ui->outText->append(result);
    for(QString line : result){
        if(line.indexOf("=====================================================") != -1){
            this->isFinalResult = true;
        }
        if(this->isFinalResult){
            ui->Out->setCurrentIndex(1);
            ui->resultText->insertPlainText(line);
           // ui->resultText->moveCursor(QTextCursor::End);
        }
        else{
            ui->Out->setCurrentIndex(0);
            ui->outText->insertPlainText(line);
            ui->outText->moveCursor(QTextCursor::End);
        }
    }
}
QList<QString> QuantumMain::filter(QString out){
    QList<QString> lines = out.split("\n");
    QList<QString> result;
    QString line;
    int currentLevel = 0;
    for(int i=0;i<lines.size()-1;i++){
    //foreach (line, lines) {
        line = lines.at(i);
        line = line.trimmed();
        if(line.startsWith("[Info]")){
            currentLevel = 0;
        }
        else if(line.startsWith("[Worning]")){
            currentLevel = 1;

        }
        else if(line.startsWith("[Error]"))
        {
            currentLevel = 2;
        }
        if(currentLevel >= this->filterLevel){
            result.append(line+"\n");
        }
    }
    if(currentLevel >= this->filterLevel){
        result.append(lines.at(lines.size()-1));
    }
    return result;
}

int QuantumMain::openProject()
{

    QFileDialog dialog(this);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setDirectory(QDir::home());
    QStringList nameFilters;
    nameFilters << "Project Files(*.pro)";
    dialog.setNameFilters(nameFilters);
    if (dialog.exec() != QDialog::Accepted)
        return -1;
    if(this->projects.size()>0){
        this->closeProject();
    }
    QString file =  dialog.selectedFiles().first();
    QFileInfo info(file);
    Project * p = new Project(info.path(),info.baseName());
    p->open(file);
    projects.append(p);
    this->currentProject = projects.size()-1;
    projectModel->addProject(p);
    if(p->isValidModelFile()){
        projectModel->addModel(new ProjectItem({p->getModelFileName(),p->getPath()}));
    }
    if(p->isValidFormuleFile()){
        projectModel->addFormule(new ProjectItem({p->getFormuleFileName(),p->getPath()}));
    }
    //ui->DirTree->repaint();
   // ui->DirTree->setRootIndex(dirModel->index(p->getPath()));
    return 0;
}

int QuantumMain::newProject(){
    ///QInputDialog to get the name of project
    QString dlgTitle="New Project";
    QString txtLabel="Project Name";
    QString defaultInput="NewProject";
    QLineEdit::EchoMode echoMode=QLineEdit::Normal;
    bool ok;
    QString projectName = QInputDialog::getText(this, dlgTitle,txtLabel, echoMode,defaultInput, &ok);
    if(! ok) return CREATE_FILE_FAILED;
    choosePath * path = new choosePath();
    if(path->exec() ==  QDialog::Accepted){
        if(this->projects.size()>0){
            this->closeProject();
        }
        QString projectPath = path->getValue();

        QDir dir(projectPath);
        int result = createFile(dir.absolutePath(),projectName+".pro");
        if(result == CREATE_FILE_FAILED){
            qDebug()<<"exist ";
            return CREATE_FILE_FAILED;
        }
        Project * p = new Project(projectPath,projectName);
        projects.append(p);
        this->currentProject = projects.size()-1;
        projectModel->addProject(p);

        //ui->DirTree->setRootIndex(dirModel->index(projectPath));
        return CREATE_FILE_SUCCESS;
    }
    return CREATE_FILE_FAILED;



}
int QuantumMain::createFile(QString filePath,QString fileName)
{
    QDir tempDir;
    //临时保存程序当前路径
    QString currentDir = tempDir.currentPath();
    //如果filePath路径不存在，创建它
    if(!tempDir.exists(filePath))
    {
        tempDir.mkpath(filePath);
    }
    QFile *tempFile = new QFile;
    //将程序的执行路径设置到filePath下
    tempDir.setCurrent(filePath);
    //检查filePath路径下是否存在文件fileName,如果停止操作。
    if(tempFile->exists(fileName))
    {
        qDebug()<<QObject::tr("文件存在");
        return CREATE_FILE_FAILED;
    }
    //此时，路径下没有fileName文件，使用下面代码在当前路径下创建文件
    tempFile->setFileName(fileName);
    if(!tempFile->open(QIODevice::WriteOnly|QIODevice::Text))
    {
        return CREATE_FILE_FAILED;
    }
    tempFile->close();
    //将程序当前路径设置为原来的路径
    tempDir.setCurrent(currentDir);
    return CREATE_FILE_SUCCESS;
}
void QuantumMain::closeProject(){
    if(currentProject == -1) return;
    Project * pro = projects.at(this->currentProject);
    pro->save();
    projectModel->closeProject(this->currentProject);
    if(pro->isValidModelFile()){
        //this->saveFile(pro->getModelFile());
    }
    if(pro->isValidFormuleFile()){
       // this->saveFile(pro->getFormuleFile());
    }

    projects.removeAt(this->currentProject);
    this-> currentProject = projects.size()-1;

    this->ui->File->clear();
    curtextEdit=nullptr;
    curFile="";
    this->files->clear();
}
void QuantumMain::about()
{

}

QString QuantumMain::saveAs()
{
    QFileDialog dialog(this);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setDirectory(this->projects.at(this->currentProject)->getPath());
    //dialog.setDefaultSuffix(suffix);
    if (dialog.exec() != QDialog::Accepted)
        return "";
    return saveFile(dialog.selectedFiles().first());
}

QString QuantumMain::save()
{
    if(ui->File->currentIndex()==-1)return "";
    if (curFile.isEmpty()) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

QString QuantumMain::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName),
                                  file.errorString()));
        return "";
    }

    QTextStream out(&file);
#ifndef QT_NO_CURSOR
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    out << curtextEdit->toPlainText();
#ifndef QT_NO_CURSOR
    QGuiApplication::restoreOverrideCursor();
#endif
    out.flush();

    file.close();
    //setCurrentFile(fileName);
    statusBar()->showMessage(tr("File saved"), 2000);
    return fileName;
}

void QuantumMain::setCurrentFile(const QString &fileName)
{
    QFile file(fileName);
    QFileInfo fileInfo(file);
    QLabel *storePath=nullptr;
    ui->File->setTabText(ui->File->currentIndex(),fileInfo.fileName());
    storePath=ui->File->currentWidget()->findChild<QLabel*>();
    storePath->setText(fileName);
    curtextEdit->document()->setModified(false);
    curFile=fileName;
}

void QuantumMain::open()
{
    QString fileName = QFileDialog::getExistingDirectory(this);
    //dirModel=new QDirModel();
    //dirModel->setSorting(QDir::Type);
    ui->DirTree->setModel(dirModel);
    ui->DirTree->setRootIndex(dirModel->index(fileName));
}
void QuantumMain::newModelFile(){
    if(this->currentProject == -1){
        int code = this->newProject();
        if(code == CREATE_FILE_FAILED) return;
    }
    Project * p = this->projects.at(currentProject);
    QString dlgTitle="New Model File";
    QString txtLabel="Model Name";
    QString defaultInput="NewModel";
    QLineEdit::EchoMode echoMode=QLineEdit::Normal;
    bool ok;
    QString fileName = QInputDialog::getText(this, dlgTitle,txtLabel, echoMode,defaultInput, &ok);
    QString suffix = p->getModelFileSuffix();
    if(!fileName.endsWith(suffix)){
        fileName += suffix;
    }
    if(! ok)
    {
        return ;
    }

    QString projectPath = this->projects.at(this->currentProject)->getPath();

    QDir dir(projectPath);
    int result = createFile(dir.absolutePath(),fileName);
    if(result == CREATE_FILE_FAILED){
        QMessageBox::warning(this, "Worning", "File exist!\n"+fileName);
        qDebug()<<"exist ";
        return ;
    }

    QString modelFile = dir.absoluteFilePath(fileName);
    QFileInfo info(modelFile);
    if(!this->newFile(info, p)) return;
    p->setModelFile(modelFile);
    this->projectModel->addModel(new ProjectItem({info.fileName(),info.path()}));
}
void QuantumMain::openFormuleFile(){
    if(this->currentProject == -1){
        QMessageBox::warning(this, "Worning", "No Project!\nPlease create a project.");
        return;
    }
    Project * p = this->projects.at(currentProject);

    QFileDialog dialog(this);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setDirectory(p->getPath());
    QStringList nameFilters;
    nameFilters << "Project Files(*"+p->getFormuleFileSuffix()+")";
    dialog.setNameFilters(nameFilters);
    if (dialog.exec() != QDialog::Accepted)
        return ;
    QString file =  dialog.selectedFiles().first();


    QString suffix = p->getFormuleFileSuffix();
    if(!file.endsWith(suffix)){
        QMessageBox::warning(this, "Worning", "Invalid File!\n Please open file with suffix"+suffix);
        return;
    }

    QFileInfo info(file);
    if(info.absolutePath().compare(p->getPath()) != 0){
        QDir dir(p->getPath());
        QString destFile = dir.absoluteFilePath(info.fileName());
        QFile::copy(info.absoluteFilePath(), destFile);
        QFileInfo destInfo(destFile);

        p->setFormuleFile(destFile);
        this->projectModel->addFormule(new ProjectItem({destInfo.fileName(),destInfo.path()}));
        return;

    }

    p->setFormuleFile(file);
    this->projectModel->addFormule(new ProjectItem({info.fileName(),info.path()}));
}

void QuantumMain::openModelFile(){
    if(this->currentProject == -1){
        QMessageBox::warning(this, "Worning", "No Project!\nPlease create a project.");
        return;
    }
    Project * p = this->projects.at(currentProject);

    QFileDialog dialog(this);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setDirectory(p->getPath());
    QStringList nameFilters;
    nameFilters << "Project Files(*"+p->getModelFileSuffix()+")";
    dialog.setNameFilters(nameFilters);
    if (dialog.exec() != QDialog::Accepted)
        return ;
    QString file =  dialog.selectedFiles().first();


    QString suffix = p->getModelFileSuffix();
    if(!file.endsWith(suffix)){
        QMessageBox::warning(this, "Worning", "Invalid File!\n Please open file with suffix"+suffix);
        return;
    }

    QFileInfo info(file);

    if(info.absolutePath().compare(p->getPath()) != 0){
        QDir dir(p->getPath());
        QString destFile = dir.absoluteFilePath(info.fileName());
        qDebug() << info.absoluteFilePath() << destFile;
        QFile::copy(info.absoluteFilePath(), destFile);
        QFileInfo destInfo(destFile);

        p->setModelFile(destFile);
        this->projectModel->addModel(new ProjectItem({destInfo.fileName(),destInfo.path()}));
        return;
    }

    p->setModelFile(file);
    this->projectModel->addModel(new ProjectItem({info.fileName(),info.path()}));
}
void QuantumMain::newFormuleFile(){
    if(this->currentProject == -1){
        int code = this->newProject();
        if(code == CREATE_FILE_FAILED) return;
    }
    Project * p = this->projects.at(currentProject);
    QString dlgTitle="New Formule File";
    QString txtLabel="Formule Name";
    QString defaultInput="NewFormule";
    QLineEdit::EchoMode echoMode=QLineEdit::Normal;
    bool ok;
    QString fileName = QInputDialog::getText(this, dlgTitle,txtLabel, echoMode,defaultInput, &ok);
    if(! ok)
    {
        return ;
    }

    QString suffix = p->getFormuleFileSuffix();
    if(!fileName.endsWith(suffix)){
        fileName += suffix;
    }
    QString projectPath = p->getPath();

    QDir dir(projectPath);
    int result = createFile(dir.absolutePath(),fileName);
    if(result == CREATE_FILE_FAILED){
        QMessageBox::warning(this, "Worning", "File exists!\n"+fileName);
        qDebug()<<"exist ";
        return ;
    }

    QString formuleFile = dir.absoluteFilePath(fileName);
    QFileInfo info(formuleFile);
    if(!this->newFile(formuleFile, p)) return;
    this->projects.at(currentProject)->setFormuleFile(formuleFile);
    this->projectModel->addFormule(new ProjectItem({info.fileName(),info.path()}));
}

bool QuantumMain::newFile(QFileInfo info, Project * p)
{
    int index = this->isLoaded(info);
    if(index != -1)
    {
        ui->File->setCurrentIndex(index);
        setCurrentFile(info.absoluteFilePath());
        return false;
    }
    this->addLoadedFile(info);

    QFont font;
    font.setFamily("Courier");
    font.setFixedPitch(true);
    font.setPointSize(12);

    QWidget *qnewTab=new QWidget(ui->File);
    QVBoxLayout *layout=new QVBoxLayout(qnewTab);
    TextEdit *newTextEdit=new TextEdit(info.absoluteFilePath(),qnewTab);
    QCompleter *completer=new QCompleter(qnewTab);
    newTextEdit->setFont(font);
    QLabel *newLabel=new QLabel(qnewTab);
    newLabel->setVisible(false);
    newTextEdit->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);

    completer->setModel(modelFromFile(":/resources/wordlist.txt",completer));

    completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setWrapAround(false);
    newTextEdit->setCompleter(completer);


    layout->addWidget(newTextEdit);
    ui->File->addTab(qnewTab,info.fileName());
    newLabel->setText("");
    ui->File->setCurrentWidget(qnewTab);
    setCurrentFile(info.absoluteFilePath());
    connect(newTextEdit->document(), &QTextDocument::contentsChanged,
            this, &QuantumMain::documentWasModified);
    connect(p, &Project::saveProject, newTextEdit, &TextEdit::save);
    return true;
}

QAbstractItemModel *QuantumMain::modelFromFile(const QString& fileName,QCompleter *completer)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly))
        return new QStringListModel(completer);

#ifndef QT_NO_CURSOR
    QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
#endif
    QStringList words;

    while (!file.atEnd()) {
        QByteArray line = file.readLine();
        if (!line.isEmpty())
            words << line.trimmed();
    }

#ifndef QT_NO_CURSOR
    QGuiApplication::restoreOverrideCursor();
#endif
    return new QStringListModel(words, completer);
}

void QuantumMain::textEditReconnect()
{
    connect(cutAct, &QAction::triggered, curtextEdit, &TextEdit::cut);
    connect(copyAct, &QAction::triggered, curtextEdit, &TextEdit::copy);
    connect(pasteAct, &QAction::triggered, curtextEdit, &TextEdit::paste);
    connect(curtextEdit, &TextEdit::copyAvailable, cutAct, &QAction::setEnabled);
    connect(curtextEdit, &TextEdit::copyAvailable, copyAct, &QAction::setEnabled);
}

void QuantumMain::documentWasModified()
{

   //ui->File->currentWidget()->setWindowModality(curtextEdit->document()->isModified());
   // ui->editortabWidget->currentWidget()->setWindowModified(curtextEdit->document()->isModified());
}

void QuantumMain::changeFileType(QAction *a)
{
    a->setChecked(true);
    if(this->currentProject == -1) return;
    if(a->text() == "&Prism"){
        this->projects.at(currentProject)->setModelType(PRISM_FILE);
    }
    else if(a->text() == "&JANI"){
        this->projects.at(currentProject)->setModelType(JANI_FILE);
    }
}

void QuantumMain::closeEvent(QCloseEvent *event)
{
    while(this->projects.size()>0){
        this->closeProject();
    }
}

void QuantumMain::removeSubTab(int index)
{
    curtextEdit = nullptr;
    curFile = "";
    ui->File->removeTab(index);
}

void QuantumMain::changeSubTab(int index)
{
    if(curtextEdit!=nullptr)disconnect(curtextEdit,nullptr,nullptr,nullptr);
    if(cutAct!=nullptr)disconnect(cutAct,nullptr,nullptr,nullptr);
    if(copyAct!=nullptr)disconnect(copyAct,nullptr,nullptr,nullptr);
    if(pasteAct!=nullptr)disconnect(pasteAct,nullptr,nullptr,nullptr);
    if(index!=-1)
    {
        TextEdit *text=ui->File->widget(index)->findChild<TextEdit*>();
        QLabel *label=ui->File->widget(index)->findChild<QLabel*>();
        curtextEdit=text;
        curFile=label->text();
        textEditReconnect();
        highlighter=new Highlighter(curtextEdit->document());
    }
}

void QuantumMain::treeViewDoubleClick(const QModelIndex & index)
{
    //curtextEdit->setText(dirModel->filePath(index));
    //loadFile(dirModel->filePath(index));
    QString file = projectModel->get(index);
    if(file.compare("") == 0){
        return;
    }
    loadFile(file);
}


void QuantumMain::on_filterLevel_activated(int index)
{
    this->filterLevel = index;
}

void QuantumMain::on_File_tabCloseRequested(int index)
{
    this->files->removeAt(index);
}
