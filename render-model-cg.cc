//
//  CGRenderer.cpp
//  BasicPlugin
//
//  Created by ZhehaoWang on 7/17/14.
//
//

#include "RenderModel.h"

// this is a sample function that draws a frame with RGB array as input, using CoreGraphics

void RenderModelCG::renderInRect(uint8_t *buffer, CGContextRef context, CGRect frame, size_t width, size_t height)
{
    CGColorSpaceRef colorSpace = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);
    
    uint8_t * alphaBuffer = (uint8_t *)malloc(width * height * 4);
    
    // According to Quartz/AppleDevelopment documentation, the only byte format for rendering RGB bytes seems to take only 32bpp, so here Peter's 24bpp input is copied, and an empty alpha byte is added after each RGB byte.
    int i = 0, j = 0;
    while (i < width * height * 4)
    {
        alphaBuffer[i++] = buffer[j++];
        alphaBuffer[i++] = buffer[j++];
        alphaBuffer[i++] = buffer[j++];
        
        // wonder why this gives a different result:
        
        //memcpy(alphaBuffer+i, buffer+j, 3);
        //i+=3;
        //j+=3;
        // a random alpha
        
        alphaBuffer[i++] = 0;
    }
    
    CGContextRef bitMapContext = CGBitmapContextCreate(alphaBuffer, width, height, 8, 4*width, colorSpace, kCGImageAlphaNoneSkipFirst | kCGBitmapByteOrder32Little);
    
    CGColorSpaceRelease(colorSpace);
    CGImageRef bitMapImage = CGBitmapContextCreateImage(bitMapContext);
    
    CGContextDrawImage(context, frame, bitMapImage);
    CGImageRelease(bitMapImage);
    
    CGContextRelease(bitMapContext);
    
    free(alphaBuffer);
}

/* drawPlugin is called every time the drawing event is fired, which is called every interval.
 It handles the repaint of the whole plugin. */
void RenderModelCG::drawPlugin(NPP instance, NPCocoaEvent* event)
{
    PluginInstance* currentInstance = (PluginInstance*)(instance->pdata);
    
    CGContextRef cgContext = event->data.draw.context;
    
    if (!cgContext) {
        return;
    }
    
    // Save the cgcontext gstate.
    CGContextSaveGState(cgContext);
    
    // before passing into bitmapContext, flip cgContext
    CGContextTranslateCTM(cgContext, 0.0, defaultWindowHeight);
    CGContextScaleCTM(cgContext, 1.0, -1.0);
    
    // A renderBuffer should be an array of buffers associated with renderBuffer in browserRenderer class, or with browserRenderer itself;
    
    for (int i = 0; i < renderWindowNum; i++)
    {
        // should do a performance comparison between having having the lock vs not (having the swapping buffers vs not)
        
        // Following module implements a renderBuffer switching module, however, the improvement in performance cannot be observed, and the trick in renderRGBFrame is what actually worked.
        /*
         if (renderWindows[i].bufferFilled_)
         {
         renderWindows[i].renderBufferLock_.lock();
         
         printf("Trying to swap RenderBuffer.\n");
         
         uint8_t *temp = renderWindows[i].renderBuffer_;
         renderWindows[i].renderBuffer_ = ((BrowserRenderer *)renderWindows[i].bRenderer_)->getBuffer();
         ((BrowserRenderer *)renderWindows[i].bRenderer_)->setBuffer(temp);
         
         renderWindows[i].bufferFilled_ = false;
         
         printf("RenderBuffer swapped.\n");
         
         renderWindows[i].renderBufferLock_.unlock();
         }
         */
        
        // cgRectMake starts with the bottom left point. the sequence of parameters are left, bottom, width and height.
        if (renderWindows[i].bufferFilled_)
        {
            renderInRect(renderWindows[i].renderBuffer_, cgContext, renderWindows[i].getRect(), defaultWindowWidth, defaultWindowHeight);
        }
    }
    
    // Restore the cgcontext gstate.
    CGContextRestoreGState(cgContext);
}

NPError RenderModelCG::selectRenderModel(NPP instance, NPNetscapeFuncs *browser)
{
    NPBool supportsCoreGraphics = false;
    if (browser->getvalue(instance, NPNVsupportsCoreGraphicsBool, &supportsCoreGraphics) == NPERR_NO_ERROR && supportsCoreGraphics) {
        browser->setvalue(instance, NPPVpluginDrawingModel, (void*)NPDrawingModelCoreGraphics);
    } else {
        printf("CoreGraphics drawing model not supported, can't create a plugin instance.\n");
        return NPERR_INCOMPATIBLE_VERSION_ERROR;
    }
    
    return NPERR_NO_ERROR;
}