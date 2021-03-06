//////////////////////////////////////////////////////////////////////////////
//  Copyright (C) 2013 by sanpolo CO.LTD                                    //
//                                                                          //
//  This file is part of GMEditor                                           //
//                                                                          //
//  GMEditor is free software; you can redistribute it and/or modify it     //
//  under the terms of the LGPL License.                                    //
//                                                                          //
//  GMEditor is distributed in the hope that it will be useful,but WITHOUT  //
//  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY      //
//  or FITNESS FOR A PARTICULAR PURPOSE.                                    //
//                                                                          //
//  You should have received a copy of the LGPL License along with this     //
//  program.  If not, see <http://www.render001.com/gmeditor/licenses>.     //
//                                                                          //
//  GMEditor website: http://www.render001.com/gmeditor                     //
//////////////////////////////////////////////////////////////////////////////

#ifndef GME_SERVER_SERVER_H
#define GME_SERVER_SERVER_H

#include "utils/singleton.h"

namespace gme
{

///@brief 负责维护网段内对等发现的广播服务。
class UDPServer
{
private:
    friend class Server;
    UDPServer(/*ios*/);
    ~UDPServer();
public:
    void    start();
    void    stop();
};

class Server : public Singleton<Server>
{
protected:
    friend class Singleton<Server>;
    typedef Singleton<Server>   inherited;
    Server();
public:
    ~Server();
    void    start();
    void    stop();
    bool    isRunning();
};

}




#endif //GME_SERVER_SERVER_H
