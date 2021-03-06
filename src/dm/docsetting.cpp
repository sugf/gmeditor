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
#include <boost/algorithm/string.hpp>
#include "dm/setting.h"
#include "dm/docsetting.h"
#include "slg/slg.h"
#include "docprivate.h"
#include "slgmaterial.h"
#include "slgsetting.h"
#include "slgutils.h"
#include <boost/assert.hpp>


namespace gme
{

bool
DocSetting::forceRefresh(void)
{
    return SlgUtil::Editor::forceRefresh();
}

void
DocSetting::forceRefresh(bool forceRefresh)
{
    SlgUtil::Editor::forceRefresh(forceRefresh);
}

bool
DocSetting::refreshWhenError(void)
{
    return SlgUtil::Editor::refreshWhenError();
}

void
DocSetting::refreshWhenError(bool rwe)
{
    SlgUtil::Editor::refreshWhenError(rwe);
}


bool
DocSetting::exportNewImage(void)
{
    return ExtraTextureManager::exportNewImage();
}

void
DocSetting::exportNewImage(bool eni)
{
    ExtraTextureManager::exportNewImage(eni);
}

bool
DocSetting::exportNewMesh(void)
{
    return ObjectNode::sv_exprtNewMesh;
}

void
DocSetting::exportNewMesh(bool enm)
{
    ObjectNode::sv_exprtNewMesh = enm;
}


const std::string&
DocSetting::getHDRLighterPath(void)
{
	slg::RenderSession* session = pDocData->getSession();
    if(session && session->renderConfig->scene)
    {
        slg::Scene  *scene = session->renderConfig->scene;
        if(scene->envLight && scene->envLight->GetType () == slg::TYPE_IL)
        {
            return ExtraSettingManager::getImageMapPath(scene->imgMapCache,dynamic_cast<slg::InfiniteLight*>(scene->envLight)->GetImageMap());
        }
    }
    throw std::runtime_error("invlaid image map");
}


slg::Scene*
DocSetting::getScene(void)
{
	slg::RenderSession* session = pDocData->getSession();
    if(session && session->film)
    {
        return session->renderConfig->scene;
    }
    return NULL;
}

bool
DocSetting::changeSkyEnv(void)
{
    bool    bSetOK = false;
	slg::RenderSession* session = pDocData->getSession();
    if(session && session->renderConfig->scene)
    {
        slg::Scene  *scene = session->renderConfig->scene;

        session->Stop();
        if(scene->envLight)
        {
            BOOST_ASSERT_MSG(scene->envLight->GetType () != slg::TYPE_IL_SKY,"change to same type?");
            delete scene->envLight;
            scene->envLight = NULL;
        }
        luxrays::Properties prop;
        prop.SetString("scene.skylight.dir","0.0 0.0 1.0");
        scene->AddSkyLight(prop);
        session->Start();
        pDocData->cachefilm().invalidate();
        bSetOK = true;
        pDocData->setModified();
    }
    return bSetOK;
}

bool
DocSetting::enableSun(void)
{
    bool    bSetOK = false;
	slg::RenderSession* session = pDocData->getSession();
    if(session && session->renderConfig->scene)
    {
        slg::Scene  *scene = session->renderConfig->scene;
        if(!scene->sunLight)
        {
            SlgUtil::Editor editor(session);
            editor.addAction(slg::SUNLIGHT_EDIT);
            editor.addAction(slg::MATERIAL_TYPES_EDIT);
            //session->Stop();
            luxrays::Properties prop;
            prop.SetString("scene.sunlight.dir","0.0 0.0 1.0");
            scene->AddSunLight(prop);
            //session->Start();
            //pDocData->cachefilm().invalidate();
            bSetOK = true;
            pDocData->setModified();
        }
    }
    return bSetOK;
}

bool
DocSetting::disableSun(void)
{
    bool    bSetOK = false;
	slg::RenderSession* session = pDocData->getSession();
    if(session && session->renderConfig->scene)
    {
        slg::Scene  *scene = session->renderConfig->scene;
        if(ExtraSettingManager::hasTwoOrMoreLighter(scene) && scene->sunLight)
        {
            SlgUtil::Editor editor(session);
            editor.addAction(slg::SUNLIGHT_EDIT);
            editor.addAction(slg::MATERIAL_TYPES_EDIT);
            //session->Stop();
            delete scene->sunLight;
            scene->sunLight = NULL;
            //session->Start();
            //pDocData->cachefilm().invalidate();
            bSetOK = true;
            pDocData->setModified();
        }
    }
    return bSetOK;
}

bool
DocSetting::disableEnv(void)
{
    bool    bSetOK = false;
	slg::RenderSession* session = pDocData->getSession();
    if(session && session->renderConfig->scene)
    {
        slg::Scene  *scene = session->renderConfig->scene;
        if(ExtraSettingManager::hasTwoOrMoreLighter(scene) && scene->envLight)
        {
            session->Stop();
            delete scene->envLight;
            scene->envLight = NULL;
            session->Start();
            pDocData->cachefilm().invalidate();
            bSetOK = true;
            pDocData->setModified();
        }
    }
    return bSetOK;
}

bool
DocSetting::changeSunTurbidity(float t)
{
	slg::RenderSession* session = pDocData->getSession();
    if(session && session->renderConfig->scene->sunLight)
    {
        SlgUtil::Editor editor(session);
        session->renderConfig->scene->sunLight->SetTurbidity(t);
        editor.addAction(slg::SUNLIGHT_EDIT);
        pDocData->setModified();
        return true;
    }
    return false;
}

bool
DocSetting::changeSunRelsize(float s)
{
	slg::RenderSession* session = pDocData->getSession();
    if(session && session->renderConfig->scene->sunLight)
    {
        SlgUtil::Editor editor(session);
        session->renderConfig->scene->sunLight->SetRelSize(s);
        editor.addAction(slg::SUNLIGHT_EDIT);
        pDocData->setModified();
        return true;
    }
    return false;
}

bool
DocSetting::changeSunGain(const luxrays::Spectrum &g)
{
	slg::RenderSession* session = pDocData->getSession();
    if(session && session->renderConfig->scene->sunLight)
    {
        SlgUtil::Editor editor(session);
        session->renderConfig->scene->sunLight->SetGain(g);
        session->renderConfig->scene->sunLight->Preprocess();
        editor.addAction(slg::SUNLIGHT_EDIT);
        pDocData->setModified();
        return true;
    }
    return false;
}

bool
DocSetting::changeSunDir(const luxrays::Vector &dir)
{
	slg::RenderSession* session = pDocData->getSession();
    if(session && session->renderConfig->scene->sunLight)
    {
        SlgUtil::Editor editor(session);
        session->renderConfig->scene->sunLight->SetDir(dir);
        editor.addAction(slg::SUNLIGHT_EDIT);
        pDocData->setModified();
        return true;
    }
    return false;
}

bool
DocSetting::changeSunDir(float filmx,float filmy)
{
	slg::RenderSession* session = pDocData->getSession();
    if(session && session->renderConfig->scene->sunLight)
    {
        slg::PerspectiveCamera *camera = session->renderConfig->scene->camera;
        float radius = ExtraCameraManager::getCurrentRadius(session,camera);

        luxrays::Ray  eyeRay;
        float oldlr = camera->lensRadius;
        camera->lensRadius = 0.0f;
        camera->GenerateRay(filmx,filmy,&eyeRay,0.001f,0.012f);
        camera->lensRadius = oldlr;

        luxrays::BSphere    sphere;
        sphere.center = camera->target;
        sphere.rad = radius;

        luxrays::Point  pt;
        if(ExtraCameraManager::SphereIntersect(sphere,eyeRay,pt))
        {
            SlgUtil::Editor editor(session);
            luxrays::Vector dir = camera->target - pt;
            session->renderConfig->scene->sunLight->SetDir(dir);
            editor.addAction(slg::SUNLIGHT_EDIT);
            pDocData->setModified();
            return true;
        }
    }
    return false;
}


bool
DocSetting::changeSkyDir(const luxrays::Vector &dir)
{
	slg::RenderSession* session = pDocData->getSession();
    if(session && session->renderConfig->scene->envLight)
    {
        slg::SkyLight  *pRealLight = dynamic_cast<slg::SkyLight *>(session->renderConfig->scene->envLight);
        if(pRealLight)
        {
            SlgUtil::Editor editor(session);
            pRealLight->SetSunDir(dir);
            editor.addAction(slg::SKYLIGHT_EDIT);
            pDocData->setModified();
            return true;
        }
    }
    return false;
}

bool
DocSetting::changeSkyTurbidity(float t)
{
	slg::RenderSession* session = pDocData->getSession();
    if(session && session->renderConfig->scene->envLight)
    {
        slg::SkyLight  *pRealLight = dynamic_cast<slg::SkyLight *>(session->renderConfig->scene->envLight);
        if(pRealLight)
        {
            SlgUtil::Editor editor(session);
            pRealLight->SetTurbidity(t);
            editor.addAction(slg::SKYLIGHT_EDIT);
            pDocData->setModified();
            return true;
        }
    }
    return false;
}

bool
DocSetting::changeEnvUDelta(float d)
{
	slg::RenderSession* session = pDocData->getSession();
    if(session && session->renderConfig->scene->envLight)
    {
        slg::InfiniteLight *pRealLight = dynamic_cast<slg::InfiniteLight*>(session->renderConfig->scene->envLight);
        if(pRealLight)
        {
            SlgUtil::Editor editor(session);
            pRealLight->GetUVMapping()->uDelta = d;
            editor.addAction(slg::INFINITELIGHT_EDIT);
            pDocData->setModified();
            return true;
        }
    }
    return false;
}

bool
DocSetting::changeEnvVDelta(float d)
{
	slg::RenderSession* session = pDocData->getSession();
    if(session && session->renderConfig->scene->envLight)
    {
        slg::InfiniteLight *pRealLight = dynamic_cast<slg::InfiniteLight*>(session->renderConfig->scene->envLight);
        if(pRealLight)
        {
            SlgUtil::Editor editor(session);
            pRealLight->GetUVMapping()->vDelta = d;
            editor.addAction(slg::INFINITELIGHT_EDIT);
            pDocData->setModified();
            return true;
        }
    }
    return false;
}

bool
DocSetting::changeEnvGain(const luxrays::Spectrum &g)
{
	slg::RenderSession* session = pDocData->getSession();
    if(session && session->renderConfig->scene->envLight)
    {
        SlgUtil::Editor editor(session);
        session->renderConfig->scene->envLight->SetGain(g);
        if(session->renderConfig->scene->envLight->GetType() == slg::TYPE_IL)
        {
            editor.addAction(slg::INFINITELIGHT_EDIT);
        }else{
            session->renderConfig->scene->envLight->Preprocess();
            editor.addAction(slg::SKYLIGHT_EDIT);
        }
        pDocData->setModified();
        return true;
    }
    return false;
}


bool
DocSetting::changeHDRfile(const std::string &fullpath)
{
    bool    bSetOK = false;
	slg::RenderSession* session = pDocData->getSession();
    if(session && session->renderConfig->scene)
    {
        slg::Scene  *scene = session->renderConfig->scene;

        luxrays::Properties prop;
        if(scene->envLight && scene->envLight->GetType () == slg::TYPE_IL)
        {
            prop = scene->envLight->ToProperties(scene->imgMapCache);
        }
        prop.SetString("scene.infinitelight.file",fullpath);

        //SlgUtil::Editor editor(session);
        session->Stop();
		if(scene->envLight)
		{
			delete scene->envLight;
			scene->envLight = NULL;
		}
        scene->AddInfiniteLight(prop);
        session->Start();
        pDocData->cachefilm().invalidate();
        //editor.addAction(slg::INFINITELIGHT_EDIT);
        pDocData->setModified();
        bSetOK = true;
    }
    return bSetOK;
}

const slg::ToneMapParams*
DocSetting::getToneMapParams(void)
{
    if(pDocData->getSession() && pDocData->getSession()->film)
    {
        return pDocData->getSession()->film->GetToneMapParams();
    }
    return NULL;
}

bool
DocSetting::setToneMapParams(const slg::ToneMapParams &param)
{
    if(pDocData->getSession() && pDocData->getSession()->film)
    {
        pDocData->getSession()->film->SetToneMapParams(param);
        pDocData->setModified();
        return true;
    }
    return false;
}

int
DocSetting::getFilmFilter(void)
{
    if(pDocData->getSession() && pDocData->getSession()->film)
    {
        return pDocData->getSession()->film->GetFilterType();
    }
    return -1;
}

bool
DocSetting::setFilmFilter(int type)
{
    if(pDocData->getSession() && pDocData->getSession()->film)
    {
        slg::FilterType ft = (slg::FilterType)type;
        pDocData->getSession()->film->SetFilterType(ft);
        pDocData->setModified();
        return true;
    }
    return false;
}

float
DocSetting::getGamma(void)
{
    if(pDocData->getSession() && pDocData->getSession()->film)
    {
        return pDocData->getSession()->film->GetGamma();
    }
    return 2.2f;
}

bool
DocSetting::setGamma(float g)
{
    if(pDocData->getSession() && pDocData->getSession()->film)
    {
        pDocData->getSession()->film->InitGammaTable(g);
        pDocData->setModified();
        return true;
    }
    return false;
}

int
DocSetting::getSamplerType(void)
{
    int sampler = -1;
    if(pDocData->getSession())
    {
        sampler = (int)slg::Sampler::String2SamplerType(pDocData->getSession()->renderConfig->cfg.GetString("sampler.type","METROPOLIS"));
    }
    return sampler;
}

bool
DocSetting::setSamplerType(int type)
{
    if(pDocData->getSession())
    {
        try{
            std::string   typestr = slg::Sampler::SamplerType2String((const slg::SamplerType)type);

            pDocData->pause();

            pDocData->getSession()->renderConfig->cfg.SetString("sampler.type",typestr);
            pDocData->getSession()->renderConfig->cfg.SetString("path.sampler.type",typestr);

            pDocData->start();
            pDocData->setModified();
            return true;
        }catch(std::runtime_error e)
        {
        }
    }
    return false;
}

int
DocSetting::getEngineType(void)
{
    if(pDocData->getSession() && pDocData->getSession()->renderEngine)
    {
        return pDocData->getSession()->renderEngine->GetEngineType();
    }
    return -1;
}

bool
DocSetting::setEngineType(int type)
{
    if(pDocData->getSession())
    {
        if(!pDocData->getSession()->renderEngine || type != pDocData->getSession()->renderEngine->GetEngineType())
        {
            try{
                pDocData->cachefilm().saveNativeFilm();
                pDocData->getSession()->SetRenderingEngineType((const slg::RenderEngineType)type);
                pDocData->setModified();
                return true;
            }catch(std::runtime_error e)
            {
            }
        }
    }
    return false;
}

int
DocSetting::getPathDepth(void)
{
    int depth = 8;
    if(pDocData->getSession())
    {
        depth = pDocData->getSession()->renderConfig->cfg.GetInt("path.maxdepth",8);
    }
    return depth;
}

bool
DocSetting::setPathDepth(int depth)
{
    if(depth != getPathDepth() && pDocData->getSession())
    {
        try{
            pDocData->pause();

            pDocData->getSession()->renderConfig->cfg.SetString("path.maxdepth",boost::lexical_cast<std::string>(depth));

            pDocData->start();
            pDocData->setModified();
            return true;
        }catch(std::runtime_error e)
        {
        }
    }
    return false;
}

int
DocSetting::getRouletteDepth(void)
{
    int depth = 3;
    if(pDocData->getSession())
    {
        depth = pDocData->getSession()->renderConfig->cfg.GetInt("path.russianroulette.depth",3);
    }
    return depth;
}

bool
DocSetting::setRouletteDepth(int depth)
{
    if(depth != getRouletteDepth() && pDocData->getSession())
    {
        pDocData->pause();

        pDocData->getSession()->renderConfig->cfg.SetString("path.russianroulette.depth",boost::lexical_cast<std::string>(depth));

        pDocData->start();
        pDocData->setModified();
        return true;
    }
    return false;
}

float
DocSetting::getRouletteCap(void)
{
    float cap = 0.5f;
    if(pDocData->getSession())
    {
        cap = pDocData->getSession()->renderConfig->cfg.GetFloat("path.russianroulette.cap",0.5f);
    }
    return cap;
}

bool
DocSetting::setRouletteCap(float cap)
{
    if(cap != getRouletteCap() && pDocData->getSession())
    {
        pDocData->pause();

        pDocData->getSession()->renderConfig->cfg.SetString("path.russianroulette.cap",boost::lexical_cast<std::string>(cap));

        pDocData->start();
        pDocData->setModified();
        return true;
    }
    return false;
}

float
DocSetting::getLargesteprate(void)
{
    float value = 0.4f;
    if(pDocData->getSession())
    {
        value = pDocData->getSession()->renderConfig->cfg.GetFloat("sampler.largesteprate",0.4f);
    }
    return value;
}

void
DocSetting::setLargesteprate(float value)
{
    if(value != getLargesteprate() && pDocData->getSession())
    {
        pDocData->pause();

        pDocData->getSession()->renderConfig->cfg.SetString("sampler.largesteprate",boost::lexical_cast<std::string>(value));

        pDocData->start();
        pDocData->setModified();
    }
}

float
DocSetting::getImagemutationrate(void)
{
    float value = 0.1f;
    if(pDocData->getSession())
    {
        value = pDocData->getSession()->renderConfig->cfg.GetFloat("sampler.imagemutationrate",0.1f);
    }
    return value;
}

void
DocSetting::setImagemutationrate(float value)
{
    if(value != getImagemutationrate() && pDocData->getSession())
    {
        pDocData->pause();

        pDocData->getSession()->renderConfig->cfg.SetString("sampler.imagemutationrate",boost::lexical_cast<std::string>(value));

        pDocData->start();
        pDocData->setModified();
    }
}

float
DocSetting::getMaxconsecutivereject(void)
{
    float value = 512.0f;
    if(pDocData->getSession())
    {
        value = pDocData->getSession()->renderConfig->cfg.GetFloat("sampler.maxconsecutivereject",512.0f);
    }
    return value;
}

void
DocSetting::setMaxconsecutivereject(float value)
{
    if(value != getMaxconsecutivereject() && pDocData->getSession())
    {
        pDocData->pause();

        pDocData->getSession()->renderConfig->cfg.SetString("sampler.maxconsecutivereject",boost::lexical_cast<std::string>(value));

        pDocData->start();
        pDocData->setModified();
    }
}

int
DocSetting::getPathFilterType(void)
{
    int retType = (int)slg::ocl::FILTER_NONE;
    if(pDocData->getSession())
    {
        std::string ft = pDocData->getSession()->renderConfig->cfg.GetString("path.filter.type","NONE");
        if(boost::equals(ft,"NONE"))
        {
            retType = (int)slg::ocl::FILTER_NONE;
        }else if(boost::equals(ft,"BOX"))
        {
            retType = (int)slg::ocl::FILTER_BOX;
        }else if(boost::equals(ft,"GAUSSIAN"))
        {
            retType = (int)slg::ocl::FILTER_GAUSSIAN;
        }else if(boost::equals(ft,"MITCHELL"))
        {
            retType = (int)slg::ocl::FILTER_MITCHELL;
        }
    }
    return retType;
}

bool
DocSetting::setPathFilterType(int type)
{
    bool    bSetOK = false;
    if(type != getPathFilterType() && pDocData->getSession())
    {
        bSetOK = true;
        switch(type)
        {
        case slg::ocl::FILTER_NONE:
            pDocData->getSession()->renderConfig->cfg.SetString("path.filter.type","NONE");
            break;
        case slg::ocl::FILTER_BOX:
            pDocData->getSession()->renderConfig->cfg.SetString("path.filter.type","BOX");
            break;
        case slg::ocl::FILTER_GAUSSIAN:
            pDocData->getSession()->renderConfig->cfg.SetString("path.filter.type","GAUSSIAN");
            break;
        case slg::ocl::FILTER_MITCHELL:
            pDocData->getSession()->renderConfig->cfg.SetString("path.filter.type","MITCHELL");
            break;
        default:
            bSetOK = false;
            break;
        }
        if(bSetOK)
        {
            pDocData->pause();
            pDocData->start();
            pDocData->setModified();
        }
    }
    return bSetOK;
}

float
DocSetting::getPFWidthX(void)
{
    float value = 1.5f;
    if(pDocData->getSession())
    {
        value = pDocData->getSession()->renderConfig->cfg.GetFloat("path.filter.width.x",1.5f);
    }
    return value;
}

bool
DocSetting::setPFWidthX(float value)
{
    if(value != getPFWidthX() && pDocData->getSession() && value > 0.0f && value <= 1.5f)
    {
        pDocData->pause();
        pDocData->getSession()->renderConfig->cfg.SetString("path.filter.width.x",boost::lexical_cast<std::string>(value));
        pDocData->start();
        pDocData->setModified();
        return true;
    }
    return false;
}

float
DocSetting::getPFWidthY(void)
{
    float value = 1.5f;
    if(pDocData->getSession())
    {
        value = pDocData->getSession()->renderConfig->cfg.GetFloat("path.filter.width.y",1.5f);
    }
    return value;
}

bool
DocSetting::setPFWidthY(float value)
{
    if(value != getPFWidthY() && pDocData->getSession() && value > 0.0f && value <= 1.5f)
    {
        pDocData->pause();
        pDocData->getSession()->renderConfig->cfg.SetString("path.filter.width.y",boost::lexical_cast<std::string>(value));
        pDocData->start();
        pDocData->setModified();
        return true;
    }
    return false;
}

float
DocSetting::getPFAlpha(void)
{
    float value = 2.0f;
    if(pDocData->getSession())
    {
        value = pDocData->getSession()->renderConfig->cfg.GetFloat("path.filter.alpha",2.0f);
    }
    return value;
}

bool
DocSetting::setPFAlpha(float value)
{
    if(value != getPFAlpha() && pDocData->getSession())
    {
        pDocData->pause();
        pDocData->getSession()->renderConfig->cfg.SetString("path.filter.alpha",boost::lexical_cast<std::string>(value));
        pDocData->start();
        pDocData->setModified();
        return true;
    }
    return false;
}

float
DocSetting::getPFilterB(void)
{
    float value = 1.f / 3.f;
    if(pDocData->getSession())
    {
        value = pDocData->getSession()->renderConfig->cfg.GetFloat("path.filter.B",value);
    }
    return value;
}

bool
DocSetting::setPFilterB(float value)
{
    if(value != getPFilterB() && pDocData->getSession())
    {
        pDocData->pause();
        pDocData->getSession()->renderConfig->cfg.SetString("path.filter.B",boost::lexical_cast<std::string>(value));
        pDocData->start();
        pDocData->setModified();
        return true;
    }
    return false;
}

float
DocSetting::getPFilterC(void)
{
    float value = 1.f / 3.f;
    if(pDocData->getSession())
    {
        value = pDocData->getSession()->renderConfig->cfg.GetFloat("path.filter.C",value);
    }
    return value;
}

bool
DocSetting::setPFilterC(float value)
{
    if(value != getPFilterC() && pDocData->getSession())
    {
        pDocData->pause();
        pDocData->getSession()->renderConfig->cfg.SetString("path.filter.C",boost::lexical_cast<std::string>(value));
        pDocData->start();
        pDocData->setModified();
        return true;
    }
    return false;
}



//
//// get toneMap properties
//type_xml_node*
//DocSetting::getToneMap(type_xml_node &parent)
//{
//	type_xml_doc *pDoc = parent.document();
//    BOOST_ASSERT_MSG(pDoc != NULL,"invalid node,must from doc");
//
//	slg::RenderSession* session = pDocData->getSession();
//    if(session && session->film)
//    {
//		slg::Film *film = session->film;
//        type_xml_node * pSelf = pDoc->allocate_node(NS_RAPIDXML::node_element,"tonemap");
//		parent.append_node(pSelf);
//
//		int type = film->GetToneMapParams()->GetType();
//		std::string typeName = getToneMapTypeNameByName(type);
//		pSelf->append_attribute(pDoc->allocate_attribute(constDef::type,allocate_string(pDoc,typeName)));
//		type_xml_node *pParams = pDoc->allocate_node(NS_RAPIDXML::node_element,allocate_string(pSelf->document(),typeName));
//		pSelf->append_node(pParams);
//		if(typeName == "linear")
//		{
//			slg::LinearToneMapParams *params = (slg::LinearToneMapParams *)film->GetToneMapParams()->Copy();
//			pParams->append_attribute(allocate_attribute_withkey(pDoc,"scale",boost::lexical_cast<std::string>( params->scale )));
//			delete params;
//		}
//		else if(typeName == "reinhard02")
//		{
//			slg::Reinhard02ToneMapParams *params = (slg::Reinhard02ToneMapParams *)film->GetToneMapParams()->Copy();
//			pParams->append_attribute(allocate_attribute_withkey(pDoc,"burn",boost::lexical_cast<std::string>( params->burn )));
//			pParams->append_attribute(allocate_attribute_withkey(pDoc,"postScale",boost::lexical_cast<std::string>( params->postScale )));
//			pParams->append_attribute(allocate_attribute_withkey(pDoc,"preScale",boost::lexical_cast<std::string>( params->preScale )));
//			delete params;
//		}
//		return pSelf;
//	}
//	return NULL;
//}

//std::string
//DocSetting::getToneMapTypeNameByName(int type)
//{
//	std::string typeName;
//	switch(type)
//	{
//	case slg::TONEMAP_NONE:
//		typeName = "none";
//		break;
//	case slg::TONEMAP_LINEAR:
//		typeName = "linear";
//		break;
//	case slg::TONEMAP_REINHARD02:
//		typeName = "reinhard02";
//		break;
//	default:
//		break;
//	}
//	return typeName;
//}
//
//bool
//DocSetting::getLinearScale(float &ls)
//{
//	bool ret = false;
//	slg::RenderSession* session = pDocData->getSession();
//    if(session && session->film)
//    {
//		slg::Film *film = session->film;
//		if(film->GetToneMapParams()->GetType() == slg::TONEMAP_LINEAR)
//		{
//			slg::LinearToneMapParams *params = (slg::LinearToneMapParams *)film->GetToneMapParams()->Copy();
//			ls = params->scale;
//			delete params;
//			ret = true;
//		}
//	}
//    return ret;
//}
//
//bool
//DocSetting::setLinearScale(float ls)
//{
//	slg::RenderSession* session = pDocData->getSession();
//    if(session && session->film)
//    {
//		slg::Film *film = session->film;
//		slg::LinearToneMapParams *params = (slg::LinearToneMapParams *)film->GetToneMapParams()->Copy();
//		params->scale = ls;
//		film->SetToneMapParams(*params);
//		delete params;
//		return true;
//	}
//    return false;
//}

}
