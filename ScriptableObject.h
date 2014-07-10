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

#include "BasicPlugin.h"

#define PLUGIN_METHOD_NUM 1
#define PLUGIN_PROPERTY_NUM 1

extern NPIdentifier pluginMethods[PLUGIN_METHOD_NUM];
static const NPUTF8 *pluginMethodNames[PLUGIN_METHOD_NUM] = {
    "getVersion"
};

enum {
    ID_GET_VERSION = 0
};

extern NPIdentifier pluginProperties[PLUGIN_PROPERTY_NUM];
static const NPUTF8 *pluginPropertyNames[PLUGIN_PROPERTY_NUM] = {
    "version"
};

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
