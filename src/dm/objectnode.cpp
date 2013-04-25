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
#include "dm/doc.h"
#include "dm/objectnode.h"
#include "docprivate.h"
#include "slg/slg.h"
#include "openctm/openctm.h"


template<class T>
bool    SaveCtmFile(bool useplynormals,T *pMesh,const std::string &filename,gme::conditional_md5 &md5)
{
    CTMcontext context = NULL;
    CTMuint    vertCount, triCount, * indices;
    CTMfloat   *vertices;
    CTMfloat   *aNormals = NULL;
    CTMfloat   *aUVCoords = NULL;

    vertCount = pMesh->GetTotalVertexCount ();
    triCount = pMesh->GetTotalTriangleCount ();
    vertices  = (CTMfloat*)(void*)pMesh->GetVertices();
    indices  = (CTMuint*)(void*)pMesh->GetTriangles();

    md5.update((const unsigned char *)(void*)vertices,vertCount * 3 * sizeof(CTMfloat));
    md5.update((const unsigned char *)(void*)indices,triCount * 3 * sizeof(CTMuint));

#if 0
    if(false)//useplynormals) //pMesh->HasNormals())
    {
        GME_TRACE("has normals ... ");
        aNormals = new CTMfloat[vertCount * 3];
        for(unsigned int idx = 0 ; idx < vertCount; idx++)
        {
            luxrays::Normal n = pMesh->GetShadeNormal(idx);
            aNormals[idx * 3] = n.x;
            aNormals[idx * 3 + 1] = n.y;
            aNormals[idx * 3 + 2] = n.z;
        }
        md5.update((const unsigned char *)(void*)aNormals,vertCount * 3 * sizeof(CTMfloat));
    }
#endif

    context = ctmNewContext(CTM_EXPORT);
    ctmDefineMesh(context, vertices, vertCount, indices, triCount, aNormals);
    if(pMesh->HasUVs())
    {
        aUVCoords = new CTMfloat[vertCount * 2];
        for(unsigned int idx = 0 ; idx < vertCount; idx++)
        {
            luxrays::UV uv = pMesh->GetUV(idx);
            aUVCoords[idx * 2] = uv.u;
            aUVCoords[idx * 2 + 1] = uv.v;
        }
        ctmAddUVMap(context,aUVCoords,"def",NULL);
    }

    ctmSave(context, filename.c_str());

    //ctxHashValue = md5.finalize().hexdigest();

    if(aNormals)
        delete[] aNormals;
    if(aUVCoords)
        delete[] aUVCoords;
    if(context)
        ctmFreeContext(context);


    return true;
}

static
bool    SaveCtmFile(bool useplynormals,luxrays::ExtMesh* extMesh,const std::string &filename,gme::conditional_md5 &md5)
{
    luxrays::ExtInstanceTriangleMesh*   pMesh = dynamic_cast<luxrays::ExtInstanceTriangleMesh*>(extMesh);
    if(pMesh)
    {
        return SaveCtmFile<luxrays::ExtInstanceTriangleMesh>(useplynormals,pMesh,filename,md5);
    }
    luxrays::ExtTriangleMesh*   pMesh2 = dynamic_cast<luxrays::ExtTriangleMesh*>(extMesh);
    if(pMesh2)
    {
        return SaveCtmFile<luxrays::ExtTriangleMesh>(useplynormals,pMesh2,filename,md5);
    }
    return false;
}

