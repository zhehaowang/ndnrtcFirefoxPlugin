//
//  CGRenderer.h
//  BasicPlugin
//
//  Created by ZhehaoWang on 7/17/14.
//
//

#ifndef __BasicPlugin__CGRenderer__
#define __BasicPlugin__CGRenderer__

#include "npapiHeaders/npapi.h"
#include "npapiHeaders/npfunctions.h"

#include "basic-plugin.h"

// RenderModel is the interface class for actual renderers.
class RenderModel
{
public:
    RenderModel() {};
    
    virtual void drawPlugin(NPP instance, NPCocoaEvent* event) = 0;
    virtual void renderInRect(uint8_t *buffer, CGContextRef context, CGRect frame, size_t width, size_t height) = 0;
    virtual NPError selectRenderModel(NPP instance, NPNetscapeFuncs* browser) = 0;
};

class RenderModelCG : public RenderModel
{
public:
    RenderModelCG() {};
    
    void drawPlugin(NPP instance, NPCocoaEvent* event);
    void renderInRect(uint8_t *buffer, CGContextRef context, CGRect frame, size_t width, size_t height);
    NPError selectRenderModel(NPP instance, NPNetscapeFuncs* browser);
};

class RenderModelCA : public RenderModel
{
public:
    RenderModelCA() {};
    
    void drawPlugin(NPP instance, NPCocoaEvent* event);
    void renderInRect(uint8_t *buffer, CGContextRef context, CGRect frame, size_t width, size_t height);
    NPError selectRenderModel(NPP instance, NPNetscapeFuncs* browser);
};

#endif /* defined(__BasicPlugin__CGRenderer__) */
