#ifndef CHOOSEPATH_H
#define CHOOSEPATH_H

#include <QDialog>

namespace Ui {
class choosePath;
}

class choosePath : public QDialog
{
    Q_OBJECT

public:
    explicit choosePath(QWidget *parent = nullptr);
    ~choosePath();
    QString getValue();

private slots:
    void on_choose_clicked();

    void on_result_accepted();

    void on_result_rejected();


private:
    Ui::choosePath *ui;
    QString value = "";
};

#endif // CHOOSEPATH_H
