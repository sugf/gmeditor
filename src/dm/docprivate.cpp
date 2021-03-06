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

#include "config.h"
#include <boost/format.hpp>
#include "docprivate.h"
#include "dm/docmat.h"
#include "slgobject.h"
#include "utils/i18n.h"
#include "slgtexture.h"
#include "slgmaterial.h"
#include "slgutils.h"

namespace gme{

bool  SlgUtil::Editor::m_bForceRefresh = false;
bool  SlgUtil::Editor::sv_bRefreshWhenError = false;

//static  bool    sv_hack_slg_last_is_material_type = false;
SlgUtil::Editor::~Editor()
{
    if (m_session->editActions.Has(slg::MATERIALS_EDIT)) {
        m_session->renderConfig->scene->RemoveUnusedMaterials();
        m_session->renderConfig->scene->RemoveUnusedTextures();
    }
    if( m_bForceRefresh ) //m_session->editActions.Has(slg::IMAGEMAPS_EDIT) || m_session->editActions.Has(slg::MATERIAL_TYPES_EDIT) )//m_bNeedRefresh )
    {///@fixme: we must restart render when we have IMAGEMAPS_EDIT.
        m_session->editActions.Reset();
        m_session->Start();
        Doc::instance().pDocData->cachefilm().invalidate();
    }else{
        if(m_session->editActions.HasAnyAction())
        {
            Doc::instance().pDocData->cachefilm().invalidate();
        }
        try{
            if(m_session->editActions.Has(slg::IMAGEMAPS_EDIT))
            {
                m_session->editActions.AddAction(slg::MATERIAL_TYPES_EDIT);
            }
//            if(m_session->editActions.Has(slg::MATERIAL_TYPES_EDIT))
//            {
//                sv_hack_slg_last_is_material_type = true;
//            }
//            if(sv_hack_slg_last_is_material_type && !m_session->editActions.Has(slg::MATERIAL_TYPES_EDIT))
//            {//material_type_edit之后第一次修正，需要compile kernel.
//                sv_hack_slg_last_is_material_type = false;
//                m_session->editActions.AddAction(slg::MATERIAL_TYPES_EDIT);
//            }
            m_session->EndEdit();
        }catch(std::exception &e)
        {
            if(refreshWhenError())
            {
                Doc::SysLog(Doc::LOG_WARNING,boost::str(boost::format(__("更新场景时发生错误:'%s'，这通常是显卡驱动引发的，请检查显卡驱动。强制更新场景...")) % e.what() ) );
                m_session->Stop();
                m_session->Start();
                Doc::SysLog(Doc::LOG_WARNING,boost::str(boost::format(__("更新完毕。建议您保存场景，重启飞图并打开场景以消除隐患。")) ) );
            }else
            {
                Doc::SysLog(Doc::LOG_WARNING,boost::str(boost::format(__("更新场景时发生错误:'%s'，这通常是显卡驱动引发的，请检查显卡驱动。建议您保存场景，重启飞图并打开场景以消除隐患。")) % e.what() ) );
            }
        }
    }
}

std::string
ImportContext::findFile(const std::string &srcpath)
{
    if(srcpath.length() == 0)
        return srcpath;
    try{
        return Doc::instance().pDocData->findFile(srcpath,m_docBasePath);
    }catch(std::exception e)
    {
        Doc::SysLog(Doc::LOG_WARNING,boost::str(boost::format(__("找不到文件'%s'。")) % srcpath  ) );
    }
    return "";
}



DocPrivate::DocPrivate(void)
{
    //全局相关。
    m_bAutofocus = false;
    m_bAutoTarget = false;
    //场景相关。(close时需要重置)
    m_state = DocPrivate::ST_STOPPED;
    forceExport = false;
    m_fileFilmValid = false;
    m_bModified = false;
}

void
DocPrivate::closeScene(void)
{
    if(this->isStop())
        return;
    clearSelection();
    if(m_session)
    {
        if(this->isRunning())
        {
            m_session->Stop();
        }
        m_session.reset();
    }
    forceExport = false;
    this->m_state = ST_STOPPED;
    m_filmPathfile.clear();
    m_fileFilmValid = false;
    m_bModified = false;
    objManager.clear();
    matManager.clear();
    texManager.clear();
    camManager.clear();
    m_cacheFilm.invalidate();
    fireStateChanged(STATE_CLOSE);
}

DocPrivate::~DocPrivate(void)
{
    clearAllListen();
    closeScene();
}

bool
DocPrivate::saveFilm(const std::string &path)
{
    return this->cachefilm().saveFilm(path);
}

void
DocPrivate::start(void)
{
    BOOST_ASSERT(m_session.get() != NULL);
    bool   bPause = isPause();
    m_session->Start();
    m_state = ST_RUNNING;
    if(m_fileFilmValid && !m_filmPathfile.empty() )
    {//load film from file
        m_fileFilmValid = false;
        if(boost::filesystem::exists(m_filmPathfile))
        {
            bool    bSuc = false;
            try{
                this->cachefilm().loadFilm(this->m_session->film,m_filmPathfile);
                bSuc = true;
            }catch(std::exception e)
            {
                (void)e;
            }
            if(!bSuc)
            {
                Doc::SysLog(Doc::LOG_WARNING,boost::str(boost::format(__("从文件'%s'中恢复渲染状态时发生异常。不能基于上次渲染结果继续，重新开始渲染。")) % m_filmPathfile ) );
            }
        }else{
            Doc::SysLog(Doc::LOG_WARNING,boost::str(boost::format(__("渲染状态文件'%s'不存在。不能基于上次渲染结果继续，重新开始渲染。"))%m_filmPathfile ) );
        }
    }
    if(!bPause)
    {
        fireStateChanged(STATE_OPEN);
        fireSizeChanged();
    }
}

void
DocPrivate::pause(void)
{
    if(!this->isRunning())
        return;

    m_cacheFilm.saveNativeFilm();
    m_session->Stop();
    m_state = ST_PAUSED;
    fireStateChanged(STATE_PAUSE);
}


void
DocPrivate::onSelectedChanged(void)
{
    if(m_bAutoTarget && this->m_session.get() && this->m_session->renderConfig->scene)
    {
        luxrays::BBox bbox = objManager.getSelectionBBox();
        if(bbox.IsValid())
        {
    	    m_session->BeginEdit();

            slg::PerspectiveCamera *camera = this->m_session->renderConfig->scene->camera;

            camera->target = bbox.Center();
    	    camera->Update(m_session->film->GetWidth(), m_session->film->GetHeight());
	        m_session->editActions.AddAction(slg::CAMERA_EDIT);
    	    m_session->EndEdit();
    	    cachefilm().invalidate();
    	    camManager.saveCurrentCamera();
        }
    }
}

bool
DocPrivate::getNativeRenderInfo(RenderInfo &ri)
{
    slg::RenderSession* session = getSession();
    if(session && session->film)
    {
        ri.convergence = session->renderEngine->GetConvergence();
        ri.elapsedTime = session->renderEngine->GetRenderingTime();
        ri.pass = session->renderEngine->GetPass();
        ri.totalRaysSec = session->renderEngine->GetTotalRaysSec();
        ri.totalSamplesSec = session->renderEngine->GetTotalSamplesSec();
        return true;
    }
    return false;
}

}


