/* ***** BEGIN LICENSE BLOCK *****
 *
 * THIS FILE IS PART OF THE MOZILLA NPAPI SDK BASIC PLUGIN SAMPLE
 * SOURCE CODE. USE, DISTRIBUTION AND REPRODUCTION OF THIS SOURCE
 * IS GOVERNED BY A BSD-STYLE SOURCE LICENSE INCLUDED WITH THIS
 * SOURCE IN 'COPYING'. PLEASE READ THESE TERMS BEFORE DISTRIBUTING.
 *
 * THE MOZILLA NPAPI SDK BASIC PLUGIN SAMPLE SOURCE CODE IS
 * (C) COPYRIGHT 2008 by the Mozilla Corporation
 * http://www.mozilla.com/
 *
 * Contributors:
 *  Josh Aas <josh@mozilla.com>
 *
 * ***** END LICENSE BLOCK ***** */

/*
 * This sample plugin uses the Cocoa event model and the Core Graphics
 * drawing model.
 */

// the source is in C, added rendering function tested by Zhehao
// um should remove some of those unnecessary frameworks

// InfoPList.string is considered as irrelevant and deleted.
// Error messages telling the key used being wrong start to arise

#include "BasicPlugin.h"
#include "ndnrtc-library.h"

#include "BrowserRenderer.h"

#define STRINGS_PRODUCTNAME "ndnrtc-plugin"
#define STRINGS_FILEDESCRIPTION "Test description"

using namespace ndnrtc;

//distinguish between extern declarations in headers, and static (only) declaration in headers
uint8_t *renderBuffer;

std::mutex renderBufferLock;

// The npobject to pass into the browser. Made global so that only one copy exists at a time
NPObject *scriptableObj;

/* Symbol called once by the browser to initialize the plugin. */
NPError NP_Initialize(NPNetscapeFuncs* browserFuncs)
{
    /* Save the browser function table. The ones with NPN_ are browser functions, handles the event usually fired from browser */
    browser = browserFuncs;
    
    return NPERR_NO_ERROR;
}

/* Function called by the browser to get the plugin's function table. */
NPError NP_GetEntryPoints(NPPluginFuncs* pluginFuncs)
{
    /* Check the size of the provided structure based on the offset of the
     last member we need. */
    if (pluginFuncs->size < (offsetof(NPPluginFuncs, setvalue) + sizeof(void*)))
        return NPERR_INVALID_FUNCTABLE_ERROR;
    
    pluginFuncs->newp = NPP_New;
    pluginFuncs->destroy = NPP_Destroy;
    pluginFuncs->setwindow = NPP_SetWindow;
    pluginFuncs->newstream = NPP_NewStream;
    pluginFuncs->destroystream = NPP_DestroyStream;
    pluginFuncs->asfile = NPP_StreamAsFile;
    pluginFuncs->writeready = NPP_WriteReady;
    pluginFuncs->write = (NPP_WriteProcPtr)NPP_Write;
    pluginFuncs->print = NPP_Print;
    pluginFuncs->event = NPP_HandleEvent;
    pluginFuncs->urlnotify = NPP_URLNotify;
    pluginFuncs->getvalue = NPP_GetValue;
    pluginFuncs->setvalue = NPP_SetValue;
    
    //pluginFuncs->
    
    return NPERR_NO_ERROR;
}

/* Function called once by the browser to shut down the plugin. */
void NP_Shutdown(void)
{
    free(renderBuffer);
}

/* Class inherited from libndnrtc's observer interface, can be used to track the state of the app */
class obDerived : public INdnRtcLibraryObserver
{
public:
    void onStateChanged(const char *state, const char *args)
    {
        //printf("***DEBUG: %s - %s\n", state, args);
        return;
    }
};

/* Timer func is called every interval. In each timer func a paint event is fired. */
void refreshTimerFunc(NPP instance, uint32_t timerID)
{
    // invalidate rect sends a paint message
    // Paint message should always be fired in main thread, trying to fire this event in browserRenderer's renderRGBFrame will cause crash.
    browser->invalidaterect(instance, &(((PluginInstance *)instance->pdata)->window.clipRect));
    
    return;
}

