/*
 *
 * window.cc
 *
 * Created by Ruibin.Chow on 2022/11/22.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#include "window.h"

namespace view {

Window::Window(QWidget *parent) : QMainWindow(parent) {
    this->setFixedSize(800, 500);
}


}

