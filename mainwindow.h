#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QCheckBox>

QT_BEGIN_NAMESPACE
namespace Ui {
class mainwindow;
}
QT_END_NAMESPACE

class mainwindow : public QMainWindow
{
    Q_OBJECT

public:
    mainwindow(QWidget *parent = nullptr);
    ~mainwindow();

private slots:
    void on_pushButton_clicked();
    void on_checkBox_clicked(const QString &task, bool checked);
    void on_deleteButton_clicked(const QString &task, QWidget *widget);

    void on_lineEdit_selectionChanged();

    void on_actionnew_to_do_list_triggered();

private:
    Ui::mainwindow *ui;
    bool selected = false;
    int cnt=0;
   QSqlDatabase db;

    void setupDatabase();
    void loadListFromDatabase();
    void saveToDatabase();
};
#endif // MAINWINDOW_H
