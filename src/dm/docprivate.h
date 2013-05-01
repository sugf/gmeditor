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
#include "slgcamera.h"
#include <boost/shared_ptr.hpp>
#include "utils/eventlisten.h"
#include <boost/function.hpp>
#include <boost/foreach.hpp>

namespace gme{

class DocPrivate
{
private:
    friend  class Doc;
    DocPrivate(void);
    ~DocPrivate(void);
    std::vector<std::string>                    m_selectionVector;
public:
    typedef boost::function<void (int,int)>     type_imagesize_handler;
    typedef boost::function<void (void)>        type_state_handler;
    typedef boost::function<void (const std::string&)>  type_selection_handler;

    enum{
        STATE_OPEN,
        STATE_CLOSE,
        STATE_MAX
    };

    enum{
        SEL_ITEMSELECTED,
        SEL_ITEMDELSELECTED,
        SEL_ITEMCHILDADDED, //指示某个ObjectNode下新添加了孩子。
        SEL_ITEMSELFREMOVED, //指示某个ObjectNode自身被删除。
        SEL_MAX
    };

    ///@todo: 需要一个材质转化专家系统来支持材质转化。
	//
    boost::shared_ptr<slg::RenderSession>   m_session;
    /** @fixme: slg的started为保护成员。
    **/
    bool                                    m_started;

	ExtraTextureManager               texManager;
	ExtraMaterialManager              matManager;
	ExtraObjectManager                objManager;
	ExtraCameraManager                camManager;
    SingleEventListen<type_imagesize_handler>   imageSize_Evt;
    EventListen<int,type_state_handler>         state_Evt;
    EventListen<int,type_selection_handler>     selection_Evt;
private:
    inline  void    clearAllListen(void)
    {
        imageSize_Evt.clear();
        state_Evt.clear();
        selection_Evt.clear();
    }
public:
    ///@brief selection members.
    inline  void    clearSelection(void)
    {
        BOOST_FOREACH(const std::string &key,m_selectionVector)
        {
            fireSelection(SEL_ITEMDELSELECTED,key);
        }
        m_selectionVector.clear();
    }
    inline  bool    addSelection(const std::string &oid)
    {
        if(std::find(m_selectionVector.begin(),m_selectionVector.end(),oid) == m_selectionVector.end())
        {
            fireSelection(SEL_ITEMSELECTED,oid);
            m_selectionVector.push_back(oid);
            return true;
        }
        return false;
    }
    inline  std::vector<std::string>& getSelection(void)
    {
        return m_selectionVector;
    }

    inline  bool    removeSelection(const std::string &oid)
    {
        std::vector<std::string>::iterator  it = std::find(m_selectionVector.begin(),m_selectionVector.end(),oid);
        if(it != m_selectionVector.end())
        {
            fireSelection(SEL_ITEMDELSELECTED,oid);
            m_selectionVector.erase(it);
            return true;
        }
        return false;
    }
    inline  void    fireSelection(int state,const std::string &oid)
    {
        selection_Evt.fire(state,oid);
    }
public:
    //关闭当前打开场景。
    void    closeScene(void);

    inline slg::RenderSession*  getSession(void){
        return m_session.get();
    }

    inline  void    fireStateChanged(int state)
    {
        state_Evt.fire(state);
    }

    inline  void    fireSizeChanged(void)
    {
        imageSize_Evt.fire(m_session->film->GetWidth(),m_session->film->GetHeight());
    }

    inline  void    startScene(void)
    {
        BOOST_ASSERT(m_session.get() != NULL);
        m_session->Start();
        m_started = true;
        fireStateChanged(STATE_OPEN);
        fireSizeChanged();
    }
};

}



#endif  //GME_DM_DOC_H
