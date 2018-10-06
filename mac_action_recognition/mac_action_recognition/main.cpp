//
//  main.cpp
//  mac_action_recognition
//
//  Created by Ddd on 2018/10/6.
//  Copyright © 2018年 Ddd. All rights reserved.
//

#include <iostream>
#include "action_recognition.h"
#include <QtWidgets/QApplication>
#include <QTextcodec.h>

using namespace std;
int main(int argc, char *argv[])
{
    QTextCodec *gbk = QTextCodec::codecForName("GBK");
    QApplication a(argc, argv);
    action_recognition w;
    w.show();
    return a.exec();
}
