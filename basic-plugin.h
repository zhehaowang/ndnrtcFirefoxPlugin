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

#ifndef __BasicPlugin__
#define __BasicPlugin__

// This just needs to include NPAPI headers, change the path to whatever works
// for you. Note that "XP_MACOSX=1" is defined in the project so that the NPAPI
// headers know we're compiling for Mac OS X.
#include "npapiHeaders/npapi.h"
#include "npapiHeaders/npfunctions.h"

#include "ndnrtcHeader/ndnrtc-library.h"

#include "scriptable-object.h"
#include "render-window.h"
#include "browser-renderer.h"
#include "linked-list.h"
#include "render-model.h"

#include <mutex>

#define STRINGS_PRODUCTNAME "ndnrtc-plugin"
#define STRINGS_FILEDESCRIPTION "Test description"

#define PLUGIN_VERSION "v0.1"
#define MAX_CLIENTS 10

/* According to MDN's reference https://developer.mozilla.org/en-US/Add-ons/Plugins/Writing_a_plugin_for_Mac_OS_X
 * these 3 functions should be made visible to the caller in a C style, as signified by the extern statement below */

/*
 NPError NP_Initialize(NPNetscapeFuncs *browserFuncs);
 NPError NP_GetEntryPoints(NPPluginFuncs *pluginFuncs);
 void    NP_Shutdown(void);
 */
/* Data for each instance of this plugin. */
typedef struct PluginInstance {
    NPP npp;
    NPWindow window;
}PluginInstance;

/* Structure containing pointers to functions implemented by the browser. */
extern NPNetscapeFuncs* browser;

// interesting that without static it does not work; with static it's semantically incorrect; the semantically correct way is below.
extern RenderWindow renderWindows[MAX_CLIENTS];
extern int renderWindowNum;

extern std::mutex renderBufferLock;
extern ndnrtc::NdnRtcLibrary * libInstance;

extern int publishingNum;
extern int fetchingNum;

const size_t defaultWindowWidth = 640;
const size_t defaultWindowHeight = 480;

extern bool inChat;
extern bool inDiscovery;

#pragma GCC visibility push(default)
extern "C" {
    NPError NP_Initialize(NPNetscapeFuncs *browserFuncs);
    NPError NP_GetEntryPoints(NPPluginFuncs *pluginFuncs);
    void NP_Shutdown(void);
}
#pragma GCC visibility pop


NPError NPP_New(NPMIMEType pluginType, NPP instance, uint16_t mode, int16_t argc, char* argn[], char* argv[], NPSavedData* saved);
NPError NPP_Destroy(NPP instance, NPSavedData** save);
NPError NPP_SetWindow(NPP instance, NPWindow* window);
NPError NPP_NewStream(NPP instance, NPMIMEType type, NPStream* stream, NPBool seekable, uint16_t* stype);
NPError NPP_DestroyStream(NPP instance, NPStream* stream, NPReason reason);
int32_t NPP_WriteReady(NPP instance, NPStream* stream);
int32_t NPP_Write(NPP instance, NPStream* stream, int32_t offset, int32_t len, void* buffer);
void    NPP_StreamAsFile(NPP instance, NPStream* stream, const char* fname);
void    NPP_Print(NPP instance, NPPrint* platformPrint);
int16_t NPP_HandleEvent(NPP instance, void* event);
void    NPP_URLNotify(NPP instance, const char* URL, NPReason reason, void* notifyData);
NPError NPP_GetValue(NPP instance, NPPVariable variable, void *value);
NPError NPP_SetValue(NPP instance, NPNVariable variable, void *value);

#endif

