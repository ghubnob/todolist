#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QCheckBox>
#include <QKeyEvent>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDebug>
#include <QHBoxLayout>
#include <QPushButton>
#include <QGraphicsDropShadowEffect>
#include <QMessageBox>
#include "mycheckbox.h"

mainwindow::mainwindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::mainwindow)
{
    ui->setupUi(this);
    setupDatabase();
    loadListFromDatabase();
    setWindowTitle("TO-DO LIST");

    if (ui->formLayout->count()==0) ui->label_emptyList->setText("There are no tasks here..");
}

mainwindow::~mainwindow()
{
    saveToDatabase();
    if (db.isOpen()) {
        db.close();
    }
    delete ui;
}

void mainwindow::on_pushButton_clicked()
{
    QString text = QString::number(cnt+1) + ") " + ui->lineEdit->text();
    if (text.isEmpty()) {
        return;
    }

    ui->label_emptyList->setText("");

    MyCheckBox *cb = new MyCheckBox(this);
    cb->setText(text);
    cb->setStyleSheet("font: 10pt 'Alef';");

    QPushButton *delBtn = new QPushButton("Delete");
    delBtn->setObjectName(text);
    delBtn->setFixedHeight(25);
    delBtn->setFixedWidth(50);
    delBtn->setStyleSheet("font: 10pt 'Alef';"
                          "border-style: outset;"
                          "border-color: rgba(149, 223, 223, 180);"
                          "border-width: 2px;"
                          "border-radius: 5px;"
                          "min-width: 1em;"
                          "background-color: rgba(149, 223, 223, 115)");

    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(cb);
    layout->addWidget(delBtn);

    QWidget *widget = new QWidget();
    widget->setLayout(layout);

    ui->formLayout->addWidget(widget);
    ui->lineEdit->clear();
    cnt++;

    QSqlQuery query;
    query.prepare("INSERT INTO todolist (task, clicked) VALUES (:task, :clicked)");
    query.bindValue(":task", cb->text());
    query.bindValue(":clicked", "0");
    if (!query.exec()) {
        qDebug() << "Save error:" << query.lastError().text();
    }

    connect(cb, &QCheckBox::clicked, this, [=](bool checked) {
        on_checkBox_clicked(cb->text(), checked);
    });

    connect(delBtn, &QPushButton::clicked, this, [=]() {
        on_deleteButton_clicked(cb->text(), widget);
    });

}

void mainwindow::setupDatabase()
{
    if (QSqlDatabase::contains("qt_sql_default_connection")) {
        QSqlDatabase::database().close();
        QSqlDatabase::removeDatabase("qt_sql_default_connection");
    }

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("todolist.db");
    if (!db.open()) {
        qDebug() << "Database Error:" << db.lastError().text();
        return;
    }

    QSqlQuery query;
    if (!query.exec("CREATE TABLE IF NOT EXISTS todolist ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "task TEXT, "
                    "clicked TEXT)")) {
        qDebug() << "Create table error:" << query.lastError().text();
    }
}

void mainwindow::loadListFromDatabase()
{
    QSqlQuery query("SELECT task, clicked FROM todolist");
    while (query.next()) {
        QString task = query.value(0).toString();
        QString clicked = query.value(1).toString();

        MyCheckBox *cb = new MyCheckBox(this);
        cb->setText(task);
        if (clicked == "1") {
            cb->setChecked(true);
        }
        cb->setStyleSheet("font: 10pt 'Alef';");

        QPushButton *delBtn = new QPushButton("Delete");
        delBtn->setObjectName(task);
        delBtn->setFixedHeight(25);
        delBtn->setFixedWidth(50);
        delBtn->setStyleSheet("font: 10pt 'Alef';"
                              "border-style: outset;"
                              "border-color: rgba(149, 223, 223, 180);"
                              "border-width: 2px;"
                              "border-radius: 5px;"
                              "min-width: 1em;"
                              "background-color: rgba(149, 223, 223, 115)");

        QHBoxLayout *layout = new QHBoxLayout();
        layout->addWidget(cb);
        layout->addWidget(delBtn);

        QWidget *widget = new QWidget();
        widget->setLayout(layout);

        ui->formLayout->addWidget(widget);
        cnt++;

        connect(cb, &QCheckBox::clicked, this, [=](bool checked) {
            on_checkBox_clicked(cb->text(), checked);
        });

        connect(delBtn, &QPushButton::clicked, this, [=]() {
            on_deleteButton_clicked(cb->text(), widget);
        });
    }
}

