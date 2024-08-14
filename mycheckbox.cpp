#include "mycheckbox.h"
#include <QMessageBox>

MyCheckBox::MyCheckBox(mainwindow *parentWindow, QWidget *parent)
    : QCheckBox(parent), m_parentWindow(parentWindow) {}

void MyCheckBox::contextMenuEvent(QContextMenuEvent *event) {
    QMenu contextMenu(this);
    QAction *deleteAction = contextMenu.addAction("Delete task");

    connect(deleteAction, &QAction::triggered, this, [=]() {
        // QMessageBox *msgb = new QMessageBox;
        // msgb->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        // msgb->setStyleSheet("background: grey;");
        // connect(msgb, &QMessageBox::buttonClicked, this, [=]() {
        //     qDebug("FFFFFFFFFFFFFFFF");
        // });
        // msgb->exec();
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "to-do list", "Delete this task?",
                                      QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            m_parentWindow->deleteTask(this->text(), this->parentWidget());
        }

    });

    contextMenu.setStyleSheet("QMenu::item:selected{ background-color: rgba(149, 223, 223, 140); color: black; }");
    contextMenu.exec(event->globalPos());
}
