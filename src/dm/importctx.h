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

#ifndef  GME_DM_IMPORTCTX_H
#define  GME_DM_IMPORTCTX_H


#include "slg/slg.h"
#include "slg/sdl/scene.h"
#include "slg/editaction.h"
#include <boost/filesystem.hpp>
#include "utils/pathext.h"

namespace gme{

/** @brief Implements the import context.
 * @details ImportContext isolation new and import
**/
class ImportContext
{
private:
    slg::Scene      *m_scene;
    int             m_editAction;
    std::string     m_docBasePath;
    bool fileFiles(const boost::filesystem::path &filename,const boost::filesystem::path &dir,boost::filesystem::path &result)
    {
        boost::filesystem::path pathfile = dir / filename;
        if(boost::filesystem::exists(pathfile) && boost::filesystem::is_regular_file(pathfile))
        {
            result = pathfile;
            return true;
        }
        boost::filesystem::directory_iterator   end_iter;
        for(boost::filesystem::directory_iterator dir_iter(dir);dir_iter != end_iter ; ++dir_iter)
        {
            if(boost::filesystem::is_directory(*dir_iter))
            {
                if(fileFiles(filename,*dir_iter,result))
                {
                    return true;
                }
            }
        }
        return false;
    }
public:
    inline std::string  findFile(const std::string &srcpath,bool bSearch)
    {
        if(srcpath.length() == 0)
            return srcpath;
        boost::filesystem::path fsPath;
        std::string path = srcpath;
        try{
            boost::filesystem::gme_ext::replaceSeparator(path);
            fsPath = boost::filesystem::absolute(path,m_docBasePath);
            if(boost::filesystem::exists(fsPath) && boost::filesystem::is_regular_file(fsPath))
            {
                return fsPath.string();
            }
        }catch(std::exception e)
        {
            (void)e;
        }
        boost::filesystem::path filename = boost::filesystem::path(path).filename();
        if(bSearch && fileFiles(filename,m_docBasePath,fsPath))
        {// search file in m_docBasePath.
            return fsPath.string();
        }
        return "";
    }
    inline const std::string&  docBasepath(void)const
    {
        return m_docBasePath;
    }
    ImportContext(slg::Scene* s,const std::string &srcFile)
    {
        m_scene = s;
        m_editAction = 0;
        m_docBasePath = srcFile;
        m_docBasePath = boost::filesystem::canonical(m_docBasePath).parent_path().string();
    }
    inline void addAction(const slg::EditAction a)
    {
        m_editAction |= a;
    }
    inline slg::EditAction getAction()const
    {
        return (slg::EditAction)m_editAction;
    }
    inline slg::Scene*  scene()const{
        return m_scene;
    }
};

}

#endif //GME_DM_IMPORTCTX_H

