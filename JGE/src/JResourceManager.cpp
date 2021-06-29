//-------------------------------------------------------------------------------------
//
// JGE++ is a hardware accelerated 2D game SDK for PSP/Windows.
//
// Licensed under the BSD license, see LICENSE in JGE root for details.
//
// Copyright (c) 2007 James Hui (a.k.a. Dr.Watson) <jhkhui@gmail.com>
//
//-------------------------------------------------------------------------------------

#include "../include/JGE.h"
#include "../include/JRenderer.h"
#include "../include/JSoundSystem.h"
#include "../include/JResourceManager.h"
#include "../include/JFileSystem.h"
#include "../include/JLBFont.h"
#include "tinyxml/tinyxml.h"

#if defined(_DEBUG) && defined(WIN32)
    #include "crtdbg.h"
    #define NEW new (_NORMAL_BLOCK, __FILE__, __LINE__)
#else
    #define NEW new
#endif

JResourceManager::JResourceManager() {
    // mResourceRoot = "Res/";				// default root folder

    mTextureList.clear();
    mTextureList.reserve(16);
    mTextureMap.clear();

    mQuadList.clear();
    mQuadList.reserve(128);
    mQuadMap.clear();

    mFontList.clear();
    mFontList.reserve(4);
    mFontMap.clear();

    // 	mParticleEffectList.clear();
    // 	mParticleEffectList.reserve(8);
    // 	mParticleEffectMap.clear();
    //
    // 	mMotionEmitterList.clear();
    // 	mMotionEmitterList.reserve(16);
    // 	mMotionEmitterMap.clear();
}

JResourceManager::~JResourceManager() { RemoveAll(); }

void JResourceManager::RemoveJLBFonts() {
    for (auto& font : mFontList) delete font;

    mFontList.clear();
    mFontMap.clear();
}

void JResourceManager::RemoveAll() {
    for (auto& tex : mTextureList) delete tex;

    mTextureList.clear();
    mTextureMap.clear();

    for (auto& quad : mQuadList) delete quad;

    mQuadList.clear();
    mQuadMap.clear();

    RemoveJLBFonts();
}

bool JResourceManager::LoadResource(const std::string& resourceName) {
    std::string path = /*mResourceRoot + */ resourceName;

    JGE* engine = JGE::GetInstance();
    if (engine == nullptr) return false;

    JFileSystem* fileSystem = JFileSystem::GetInstance();
    if (fileSystem == nullptr) return false;

    if (!fileSystem->OpenFile(path.c_str())) return false;

    int size = fileSystem->GetFileSize();
    char* xmlBuffer = new char[size];
    fileSystem->ReadFile(xmlBuffer, size);

    TiXmlDocument doc;
    doc.Parse(xmlBuffer);

    TiXmlNode* resource   = nullptr;
    TiXmlNode* node       = nullptr;
    TiXmlElement* element = nullptr;

    resource = doc.FirstChild("resource");
    if (resource) {
        element = resource->ToElement();
        printf("---- Loading %s:%s\n", element->Value(), element->Attribute("name"));

        for (node = resource->FirstChild(); node; node = node->NextSibling()) {
            element = node->ToElement();
            if (element != nullptr) {
                if (strcmp(element->Value(), "texture") == 0) {
                    CreateTexture(element->Attribute("name"));
                } else if (strcmp(element->Value(), "quad") == 0) {
                    std::string quadName = element->Attribute("name");
                    std::string textureName = element->Attribute("texture");
                    float x = 0.0f;
                    float y = 0.0f;
                    float width = 16.0f;
                    float height = 16.0f;
                    float value;
                    float hotspotX = 0.0f;
                    float hotspotY = 0.0f;

                    if (element->QueryFloatAttribute("x", &value) == TIXML_SUCCESS) x = value;

                    if (element->QueryFloatAttribute("y", &value) == TIXML_SUCCESS) y = value;

                    if (element->QueryFloatAttribute("width", &value) == TIXML_SUCCESS) width = value;

                    if (element->QueryFloatAttribute("height", &value) == TIXML_SUCCESS) height = value;

                    if (element->QueryFloatAttribute("w", &value) == TIXML_SUCCESS) width = value;

                    if (element->QueryFloatAttribute("h", &value) == TIXML_SUCCESS) height = value;

                    if (element->QueryFloatAttribute("hotspot.x", &value) == TIXML_SUCCESS)
                        hotspotX = value;
                    else
                        hotspotX = width / 2;

                    if (element->QueryFloatAttribute("hotspot.y", &value) == TIXML_SUCCESS)
                        hotspotY = value;
                    else
                        hotspotY = height / 2;

                    int id = CreateQuad(quadName, textureName, x, y, width, height);
                    if (id != INVALID_ID) {
                        GetQuad(id)->SetHotSpot(hotspotX, hotspotY);
                    }
                } else if (strcmp(element->Value(), "font") == 0) {
                }
            }
        }
    }

    fileSystem->CloseFile();
    delete[] xmlBuffer;
    //	JGERelease();

    return true;
}

int JResourceManager::CreateTexture(const std::string& textureName) {
    auto itr = mTextureMap.find(textureName);

    if (itr == mTextureMap.end()) {
        std::string path = /*mResourceRoot + */ textureName;

        printf("creating texture:%s\n", path.c_str());

        JTexture* tex = JRenderer::GetInstance()->LoadTexture(path.c_str());

        if (tex == nullptr) return INVALID_ID;

        int id = mTextureList.size();
        mTextureList.push_back(tex);
        mTextureMap[textureName] = id;

        return id;
    } else
        return itr->second;
}

