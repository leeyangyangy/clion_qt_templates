//
// Created by leeyangy on 2024/9/26.
//

// You may need to build the project (run Qt uic code generator) to get "ui_Widget.h" resolved
#include "widget.h"
#include "ui_Widget.h"

Widget::Widget(QWidget* parent) :
    QWidget(parent), ui(new Ui::Widget)
{
    ui->setupUi(this);
    setWindowTitle("--Clion QT5.12.9 Template-- 测试版  V1.0 24-0905     制作：LEEYANGY");

    // 测试文件是否能被正确读取
    QFile file = QFile(":/lang/test.txt");

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Failed to open file";
    }
    else
    {
        qDebug() << "File opened successfully";
        const QString text = file.readAll();
        qDebug() << text;
        file.close();
    }


    QPixmap pixmap(":/image/404.png");
    if (pixmap.isNull())
    {
        qDebug() << "Image not found!";
    }
    else
    {
        qDebug() << "Image loaded successfully!";
        ui->picture->setPixmap(pixmap);
    }

    ui->picture->setPixmap(QPixmap(":/image/404.png"));


}

Widget::~Widget()
{
    delete ui;
}
