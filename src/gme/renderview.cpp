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
#include "renderview.h"
#include "utils/option.h"
#include "dm/docimg.h"
#include "dm/doccamera.h"

namespace gme{

BEGIN_EVENT_TABLE(RenderView, inherited)

    EVT_MOTION(RenderView::mouseMoved)
    EVT_LEFT_DOWN(RenderView::mouseLeftDown)
    EVT_LEFT_UP(RenderView::mouseLeftReleased)
    EVT_RIGHT_DOWN(RenderView::rightClick)
    EVT_LEAVE_WINDOW(RenderView::mouseLeftWindow)
    EVT_KEY_DOWN(RenderView::keyPressed)
    EVT_KEY_UP(RenderView::keyReleased)
    EVT_MOUSEWHEEL(RenderView::mouseWheelMoved)
    EVT_IDLE(RenderView::onIdle)

    // catch paint events
    EVT_PAINT(RenderView::paintEvent)
END_EVENT_TABLE()



RenderView::RenderView(wxFrame* parent) : inherited(parent)
{
    ///@todo: connection from option.
    SetMinSize( wxSize(20, 20) );
    m_action = ACTION_INVALID;
    opt_RotateStep = 4.f;
    m_micro_tick = boost::posix_time::microsec_clock::local_time();
    //0.2 second.
    opt_MinEditInterval = 200;

    //default is 3.3fps.
    m_last_update_tick = boost::posix_time::microsec_clock::local_time();
    opt_MinUpdateInterval = 300;
}

RenderView::~RenderView()
{
}

void
RenderView::onIdle(wxIdleEvent &event)
{
    boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
    boost::posix_time::time_duration diff = now - m_last_update_tick;
    if(diff.total_milliseconds() > opt_MinUpdateInterval)
    {
        this->Update();
        m_last_update_tick = now;
    }
    event.Skip();
}

/*
 * Called by the system of by wxWidgets when the panel needs
 * to be redrawn. You can also trigger this call by
 * calling Refresh()/Update().
 */
void RenderView::paintEvent(wxPaintEvent & evt)
{
    // depending on your system you may need to look at double-buffered dcs
    wxPaintDC dc(this);
    render(dc);
}

/*
 * Alternatively, you can use a clientDC to paint on the panel
 * at any time. Using this generally does not free you from
 * catching paint events, since it is possible that e.g. the window
 * manager throws away your drawing when the window comes to the
 * background, and expects you will redraw it when the window comes
 * back (by sending a paint event).
 */
void RenderView::paintNow()
{
    // depending on your system you may need to look at double-buffered dcs
    wxClientDC dc(this);
    render(dc);
}

/*
 * Here we do the actual rendering. I put it in a separate
 * method so that it can work no matter what type of DC
 * (e.g. wxPaintDC or wxClientDC) is used.
 */
void RenderView::render(wxDC&  dc)
{
    gme::DocImg docimg;
    int w,h;
    this->GetSize(&w,&h);
    bool    bDrawed = false;
    if(docimg.isValid())
    {
        wxImage image(w,h);
        ImageDataScale   idata;
        idata.data = image.GetData();
        idata.width = w;
        idata.height = h;
        idata.pitch = w * 3;
        idata.default_red = 255;
        idata.default_green = 0;
        idata.default_blue = 0;

        docimg.getData(&idata);

        ///@todo 添加卷滚处理。

        wxBitmap bmp( image );
        dc.DrawBitmap( bmp, 0, 0 );
        bDrawed = true;
    }
    if(!bDrawed)
    {
        dc.SetBrush( *wxRED_BRUSH );
        dc.DrawRectangle( 0, 0, w, h );
    }
}

void
RenderView::rotateCam(wxMouseEvent& event)
{
    boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
    boost::posix_time::time_duration diff = now - m_micro_tick;
    if(diff.total_milliseconds() > opt_MinEditInterval)
    {
        long x,y;
        event.GetPosition(&x,&y);
		if(x != m_lastx || y != m_lasty)
		{
			gme::DocCamera doccam;
			doccam.rotate(m_lastx - x, m_lasty - y,opt_RotateStep);
			m_lastx = x;
			m_lasty = y;
			m_micro_tick = now;
		}
    }
}

void RenderView::mouseLeftDown(wxMouseEvent& event)
{
    event.GetPosition(&m_lastx,&m_lasty);
    m_action = ACTION_CAM_ROTATE;
}

void RenderView::mouseLeftReleased(wxMouseEvent& event)
{
//    wxMessageBox( wxT("You pressed a custom button") );
    rotateCam(event);
    m_action = ACTION_INVALID;
}

void RenderView::mouseLeftWindow(wxMouseEvent& event)
{
}

void RenderView::mouseMoved(wxMouseEvent& event)
{
    if(m_action == ACTION_CAM_ROTATE)
    {
        rotateCam(event);
    }
}

void RenderView::mouseWheelMoved(wxMouseEvent& event)
{
}

void RenderView::rightClick(wxMouseEvent& event)
{
}

void RenderView::keyPressed(wxKeyEvent& event)
{
}

void RenderView::keyReleased(wxKeyEvent& event)
{
}

}//end namespace gme

