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

#ifndef  GME_DM_DOCIO_H
#define  GME_DM_DOCIO_H


#include "dm/doc.h"

namespace gme{

class DocIO : public DocScopeLocker
{
protected:
    void    loadObjectFromScene(void);
    void    loadSlgSceneFile(const std::string pathstring);
    bool    exportSpoloScene(const std::string &pathstring,bool bExportRes);
public:
    bool    loadScene(const std::string &path);
    bool    exportScene(const std::string &path);
};

}

#endif //GME_DM_DOCIO_H

