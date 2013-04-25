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

#ifndef  GME_DM_DOCPRIVATE_H
#define  GME_DM_DOCPRIVATE_H

#include "slg/rendersession.h"
#include "slgtexture.h"
#include "slgmaterial.h"
#include "slgobject.h"
#include <boost/shared_ptr.hpp>

namespace gme{

class DocPrivate
{
private:
    friend  class Doc;
    DocPrivate(void);
    ~DocPrivate(void);
public:
    ///@todo: 需要一个材质转化专家系统来支持材质转化。
	//
    boost::shared_ptr<slg::RenderSession>   m_session;
    /** @fixme: slg的started为保护成员。
    **/
    bool                                    m_started;

	ExtraTextureManager               texManager;
	ExtraMaterialManager              matManager;
	ExtraObjectManager                objManager;
public:
    //关闭当前打开场景。
    void    closeScene(void);
    inline slg::RenderSession*  getSession(void){
        return m_session.get();
    }
};

}



#endif  //GME_DM_DOC_H