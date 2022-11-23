/*
 * window.h
 *
 * Created by Ruibin.Chow on 2022/11/22.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>

namespace view {

class Window : public QMainWindow {

Q_OBJECT

public:
    Window(QWidget *parent = nullptr);
    virtual ~Window() = default;
};

}

#endif /* !WINDOW_H */
