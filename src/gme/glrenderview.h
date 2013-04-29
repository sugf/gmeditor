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

#ifndef  GME_GLRENDERVIEW_H
#define  GME_GLRENDERVIEW_H

#include <wx/wx.h>
#include <boost/any.hpp>

namespace gme{

class GlRenderFrame;
class GlRenderView : public wxScrolledWindow
{
public:
    static  const char*    vm_opt_path;
private:
    typedef wxScrolledWindow    inherited;
    GlRenderFrame       *m_glframe;
    wxBoxSizer          *m_sizer;
protected:
    void onDocumentSizeChanged(int w,int h);
    bool onEditModeChanged(const boost::any &nvar);
    void updateViewmode(int viewModeCmds);
    void setViewmode(int m);
public:
    GlRenderView(wxFrame* parent);
    virtual ~GlRenderView();
    void    setViewmodeFromCmd(int cmds);
protected:
//    DECLARE_EVENT_TABLE()
};

} //end namespace gme

#endif //GME_GLRENDERVIEW_H