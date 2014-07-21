//
//  RenderModelCA.cpp
//  BasicPlugin
//
//  Created by ZhehaoWang on 7/17/14.
//
//

#include "RenderModel.h"

void RenderModelCA::renderInRect(uint8_t *buffer, CGContextRef context, CGRect frame, size_t width, size_t height)
{
    
}

void RenderModelCA::drawPlugin(NPP instance, NPCocoaEvent *event)
{
    
}

NPError RenderModelCA::selectRenderModel(NPP instance, NPNetscapeFuncs *browser)
{
    NPBool supportsCoreAnimation = false;
    if (browser->getvalue(instance, NPNVsupportsCoreAnimationBool, &supportsCoreAnimation) == NPERR_NO_ERROR && supportsCoreAnimation) {
        browser->setvalue(instance, NPPVpluginDrawingModel, (void*)NPDrawingModelCoreAnimation);
    } else {
        printf("CoreGraphics drawing model not supported, can't create a plugin instance.\n");
        return NPERR_INCOMPATIBLE_VERSION_ERROR;
    }
    
    return NPERR_NO_ERROR;
}