JTexture* JResourceManager::GetTexture(const std::string& textureName) {
    auto itr = mTextureMap.find(textureName);

    if (itr == mTextureMap.end())
        return nullptr;
    else
        return mTextureList[itr->second];
}

JTexture* JResourceManager::GetTexture(int id) {
    if (id >= 0 && id < (int)mTextureList.size())
        return mTextureList[id];
    else
        return nullptr;
}

int JResourceManager::CreateQuad(const std::string& quadName, const std::string& textureName, float x, float y,
                                 float width, float height) {
    auto itr = mQuadMap.find(quadName);

    if (itr == mQuadMap.end()) {
        JTexture* tex = GetTexture(textureName);
        if (tex == nullptr) {
            int texId = CreateTexture(textureName);  // load texture if necessary
            tex = GetTexture(texId);
        }

        if (tex == nullptr)  // no texture, no quad...
            return INVALID_ID;

        printf("creating quad:%s\n", quadName.c_str());

        int id = mQuadList.size();
        mQuadList.push_back(NEW JQuad(tex, x, y, width, height));

        mQuadMap[quadName] = id;

        return id;

    } else
        return itr->second;
}

JQuad* JResourceManager::GetQuad(const std::string& quadName) {
    auto itr = mQuadMap.find(quadName);

    if (itr == mQuadMap.end())
        return nullptr;
    else
        return mQuadList[itr->second];
}

JQuad* JResourceManager::GetQuad(int id) {
    if (id >= 0 && id < (int)mQuadList.size())
        return mQuadList[id];
    else
        return nullptr;
}

JLBFont* JResourceManager::LoadJLBFont(const std::string& fontName, int height) {
    auto itr = mFontMap.find(fontName);

    if (itr != mFontMap.end()) return mFontList[itr->second];

    std::string path = fontName;

    int id = mFontList.size();

    mFontList.push_back(NEW JLBFont(path.c_str(), height, true));

    mFontMap[fontName] = id;

    return mFontList[id];
}

JLBFont* JResourceManager::GetJLBFont(const std::string& fontName) {
    auto itr = mFontMap.find(fontName);

    if (itr == mFontMap.end())
        return nullptr;
    else
        return mFontList[itr->second];
}

JLBFont* JResourceManager::GetJLBFont(int id) {
    if (id >= 0 && id < (int)mFontList.size())
        return mFontList[id];
    else
        return nullptr;
}

//
//
// int JResourceManager::RegisterParticleEffect(const std::string &effectName)
// {
// 	map<string, int>::iterator itr = mParticleEffectMap.find(effectName);
//
// 	if (itr == mParticleEffectMap.end())
// 	{
// 		string path = mResourceRoot + effectName;
// 		printf("creating effect:%s\n", path.c_str());
// 		JParticleEffect *effect = new JParticleEffect(path.c_str());
//
// 		if (effect == NULL)
// 			return INVALID_ID;
//
//
// 		int id = mParticleEffectList.size();
// 		mParticleEffectList.push_back(effect);
//
// 		mParticleEffectMap[effectName] = id;
//
// 		return id;
//
// 	}
// 	else
// 		return itr->second;
// }
//
//
// JParticleEffect *JResourceManager::GetParticleEffect(const std::string &effectName)
// {
// 	map<string, int>::iterator itr = mParticleEffectMap.find(effectName);
//
// 	if (itr == mParticleEffectMap.end())
// 		return NULL;
// 	else
// 		return mParticleEffectList[itr->second];
// }
//
//
// JParticleEffect *JResourceManager::GetParticleEffect(int id)
// {
// 	if (id >=0 && id < (int)mParticleEffectList.size())
// 		return mParticleEffectList[id];
// 	else
// 		return NULL;
// }
//
//
//
// int JResourceManager::RegisterMotionEmitter(const std::string &emitterName)
// {
// 	map<string, int>::iterator itr = mMotionEmitterMap.find(emitterName);
//
// 	if (itr == mMotionEmitterMap.end())
// 	{
// 		string path = mResourceRoot + emitterName;
// 		printf("creating effect:%s\n", path.c_str());
// 		JMotionEmitter *emitter = new JMotionEmitter();
//
// 		if (emitter == NULL)
// 			return INVALID_ID;
//
// 		emitter->LoadMotionML(path.c_str());
//
// 		int id = mMotionEmitterList.size();
// 		mMotionEmitterList.push_back(emitter);
//
// 		mMotionEmitterMap[emitterName] = id;
//
// 		return id;
//
// 	}
// 	else
// 		return itr->second;
// }
//
//
// JMotionEmitter *JResourceManager::GetMotionEmitter(const std::string &emitterName)
// {
// 	map<string, int>::iterator itr = mMotionEmitterMap.find(emitterName);
//
// 	if (itr == mMotionEmitterMap.end())
// 		return NULL;
// 	else
// 		return mMotionEmitterList[itr->second];
// }
//
//
// JMotionEmitter *JResourceManager::GetMotionEmitter(int id)
// {
// 	if (id >=0 && id < (int)mMotionEmitterList.size())
// 		return mMotionEmitterList[id];
// 	else
// 		return NULL;
// }