void mainwindow::on_checkBox_clicked(const QString &task, bool checked)
{
    QSqlQuery query;
    query.prepare("UPDATE todolist SET clicked=:clicked WHERE task=:task");
    query.bindValue(":clicked", checked ? "1" : "0");
    query.bindValue(":task", task);
    if (!query.exec()) {
        qDebug() << "Update error:" << query.lastError().text();
    }
}

void mainwindow::on_deleteButton_clicked(const QString &task, QWidget *widget)
{
    QSqlQuery query;
    query.prepare("DELETE FROM todolist WHERE task=:task");
    query.bindValue(":task", task);
    if (!query.exec()) {
        qDebug() << "Delete error:" << query.lastError().text();
    } else {
        delete widget;
    }
    cnt--;
    for (int i = 0; i < ui->formLayout->count(); ++i) {
        QWidget *widget = ui->formLayout->itemAt(i)->widget();
        if (widget) {
            MyCheckBox *cb = widget->findChild<MyCheckBox *>();
            cb->setText(QString::number(i + 1) + ") " + cb->text().split(") ")[1]);

            QSqlQuery updateQuery;
            updateQuery.prepare("UPDATE todolist SET task=:task WHERE task=task");
            updateQuery.bindValue(":task", QString::number(i + 1) + ") " + cb->text().split(") ")[1]);
            updateQuery.exec();
        }
    }
    if (ui->formLayout->count()==0) ui->label_emptyList->setText("There are no tasks here..");
}

void mainwindow::saveToDatabase() {
    QSqlQuery query("DELETE FROM todolist");
    for (int i=0; i < ui->formLayout->count(); ++i) {
        QWidget *widget = ui->formLayout->itemAt(i)->widget();
        if (widget) {
            MyCheckBox *cb = widget->findChild<MyCheckBox *>();
            QSqlQuery query;
            query.prepare("INSERT INTO todolist (task, clicked) VALUES (:task, :clicked)");
            query.bindValue(":task", cb->text());
            if (cb->isChecked()) query.bindValue(":clicked", "1");
            else query.bindValue(":clicked", "0");
            query.exec();
        }
    }
}

void mainwindow::on_lineEdit_selectionChanged()
{
    if (!selected) { ui->lineEdit->setText(""); selected = true; }
}


void mainwindow::on_actionnew_to_do_list_triggered()
{
    QMessageBox *msgbox = new QMessageBox();
    msgbox->setText("in development...");
    msgbox->exec();
}

void mainwindow::deleteTask(const QString &task, QWidget *widget)
{
    QWidget *wid1 = widget;
    bool waswid = false;
    QSqlQuery query;
    query.prepare("DELETE FROM todolist WHERE task=:task");
    query.bindValue(":task", task);
    if (!query.exec()) {
        qDebug() << "Delete error:" << query.lastError().text();
    } else {
        widget->deleteLater();
    }
    cnt--;
    for (int i=0; i < ui->formLayout->count(); ++i) {
        QWidget *widget = ui->formLayout->itemAt(i)->widget();
        if (widget==wid1) waswid = true;
        if (widget) {
            MyCheckBox *cb = widget->findChild<MyCheckBox *>();
            if (waswid == false) cb->setText(QString::number(i+1)+") "+cb->text().split(") ")[1]);
            else cb->setText(QString::number(i)+") "+cb->text().split(") ")[1]);
            QSqlQuery query;
            query.prepare("UPDATE todolist SET task=:task WHERE task=task");
            query.bindValue(":task", QString::number(cnt)+") "+cb->text().split(") ")[1]);
            query.exec();
        }
    }
    if (ui->formLayout->count()==1) ui->label_emptyList->setText("There are no tasks here..");
}

