//
// Created by leeyangy on 2024/9/26.
//

// You may need to build the project (run Qt uic code generator) to get "ui_Widget.h" resolved
#include "widget.h"
#include "ui_Widget.h"
#include <QSqlDatabase>
#include  <QSqlQuery>
#include <QSqlError>

Widget::Widget(QWidget* parent) :
    QWidget(parent), ui(new Ui::Widget)
{
    ui->setupUi(this);
    setWindowTitle("--Clion QT5.12.9 Template-- 测试版  V1.0 24-0928     制作：LEEYANGY");

    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("127.0.0.1");
    db.setUserName("root");
    db.setPassword("root");
    db.setPort(3306);
    db.setDatabaseName("test");

    if (!db.open())
    {
        QMessageBox::warning(this, "fail", db.lastError().text());
        qDebug() << "Failed to connect to database";
        return;
    } else
    {
        qDebug() << "Connected to database";
    }
    QSqlQuery query;
    query.exec("select * from user");
    while (query.next())
    {
        qDebug() << query.value("name").toString();
    }
    db.close();
}

Widget::~Widget()
{
    delete ui;
}
