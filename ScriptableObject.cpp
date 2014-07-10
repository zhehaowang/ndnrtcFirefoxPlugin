//
//  ScriptableObject.cpp
//  BasicPlugin
//
//  Created by ZhehaoWang on 7/7/14.
//
//

#include "ScriptableObject.h"
#include "BasicPlugin.h"

#define PLUGIN_VERSION "v0.1"

NPIdentifier pluginMethods[PLUGIN_METHOD_NUM];
NPIdentifier pluginProperties[PLUGIN_PROPERTY_NUM];

MyScriptableNPObject::MyScriptableNPObject(NPP instance)
{
    instance_ = instance;
}

// static
void MyScriptableNPObject::_Invalidate(NPObject *obj) {
    ((MyScriptableNPObject*)obj)->Invalidate();
}
void MyScriptableNPObject::Invalidate() {
    // Invalidate the control
}

// static
void MyScriptableNPObject::_Deallocate(NPObject *obj) {
    ((MyScriptableNPObject*)obj)->Deallocate();
    delete ((MyScriptableNPObject*)obj);
}
void MyScriptableNPObject::Deallocate() {
    // Do any cleanup needed
}

// static
bool MyScriptableNPObject::_HasMethod(NPObject *npobj, NPIdentifier name)
{
    return ((MyScriptableNPObject*)npobj)->HasMethod(name);
}
bool MyScriptableNPObject::HasMethod(NPIdentifier name)
{
    // getstringidentifier crashes, could it because that cross-thread reference of getstringidentifier is not allowed?
    // Figure out: what is actually passed to the browser. The npobject, or the wrapper class; if it's the former...the browser still can refer to the wrapper class via the class's actual implementation instead of the static skeletons; and those implementation has access to the wrapper class's members.
    
    //NPIdentifier debugStr[6] = {'c', 'r', 'a', 's', 'h'};
    
    // calling getstringidentifier directly results in the crash
    
    //char debugStr[6] = "crash";
    //browser->getstringidentifier(debugStr);
    //NPN_GetStringIdentifier("crash");
    
    printf("*** HasMethod function called. ***\n");
    
    // browser->UTF8FromIdentifier and browser->GetStringIdentifier does not work for this thread?
    // crashes at NPN_UTF8FromIdentifier, storing it at the initializatong phase also crashes...
    
    //printf("*** Name it's trying to lookup : %s. ***\n", NPN_UTF8FromIdentifier(name));
    int i = 0;
    for (i = 0; i < PLUGIN_METHOD_NUM; i++)
    {
        if (name == pluginMethods[i])
        {
            return true;
        }
    }
    return false;
}

// static
bool MyScriptableNPObject::_Invoke(NPObject *npobj, NPIdentifier name, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
    return ((MyScriptableNPObject*)npobj)->Invoke(name, args, argCount, result); 
}
bool MyScriptableNPObject::Invoke(NPIdentifier name, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
    bool rc = false;
    char* wptr = NULL;
    printf("*** Invoke function called. ***\n");
    
    if  (name == pluginMethods[ID_GET_VERSION])
    {
        printf("*** Tring to print version. ***\n");
        wptr = (NPUTF8*)(browser->memalloc(strlen(PLUGIN_VERSION) + 1)); //Should be freed by browser
        if  (wptr != NULL)
        {
            rc = true;
            memset(wptr, 0x00, strlen(PLUGIN_VERSION) + 1);
            memcpy(wptr, PLUGIN_VERSION, strlen(PLUGIN_VERSION));
            STRINGZ_TO_NPVARIANT(wptr, *result);
        }
    }
    return (rc);
}

//static
bool MyScriptableNPObject::_InvokeDefault(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
    ((MyScriptableNPObject*)npobj)->InvokeDefault(args, argCount, result);
    return true;
}
bool MyScriptableNPObject::InvokeDefault(const NPVariant *args, uint32_t argCount, NPVariant *result)
{
    printf("\n*** Method invoked by InvokeDefault. ***\n");
    return true;
}

// static
bool MyScriptableNPObject::_HasProperty(NPObject *npobj, NPIdentifier name)
{
    ((MyScriptableNPObject*)npobj)->HasProperty(name);
    return true;
}
bool MyScriptableNPObject::HasProperty(NPIdentifier name)
{
    return true;
}

// static
bool MyScriptableNPObject::_GetProperty(NPObject *npobj, NPIdentifier name, NPVariant *result)
{
    ((MyScriptableNPObject*)npobj)->GetProperty(name, result);
    return true;
}
bool MyScriptableNPObject::GetProperty(NPIdentifier name, NPVariant *result)
{
    return true;
}

// static
bool MyScriptableNPObject::_SetProperty(NPObject *npobj, NPIdentifier name, const NPVariant *value)
{
    ((MyScriptableNPObject*)npobj)->SetProperty(name, value);
    return true;
}
bool MyScriptableNPObject::SetProperty(NPIdentifier name, const NPVariant *value)
{
    return true;
}

// static
bool MyScriptableNPObject::_RemoveProperty(NPObject *npobj, NPIdentifier name)
{
    ((MyScriptableNPObject*)npobj)->RemoveProperty(name);
    return true;
}
bool MyScriptableNPObject::RemoveProperty(NPIdentifier name)
{
    return true;
}

// static
bool MyScriptableNPObject::_Enumerate(NPObject *npobj, NPIdentifier **identifier, uint32_t *count)
{
    ((MyScriptableNPObject*)npobj)->Enumerate(identifier, count);
    return true;
}
bool MyScriptableNPObject::Enumerate(NPIdentifier **identifier, uint32_t *count)
{
    return true;
}

// static
bool MyScriptableNPObject::_Construct(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
    ((MyScriptableNPObject*)npobj)->Construct(args, argCount, result);
    return true;
}
bool MyScriptableNPObject::Construct(const NPVariant *args, uint32_t argCount, NPVariant *result)
{
    return true;
}

// not sure if it's the best practice to put initilization here.
// this is an example of how a static struct class member should be initialized...It's not so intuitive to me...should double check
NPClass MyScriptableNPObject::_npclass = {
    NP_CLASS_STRUCT_VERSION,
    MyScriptableNPObject::Allocate,
    MyScriptableNPObject::_Deallocate,
    MyScriptableNPObject::_Invalidate,
    MyScriptableNPObject::_HasMethod,
    MyScriptableNPObject::_Invoke,
    MyScriptableNPObject::_InvokeDefault,
    MyScriptableNPObject::_HasProperty,
    MyScriptableNPObject::_GetProperty,
    MyScriptableNPObject::_SetProperty,
    MyScriptableNPObject::_RemoveProperty,
    MyScriptableNPObject::_Enumerate,
    MyScriptableNPObject::_Construct
};

MyScriptableNPObject* MyScriptableNPObject::NewObject(NPP npp) {
    MyScriptableNPObject* newObj = (MyScriptableNPObject*)(browser->createobject(npp, &_npclass));
    return newObj;
}