namespace gme{

const char* constDef::object = "object";
const char* constDef::material = "material";
const char* constDef::texture = "texture";
const char* constDef::id = "id";
const char* constDef::name = "name";
const char* constDef::type = "type";
const char* constDef::mapping = "mapping";
const char* constDef::ctxmd5 = "ctxmd5";
const char* constDef::transformation = "transformation";
const char* constDef::position = "position";
const char* constDef::file = "file";
const char* constDef::emission = "emission";
const char* constDef::bumptex = "bumptex";
const char* constDef::normaltex = "normaltex";
const char* constDef::kd = "kd";
const char* constDef::kr = "kr";
const char* constDef::kt = "kt";
const char* constDef::ioroutside = "ioroutside";
const char* constDef::iorinside = "iorinside";
const char* constDef::exp = "exp";
const char* constDef::amount = "amount";
const char* constDef::ks = "ks";
const char* constDef::uroughness = "uroughness";
const char* constDef::vroughness = "vroughness";
const char* constDef::ka = "ka";
const char* constDef::d = "d";
const char* constDef::index = "index";
const char* constDef::n = "n";
const char* constDef::k = "k";




ObjectNode*
ObjectNode::findObject(const std::string &id,type_path *pPath)
{
    if(pPath)
        pPath->push_back(this);
    if(this->id() == id)
        return this;
    ObjectNode* result = NULL;
    type_child_container::iterator  it = this->begin();
    while(it != this->end())
    {
        result = it->findObject(id);
        if(result)
        {
            break;
        }
        it++;
    }
    if(pPath && !result)
        pPath->pop_back();
    return result;
}

type_xml_node*
ObjectNode::dump(type_xml_node &parent,dumpContext &ctx)
{
    type_xml_doc  *pDoc = parent.document();
    BOOST_ASSERT_MSG(pDoc,"node usage error!");
    type_xml_node *pSelf = pDoc->allocate_node(NS_RAPIDXML::node_element, constDef::object);

    parent.append_node(pSelf);
    pSelf->append_attribute(pDoc->allocate_attribute(constDef::id,allocate_string(pDoc,m_id)));

    if(this->m_name.length())
    {
        pSelf->append_attribute(pDoc->allocate_attribute(constDef::name,allocate_string(pDoc,m_name)));
    }

	luxrays::ExtMesh*   extMesh = Doc::instance().pDocData->objManager.getExtMesh(id());
    if(extMesh)
    {//只有模型存在，我们才继续输出与模型相关的信息。
        std::string     write_file;
        conditional_md5 md5(ctx);
        //boost::filesystem::path target_model = ctx.target / "mesh%%%%%%.ply";
        //只有在不是multiMesh时我们才可以保存。否则会引发下次加载的模型重复。从而导致材质无法配对。
        if(this->filepath().length())
        {//获取映射的文件名。
            if(ctx.isCopyResource())
            {//保存资源。
		        boost::filesystem::path target_model = ctx.target / "mesh%%%%%%.ctm";
                boost::filesystem::path target = boost::filesystem::unique_path(target_model);
                //extMesh->WritePly(target.string());
                SaveCtmFile(this->useplynormals(),extMesh,target.string(),md5);
                write_file = target.filename().string();
            }else{//不保存资源，直接保存m_filepath.
                write_file = this->filepath();
            }
        }else{//没有定义文件名。此时直接保存资源。
	        boost::filesystem::path target_model = ctx.target / "mesh%%%%%%.ctm";
            boost::filesystem::path target = boost::filesystem::unique_path(target_model);
            //extMesh->WritePly(target.string());
            SaveCtmFile(this->useplynormals(),extMesh,target.string(),md5);
            write_file = target.filename().string();
        }
        if(md5.isGenerateMD5())
        {
            std::string ctxHashValue = md5.hexdigest();
            pSelf->append_attribute(pDoc->allocate_attribute(constDef::ctxmd5,allocate_string(pDoc,ctxHashValue)));

            const std::string *pName = ctx.queryObjFilepath(ctxHashValue);
            if(pName)
            {
                ///@todo 由于内容重复，删除刚保存的模型文件。 需要判断，不能删除外部文件。
                if(ctx.isCopyResource())
                    boost::filesystem::remove(write_file);
                write_file = *pName;
            }else{
                ctx.addObjMapper(ctxHashValue,write_file);
            }
        }
        BOOST_ASSERT_MSG(write_file.length() > 0, "fail to write object data?");

        pSelf->append_attribute(pDoc->allocate_attribute(constDef::file,allocate_string(pDoc,write_file)));

        const slg::Material* pMat = ExtraMaterialManager::getSlgMaterial(this->matid());
        BOOST_ASSERT_MSG(pMat,"invalid ref material?");
        Doc::instance().pDocData->matManager.dump(*pSelf,pMat,ctx);
    }

    //不需要输出materialid.这个id在子节点中自己创建。

    ObjectNode::type_child_container::iterator  it = this->begin();
    while(it != this->end())
    {
        it->dump(*pSelf,ctx);
        it++;
    }

    return pSelf;
}

}