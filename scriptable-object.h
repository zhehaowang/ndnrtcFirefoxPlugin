//
//  ScriptableObject.h
//  BasicPlugin
//
//  Created by ZhehaoWang on 7/7/14.
//
//

#ifndef BasicPlugin_ScriptableObject_h
#define BasicPlugin_ScriptableObject_h

#include "npapiHeaders/npapi.h"
#include "npapiHeaders/npfunctions.h"

#include "basic-plugin.h"

#define PLUGIN_METHOD_NUM 9
#define PLUGIN_PROPERTY_NUM 8

#define MAX_STRING_LENGTH 100

extern NPIdentifier pluginMethods[PLUGIN_METHOD_NUM];
static const NPUTF8 *pluginMethodNames[PLUGIN_METHOD_NUM] = {
    "getVersion", // getVersion is the debug call
    "startPublishing",
    "stopPublishing",
    "startFetching",
    "stopFetching",
    "joinChat",
    "leaveChat",
    "startSpeakerDiscovery",
    "stopSpeakerDiscovery"
};

enum {
    ID_GET_VERSION = 0,
    ID_START_PUBLISHING,
    ID_STOP_PUBLISHING,
    ID_START_FETCHING,
    ID_STOP_FETCHING,
    ID_JOIN_CHAT,
    ID_LEAVE_CHAT,
    ID_START_SPEAKER_DISCOVERY,
    ID_STOP_SPEAKER_DISCOVERY
};

extern NPIdentifier pluginProperties[PLUGIN_PROPERTY_NUM];
static const NPUTF8 *pluginPropertyNames[PLUGIN_PROPERTY_NUM] = {
    "version", // version is the debug property
    "publishingNum", // isPublishing returns if the plugin's publishing
    "fetchingNum",
    "renderWindowNum",
    "defaultWindowWidth",
    "defaultWindowHeight",
    "inChat",
    "inDiscovery"
};

enum {
    ID_VERSION = 0,
    ID_PUBLISHING_NUM,
    ID_FETCHING_NUM,
    ID_RENDER_WINDOW_NUM,
    ID_DEFAULT_WINDOW_WIDTH,
    ID_DEFAULT_WINDOW_HEIGHT,
    ID_IN_CHAT,
    ID_IN_DISCOVERY
};

extern char *versionStr;

// Scriptable object example used for callback from Javascript
class MyScriptableNPObject : public NPObject
{
protected:
    // Class member functions that do the real work
    void Deallocate();
    void Invalidate();
    
    bool HasMethod(NPIdentifier name);
    
    bool Invoke(NPIdentifier name, const NPVariant *args, uint32_t argCount, NPVariant *result);
    bool InvokeDefault(const NPVariant *args, uint32_t argCount, NPVariant *result);
    
    bool HasProperty(NPIdentifier name);
    bool GetProperty(NPIdentifier name, NPVariant *result);
    bool SetProperty(NPIdentifier name, const NPVariant *value);
    
    bool RemoveProperty(NPIdentifier name);
    bool Enumerate(NPIdentifier **identifier, uint32_t *count);
    bool Construct(const NPVariant *args, uint32_t argCount, NPVariant *result);
    
    bool debugWindowPrint();
    
public:
    MyScriptableNPObject(NPP instance);
    
    // This is the method used to create the NPObject
    // This method should not be called explicitly
    // Instead, use NPN_CreateObject
    static NPObject* Allocate(NPP npp, NPClass *aClass) {
        return (NPObject *)new MyScriptableNPObject(npp);
    }

    /////////////////////////////
    // Static NPObject methods //
    /////////////////////////////
    static void _Deallocate(NPObject *npobj);
    static void _Invalidate(NPObject *npobj);
    static bool _HasMethod(NPObject *npobj, NPIdentifier name);
    static bool _Invoke(NPObject *npobj, NPIdentifier name, const NPVariant *args, uint32_t argCount, NPVariant *result);
    static bool _InvokeDefault(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result);
    static bool _HasProperty(NPObject * npobj, NPIdentifier name);
    static bool _GetProperty(NPObject *npobj, NPIdentifier name, NPVariant *result);
    static bool _SetProperty(NPObject *npobj, NPIdentifier name, const NPVariant *value);
    static bool _RemoveProperty(NPObject *npobj, NPIdentifier name);
    static bool _Enumerate(NPObject *npobj, NPIdentifier **identifier, uint32_t *count);
    static bool _Construct(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result);
    
    static MyScriptableNPObject* NewObject(NPP npp);
    
    static NPClass _npclass;
    //static NPIdentifier getVersionId_;
protected:
    NPP instance_;
};


#endif
