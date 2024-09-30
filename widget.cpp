//
// Created by leeyangy on 2024/9/26.
//

// You may need to build the project (run Qt uic code generator) to get "ui_Widget.h" resolved
#include "widget.h"
#include "ui_Widget.h"
#include <QSqlDatabase>
#include  <QSqlQuery>
#include <QSqlError>

#include <opencv2/highgui/highgui.hpp>

Widget::Widget(QWidget* parent) :
    QWidget(parent), ui(new Ui::Widget)
{
    ui->setupUi(this);
    setWindowTitle("--Clion QT5.12.9 Template-- 测试版  V1.0 24-0928     制作：LEEYANGY");

    const cv::Mat image = cv::imread("E:/ROG-4k.jpg", 1);
    // create image window named "My Image"
    cv::namedWindow("My Image");
    // show the image on window
    cv::imshow("My Image", image);

}

Widget::~Widget()
{
    delete ui;
}
