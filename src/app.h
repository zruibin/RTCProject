/*
 * app.h
 *
 * Created by Ruibin.Chow on 2022/10/20.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#ifndef APP_H
#define APP_H

namespace app {

class App {
    
public:
    explicit App() = default;
    
    void Init(void);
    void Run(int &argc, char **argv);
};

}

#endif /* !APP_H */
