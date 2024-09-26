//
// Created by leeyangy on 2024/9/26.
//

#ifndef WIDGET_H
#define WIDGET_H

#include <QApplication>
#include <QDebug>
#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>
#include <QTextCodec>
#include <QMessageBox>
#include <string>

// 1:Combo Box控件
#include <QComboBox>

// 2:FontComboBox控件
#include <QFontComboBox>
#include <QLabel>

// 3:Line Edit控件
#include <QLineEdit>
#include <QPushButton>

// 4:Plain Text Edit控件
#include <QPlainTextEdit>
#include <QRadioButton>

// 5:Spin Box控件
#include <QSpinBox>

// 6:时间控件
#include <QTimeEdit>
#include <QDateEdit>
#include <QDateTimeEdit>

// 7: Scroll Bar控件
#include <QScrollBar>

// 8: Key Sequence Edit控件
#include <QKeySequenceEdit>

QT_BEGIN_NAMESPACE

namespace Ui
{
    class Widget;
}

QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget* parent = nullptr);
    ~Widget() override;

private slots:

private:
    Ui::Widget* ui;
};


#endif //WIDGET_H
