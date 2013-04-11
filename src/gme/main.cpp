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
#include "mainframe.h"
#include "utils/option.h"
#include <boost/scope_exit.hpp>
#include "dm/doc.h"

class MyApp : public wxApp
{
public:
    bool OnInit()
    {
        //initionlize config.
        typedef const char*   str_point;
        str_point*   mb_args = new str_point[argc];
        for(int i = 0; i < argc;i++)
        {
            wxString string(argv[i]);
            mb_args[i] = strdup(string.mb_str());
        }
	    BOOST_SCOPE_EXIT( (&mb_args) (&argc))
	    {
	        for(int i = 0; i < argc; i++)
	        {
	            free(const_cast<char*>(mb_args[i]));
            }
            delete[] mb_args;
	    }BOOST_SCOPE_EXIT_END
        
    	if(!gme::Option::instance().initFromArgs(argc,mb_args ))
		    return false;

	    std::string source("system.source");
	    if(gme::Option::instance().is_existed(source))
	    {
		    std::vector<std::string> srcset = gme::Option::instance().get<std::vector<std::string> >(source);
		    if(srcset.size())
		    {
    		    std::vector<std::string>::iterator it = srcset.begin();
		    	gme::Doc::instance().loadScene(*it);
            }
        }


        wxFrame* frame = new gme::MainFrame(NULL);
        SetTopWindow(frame);
        frame->Maximize();
        frame->Show();
        return true;
    }
};

DECLARE_APP(MyApp)
IMPLEMENT_APP(MyApp)