/* Called to create a new instance of the plugin. */
NPError NPP_New(NPMIMEType pluginType, NPP instance, uint16_t mode, int16_t argc, char* argn[], char* argv[], NPSavedData* saved)
{
    PluginInstance *pluginInstance = (PluginInstance*)malloc(sizeof(PluginInstance));
    bzero(pluginInstance, sizeof(PluginInstance));
    
    pluginInstance->npp = instance;
    instance->pdata = pluginInstance;
    
    /* Select the Core Graphics drawing model. */
    NPBool supportsCoreGraphics = false;
    if (browser->getvalue(instance, NPNVsupportsCoreGraphicsBool, &supportsCoreGraphics) == NPERR_NO_ERROR && supportsCoreGraphics) {
        browser->setvalue(instance, NPPVpluginDrawingModel, (void*)NPDrawingModelCoreGraphics);
    } else {
        printf("CoreGraphics drawing model not supported, can't create a plugin instance.\n");
        return NPERR_INCOMPATIBLE_VERSION_ERROR;
    }
    
    /* Select the Cocoa event model. */
    NPBool supportsCocoaEvents = false;
    if (browser->getvalue(instance, NPNVsupportsCocoaBool, &supportsCocoaEvents) == NPERR_NO_ERROR && supportsCocoaEvents) {
        browser->setvalue(instance, NPPVpluginEventModel, (void*)NPEventModelCocoa);
    } else {
        printf("Cocoa event model not supported, can't create a plugin instance.\n");
        return NPERR_INCOMPATIBLE_VERSION_ERROR;
    }
    
    // Though it's loading from an absolute path, still, for some reasons, the lib in /usr/lib with the same name gets loaded first.
    // Notice: it still is trying to load from /usr/lib, am I linking against the wrong stuff? No unlikely, build phase passed, but still, firefox does not know where the library is...
    NdnRtcLibrary * libInstance = NdnRtcLibrary::instantiateLibraryObject("/Library/Internet Plug-Ins/BasicPlugin.bundle/Contents/Resources/libndnrtc-sa.dylib");
    
    obDerived * debugOb = new obDerived();
    
    libInstance->setObserver(debugOb);
    
    // Below ParamsStruct is a definition of default params
    ParamsStruct videoParams, audioParams;
    
    libInstance->getDefaultParams(videoParams, audioParams);
    
    printf("***\t***\n");
    printf("***BUILD NUMBER: %d\n", libInstance->getBuildNumber());
    
    videoParams.producerId = "zhehao";
    
    //videoParams.useFec = false;
    // So segment size differs from the default one.
    videoParams.segmentSize = 800;
    
    videoParams.ndnHub = "ndn/edu/ucla/remap";
    videoParams.loggingLevel = ndnlog::NdnLoggerDetailLevelNone;
    
    libInstance->configure(videoParams, audioParams);
    
    BrowserRenderer *bRenderer = new BrowserRenderer(pluginInstance);
    
    //libInstance->startPublishing("zhehao", bRenderer);
    libInstance->startFetching("remap-512", bRenderer);
    
    renderBuffer = (uint8_t *)malloc(videoParams.renderHeight * videoParams.renderHeight * 3);
    bzero(renderBuffer, videoParams.renderHeight * videoParams.renderHeight * 3);
    
    // schedule timer fires timer event every interval, in which paint event is fired
    browser->scheduletimer(pluginInstance->npp, 30, true, refreshTimerFunc);
    
    // The lib's alloc'ed in heap and is not freed
    return NPERR_NO_ERROR;
}

/* Called to destroy an instance of the plugin. */
NPError NPP_Destroy(NPP instance, NPSavedData** save)
{
    free(instance->pdata);
    
    return NPERR_NO_ERROR;
}

/* Called to update a plugin instances's NPWindow. */
NPError NPP_SetWindow(NPP instance, NPWindow* window)
{
    PluginInstance* currentInstance = (PluginInstance*)(instance->pdata);
    
    //window->type = NPWindowTypeDrawable;
    
    currentInstance->window = *window;
    
    return NPERR_NO_ERROR;
}

NPError NPP_NewStream(NPP instance, NPMIMEType type, NPStream* stream, NPBool seekable, uint16_t* stype)
{
    *stype = NP_ASFILEONLY;
    return NPERR_NO_ERROR;
}

NPError NPP_DestroyStream(NPP instance, NPStream* stream, NPReason reason)
{
    return NPERR_NO_ERROR;
}

int32_t NPP_WriteReady(NPP instance, NPStream* stream)
{
    return 0;
}

int32_t NPP_Write(NPP instance, NPStream* stream, int32_t offset, int32_t len, void* buffer)
{
    return 0;
}

void NPP_StreamAsFile(NPP instance, NPStream* stream, const char* fname)
{
}

void NPP_Print(NPP instance, NPPrint* platformPrint)
{
    
}

