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

// Issue: refresh page crashes the plugin, has to refresh again, crash happened once with logs in log directory.
// Issue: Consumer delay too large
// Issue: Configure dylib load path, so that libndnrtc.dylib does not need to be in system paths like "/usr/local/lib"

// Git commit rename file and linking issues;

#include "basic-plugin.h"

#define PATH_LENGTH 200

// renderBuffer and window area should belong to each render window, which can be represented by a renderWindow class.
// renderWindows are allocated in an array, deletion of a certain window will cause all windows after it to move forward
RenderWindow renderWindows[MAX_CLIENTS];
int renderWindowNum = 0;

std::mutex renderBufferLock;

NPNetscapeFuncs* browser;

NdnRtcLibrary * libInstance;

RenderModel *rModel;

// The npobject to pass into the browser. Made global so that only one copy exists at a time.
NPObject *scriptableObj;

// publishingNum is the window number of the publisher; -1 signifies not publishing.
int publishingNum = -1;

// fetchNum is the total number of running consumers.
int fetchingNum = 0;

// chat is off by default, and discovery is on by default
bool inChat = false;
bool inDiscovery = false;

/* Symbol called once by the browser to initialize the plugin. */
NPError NP_Initialize(NPNetscapeFuncs* browserFuncs)
{
    /* Save the browser function table. The ones with NPN_ are browser functions, handles the event usually fired from browser.
     Headers included in this plugin project contain the definitions for (pointers to) NPN function prototypes. The actual functions implementatioins are passed in from this initialization function.
     */
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
    
    return NPERR_NO_ERROR;
}

/* Function called once by the browser to shut down the plugin. */
void NP_Shutdown(void)
{
    free(renderWindows);
}

/* Class inherited from libndnrtc's observer interface, can be used to track the state of the app */
class obDerived : public INdnRtcLibraryObserver
{
public:
    void onStateChanged(const char *state, const char *args)
    {
        printf("***DEBUG: %s - %s\n", state, args);
        return;
    }
};

static void initializeIdentifiers()
{
    //pluginMethods[0] = browser->getstringidentifier(pluginMethodNames[0]);
}

static void setNdnrtcDefaultConfiguration()
{
    // Below ParamsStruct is a definition of default params
    ParamsStruct videoParams, audioParams;
    
    libInstance->getDefaultParams(videoParams, audioParams);
    
    CodecParams videoStreamCodec;
    videoStreamCodec.idx = 0;
    videoStreamCodec.codecFrameRate = 30;
    videoStreamCodec.gop = 30;
    
    videoStreamCodec.startBitrate = 500;
    videoStreamCodec.encodeWidth = 640;
    videoStreamCodec.encodeHeight = 480;
    
    videoStreamCodec.maxBitrate = 0;
    
    CodecParams audioStreamCodec;
    audioStreamCodec.startBitrate = 90;
    
    videoParams.addNewStream(videoStreamCodec);
    
    audioParams.addNewStream(audioStreamCodec);
    
    videoParams.producerId = "zhehao";
    
    videoParams.useFec = false;
    videoParams.segmentSize = 1000;
    
    videoParams.ndnHub = "ndn/edu/ucla/remap";
    videoParams.loggingLevel = ndnlog::NdnLoggerDetailLevelAll;
    videoParams.host = "localhost";
    
    libInstance->configure(videoParams, audioParams);
}

static bool getPluginLibraryPath(char * libPath)
{
    ::Dl_info dlinfo;
    if (::dladdr((void*)::NP_Initialize, &dlinfo) != 0) {
        // Library path configuration; while loop used for neglecting last name component.
        std::string pluginPath = dlinfo.dli_fname;
        
        memset(libPath, 0, PATH_LENGTH);
        
        strcat(libPath, pluginPath.c_str());
        int i = strlen(libPath);
        while (libPath[i] != '/')
        {
            libPath[i] = 0;
            i--;
        }
        strcat(libPath, "libndnrtc.dylib");
        printf("%s\n", libPath);
        
        return true;
    } else {
        return false;
    }
}

/* Called to create a new instance of the plugin. */
NPError NPP_New(NPMIMEType pluginType, NPP instance, uint16_t mode, int16_t argc, char* argn[], char* argv[], NPSavedData* saved)
{
    PluginInstance *pluginInstance = (PluginInstance*)malloc(sizeof(PluginInstance));
    bzero(pluginInstance, sizeof(PluginInstance));
    
    pluginInstance->npp = instance;
    instance->pdata = pluginInstance;
    
    /* Initialize identifiers */
    
    browser->getstringidentifiers(pluginMethodNames, PLUGIN_METHOD_NUM, pluginMethods);
    browser->getstringidentifiers(pluginPropertyNames, PLUGIN_PROPERTY_NUM, pluginProperties);
    
    /* Browser memory allocation */
    versionStr = (NPUTF8*)(browser->memalloc(strlen(PLUGIN_VERSION) + 1));
    
    /* Select the Core Graphics drawing model. */
    rModel = new RenderModelCG();
    rModel->selectRenderModel(instance, browser);
    
    /* Select the Cocoa event model. */
    
    NPBool supportsCocoaEvents = false;
    if (browser->getvalue(instance, NPNVsupportsCocoaBool, &supportsCocoaEvents) == NPERR_NO_ERROR && supportsCocoaEvents) {
        browser->setvalue(instance, NPPVpluginEventModel, (void*)NPEventModelCocoa);
    } else {
        printf("Cocoa event model not supported, can't create a plugin instance.\n");
        return NPERR_INCOMPATIBLE_VERSION_ERROR;
    }
    
    // Though it's loading from an absolute path, still, for some reasons, the lib in /usr/lib with the same name gets loaded first, and usually, the library specified does not matter at all.
    
    printf("trying to get library path\n");
    
    
    char libPath[PATH_LENGTH] = "";
    
    getPluginLibraryPath(libPath);
    
    libInstance = NdnRtcLibrary::instantiateLibraryObject(libPath);
    
    obDerived * debugOb = new obDerived();
    
    libInstance->setObserver(debugOb);
    
    setNdnrtcDefaultConfiguration();
    
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
        rModel->drawPlugin(instance, (NPCocoaEvent*)event);
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
    if (scriptableObj == NULL)
    {
        // The point of npruntime.h is just to provide what the interface(between what and what) should look like?
        // As NPN_ScheduleTimer, the following npn functions are called from browser instance.
        //NPObject *newObj = NPN_CreateObject(instance, &(MyScriptableNPObject::_npclass));
        //NPN_RetainObject(newObj);
        
        scriptableObj = browser->createobject(npp, &(MyScriptableNPObject::_npclass));
    }
    browser->retainobject(scriptableObj);
    return scriptableObj;
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
            // The calling model for this function is like what?
            
            *((NPObject **)value) = NPP_GetScriptableObject(instance);
            printf("*** Trying to get a scriptable object. ***\n");
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
