#ifndef MYCHECKBOX_H
#define MYCHECKBOX_H

#include <QCheckBox>
#include <QMenu>
#include <QContextMenuEvent>
#include "mainwindow.h"

class MyCheckBox : public QCheckBox
{
    Q_OBJECT

public:
    explicit MyCheckBox(mainwindow *parentWindow, QWidget *parent = nullptr);

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;

private:
    mainwindow *m_parentWindow;
};

#endif // MYCHECKBOX_H
