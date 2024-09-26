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
}

Widget::~Widget()
{
    delete ui;
}
