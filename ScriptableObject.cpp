//
//  ScriptableObject.cpp
//  BasicPlugin
//
//  Created by ZhehaoWang on 7/7/14.
//
//

#include "ScriptableObject.h"

MyScriptableNPObject::MyScriptableNPObject(NPP instance)
{
    instance_ = instance;
    
    // not sure if it's the best practice to put initilization here.
    NPClass _npclass = {
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
    return true;
}

// static
bool MyScriptableNPObject::_Invoke(NPObject *npobj, NPIdentifier name, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
    return true;
}

//static
bool MyScriptableNPObject::_InvokeDefault(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
    return true;
}

// static
bool MyScriptableNPObject::_HasProperty(NPObject *npobj, NPIdentifier name)
{
    return true;
}

// static
bool MyScriptableNPObject::_GetProperty(NPObject *npobj, NPIdentifier name, NPVariant *result)
{
    return true;
}

// static
bool MyScriptableNPObject::_SetProperty(NPObject *npobj, NPIdentifier name, const NPVariant *value)
{
    return true;
}

// static
bool MyScriptableNPObject::_RemoveProperty(NPObject *npobj, NPIdentifier name)
{
    return true;
}

// static
bool MyScriptableNPObject::_Enumerate(NPObject *npobj, NPIdentifier **identifier, uint32_t *count)
{
    return true;
}

// static
bool MyScriptableNPObject::_Construct(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
    return true;
}
