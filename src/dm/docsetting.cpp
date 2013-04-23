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
#include "dm/setting.h"
#include "dm/docsetting.h"
#include "slg/slg.h"
#include "docprivate.h"
#include "slgmaterial.h"
#include <boost/assert.hpp>


namespace gme
{
bool
DocSetting::getImageSize(unsigned long &width,unsigned long &height)
{
    return false;
}

bool
DocSetting::getLinearScale(float &ls)
{
    return false;
}

bool
DocSetting::setLinearScale(float ls)
{
	slg::RenderSession* session = pDocData->getSession();
    if(session && session->film)
    {
		slg::Film *film = session->film;
		slg::LinearToneMapParams *params = (slg::LinearToneMapParams *)film->GetToneMapParams()->Copy();
		params->scale = ls;
		film->SetToneMapParams(*params);
		delete params;
		return true;
	}
    return false;
}
}
