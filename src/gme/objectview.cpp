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
#include "objectview.h"
#include "stringutil.h"
#include "dm/docobj.h"
#include <wx/sizer.h>


namespace gme{

class ObjectViewClientData : public wxClientData
{
public:
    const std::string      m_objid;
public:
    ObjectViewClientData(const std::string &id) : m_objid(id)
    {
    }
};

BEGIN_EVENT_TABLE(ObjectView, inherited)
    EVT_TREELIST_SELECTION_CHANGED(wxID_ANY, ObjectView::OnSelectionChanged)
    EVT_TREELIST_ITEM_EXPANDING(wxID_ANY, ObjectView::OnItemExpanding)
    EVT_TREELIST_ITEM_EXPANDED(wxID_ANY, ObjectView::OnItemExpanded)
    EVT_TREELIST_ITEM_CHECKED(wxID_ANY, ObjectView::OnItemChecked)
    EVT_TREELIST_ITEM_ACTIVATED(wxID_ANY, ObjectView::OnItemActivated)
    EVT_TREELIST_ITEM_CONTEXT_MENU(wxID_ANY, ObjectView::OnItemContextMenu)
END_EVENT_TABLE()

ObjectView::ObjectView(wxWindow* parent, wxWindowID id,const wxPoint& pos, const wxSize& size)
    : inherited(parent,id,pos,size)
{
    m_treelist = CreateTreeListCtrl(wxTL_DEFAULT_STYLE);
    refresh();
    m_sizer = new wxBoxSizer(wxVERTICAL);
    m_sizer->Add(m_treelist,wxSizerFlags(1).Expand());
    SetSizer(m_sizer);
    Show();
}

void
ObjectView::refreshAll(void)
{
    m_treelist->DeleteAllItems();
    refresh();
}

void
ObjectView::refresh(const std::string &id)
{
	wxTreeListItem  parent = FindItem(id,m_treelist->GetRootItem());
	BOOST_ASSERT_MSG(parent,"data panic....pls check");
	DocObj	obj;
	DocMat	mat;
	gme::ObjectNode &root = obj.getRootObject();
	ObjectNode *pParentNode = root.findObject(id);
	BOOST_ASSERT_MSG(pParentNode,"data panic...");
	ObjectNode::type_child_container::const_iterator it = pParentNode->begin();
	while(it != pParentNode->end())
	{
		addChild(parent,*it,mat);
		it++;
	}
}

wxTreeListItem
ObjectView::FindItem(const std::string &id,const wxTreeListItem &parent)
{
	const ObjectViewClientData* clientData = dynamic_cast<const ObjectViewClientData*>(m_treelist->GetItemData(parent));
	if(clientData && (clientData->m_objid == id))
		return parent;
	wxTreeListItem child = m_treelist->GetFirstChild(parent);
	while(child)
	{
		wxTreeListItem founded = FindItem(id,child);
		if(founded)
			return founded;
		child = m_treelist->GetNextSibling(child);
	}
	return NULL;
}

void
ObjectView::refresh(void)
{
    gme::DocObj dobj;
    gme::DocMat dobjmat;
    const gme::ObjectNode &root = dobj.getRootObject();
    ObjectNode::type_child_container::const_iterator it = root.begin();
    wxTreeListItem  rootItem = m_treelist->GetRootItem();
    while(it != root.end())
    {
        addChild(rootItem,*it,dobjmat);
        it++;
    }
}

void
ObjectView::addChild(wxTreeListItem& parent,const ObjectNode &node,DocMat &objop)
{
	wxTreeListItem item = m_treelist->AppendItem(parent,node.name());
    m_treelist->SetItemText(item, 1, objop.getMatName(node.matid()));
    m_treelist->SetItemText(item, 2, "");
    m_treelist->SetItemData(item,new ObjectViewClientData(node.id()));
    ObjectNode::type_child_container::const_iterator it = node.begin();
    while(it != node.end())
    {
        addChild(item,*it,objop);
        it++;
    }
}


wxTreeListCtrl*
ObjectView::CreateTreeListCtrl(long style)
{
	DECLARE_WXCONVERT;
    wxTreeListCtrl* const tree = new wxTreeListCtrl(this, wxID_ANY,
                                  wxDefaultPosition, wxDefaultSize,style);
    tree->AppendColumn(gmeWXT("名称"),
                       wxCOL_WIDTH_AUTOSIZE,
                       wxALIGN_LEFT,
                       wxCOL_RESIZABLE | wxCOL_SORTABLE);
    tree->AppendColumn(gmeWXT("材质"),
                       tree->WidthFor("0123456789"),
                       wxALIGN_RIGHT,
                       wxCOL_RESIZABLE | wxCOL_SORTABLE);
    tree->AppendColumn(gmeWXT("引用对象"),
                       tree->WidthFor("0123456789"),
                       wxALIGN_RIGHT,
                       wxCOL_RESIZABLE | wxCOL_SORTABLE);
    return tree;
}

ObjectView::~ObjectView()
{

}

bool
ObjectView::getSelection(std::string &id)
{
    wxTreeListItem  item = m_treelist->GetSelection();
    if(item.IsOk())
    {
        wxClientData *pData = m_treelist->GetItemData(item);
        ObjectViewClientData* pvd = dynamic_cast<ObjectViewClientData*>(pData);
        if(pvd)
        {
            id = pvd->m_objid;
            return true;
        }
    }
    return false;
}

bool
ObjectView::delSelection()
{
	wxTreeListItem  item = m_treelist->GetSelection();
    if(item.IsOk())
    {
		m_treelist->DeleteItem(item);
		return true;
    }
    return false;
}




void
ObjectView::OnSelectionChanged(wxTreeListEvent& event)
{

}

void
ObjectView::OnItemExpanding(wxTreeListEvent& event)
{

}

void
ObjectView::OnItemExpanded(wxTreeListEvent& event)
{

}

void
ObjectView::OnItemChecked(wxTreeListEvent& event)
{

}

void
ObjectView::OnItemActivated(wxTreeListEvent& event)
{

}

void
ObjectView::OnItemContextMenu(wxTreeListEvent& event)
{

}


} //end namespace gme