int16_t NPP_HandleEvent(NPP instance, void* event)
{
    NPCocoaEvent* cocoaEvent = (NPCocoaEvent*)event;
    if (cocoaEvent && (cocoaEvent->type == NPCocoaEventDrawRect)) {
        drawPlugin(instance, (NPCocoaEvent*)event);
        return 1;
    }
    
    return 0;
}

void NPP_URLNotify(NPP instance, const char* url, NPReason reason, void* notifyData)
{
    
}

// This function is called by NPP_GetValue, which is passed into the browser via function table.
NPObject* NPP_GetScriptableObject(NPP npp)
{
    NPObject *returnObj;
    if (scriptableObj != NULL)
    {
        returnObj = (NPObject *)scriptableObj;
    }
    else
    {
        // The point of npruntime.h is just to provide what the interface(between what and what) should look like?
        // As NPN_ScheduleTimer, the following npn functions are called from browser instance.
        //NPObject *newObj = NPN_CreateObject(instance, &(MyScriptableNPObject::_npclass));
        //NPN_RetainObject(newObj);
        
        scriptableObj = browser->createobject(npp, &(MyScriptableNPObject::_npclass));
    }
    browser->retainobject(scriptableObj);
    return returnObj;
}

// ScriptableObject is passed through GetValue, by specifying a certain NPVariable type.
NPError NPP_GetValue(NPP instance, NPPVariable variable, void *value)
{
    NPError rv = NPERR_NO_ERROR;
    switch(variable)
    {
        case NPPVpluginNameString:
            value = *((char **)value) = STRINGS_PRODUCTNAME;
            break;
        case NPPVpluginDescriptionString:    // Plugin description
            *((char **)value) = STRINGS_FILEDESCRIPTION;
            break;
        case NPPVpluginScriptableNPObject:
            // Scriptable plugin interface (for accessing from javascript)
            // this is a most interesting expression of getting scriptable object
            *(NPObject **)value = NPP_GetScriptableObject(instance);
            break;
        case NPPVpluginWindowBool:
            //*((PRBool *)value) = this->isWindowed;
            break;
        default:
            rv = NPERR_GENERIC_ERROR;
    }
    return rv;
}

NPError NPP_SetValue(NPP instance, NPNVariable variable, void *value)
{
    return NPERR_GENERIC_ERROR;
}

// this is a sample function that draws a frame with RGB array as input, using CoreGraphics
void drawDataInRect(CGContextRef context, CGRect frame, size_t width, size_t height)
{
    CGColorSpaceRef colorSpace = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);
    
    uint8_t * alphaBuffer = (uint8_t *)malloc(width * height * 4);
    
    // According to Quartz/AppleDevelopment documentation, the only byte format for rendering RGB bytes seems to take only 32bpp, so here Peter's 24bpp input is copied, and an empty alpha byte is added after each RGB byte.
    int i = 0, j = 0;
    while (i < width * height * 4)
    {
        alphaBuffer[i++] = renderBuffer[j++];
        alphaBuffer[i++] = renderBuffer[j++];
        alphaBuffer[i++] = renderBuffer[j++];
        
        // wonder why this gives a different result:
        
        //memcpy(alphaBuffer+i, renderBuffer+j, 3);
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

/* drawPlugin is actually called every interval, and handles the repaint according to renderBuffer. */
void drawPlugin(NPP instance, NPCocoaEvent* event)
{
    PluginInstance* currentInstance = (PluginInstance*)(instance->pdata);
    // Okay, so this works for Carbon event model, but not Cocoa event model
    //CGContextRef cgContext = ((NP_CGContext *)(((PluginInstance *)(instance->pdata))->window.window))->context;
    
    // This way of getting drawing context seems to be the only working way for Cocoa event model
    CGContextRef cgContext = event->data.draw.context;

    if (!cgContext) {
        return;
    }
    
    float windowWidth = currentInstance->window.width;
    float windowHeight = currentInstance->window.height;
    
    // Save the cgcontext gstate.
    CGContextSaveGState(cgContext);
    
    // before passing into bitmapContext, flip cgContext
    CGContextTranslateCTM(cgContext, 0.0, windowHeight);
    CGContextScaleCTM(cgContext, 1.0, -1.0);
    
    renderBufferLock.lock();
    drawDataInRect(cgContext, CGRectMake(0, 0, windowWidth, windowHeight), windowWidth, windowHeight);
    renderBufferLock.unlock();
    
    // Restore the cgcontext gstate.
    CGContextRestoreGState(cgContext);
}
