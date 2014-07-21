//
//  ScriptableObject.cpp
//  BasicPlugin
//
//  Created by ZhehaoWang on 7/7/14.
//
//

#include "ScriptableObject.h"
#include "BasicPlugin.h"

#include "BrowserRenderer.h"

using namespace ndnrtc;

NPIdentifier pluginMethods[PLUGIN_METHOD_NUM];
NPIdentifier pluginProperties[PLUGIN_PROPERTY_NUM];

char * versionStr = NULL;

/* Timer func is called every interval. In each timer func a paint event is fired. */
void refreshTimerFunc(NPP instance, uint32_t timerID)
{
    // invalidate rect sends a paint message
    browser->invalidaterect(instance, &(((PluginInstance *)instance->pdata)->window.clipRect));
    
    return;
}

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
    printf("*** Invoke function called. ***\n");
    
    if  (name == pluginMethods[ID_GET_VERSION])
    {
        printf("*** Tring to print version. ***\n");
        char * returnStr = (NPUTF8*)(browser->memalloc(strlen(PLUGIN_VERSION) + 1));
        // versionStr is allocated in main thread, globally
        if  (returnStr != NULL)
        {
            rc = true;
            memset(returnStr, 0x00, strlen(PLUGIN_VERSION) + 1);
            memcpy(returnStr, PLUGIN_VERSION, strlen(PLUGIN_VERSION));
            STRINGZ_TO_NPVARIANT(returnStr, *result);
        }
    }
    if (name == pluginMethods[ID_START_FETCHING])
    {
        // Issue: startFetch takes too long (usually it is still working), if publisher is started before consumer.
        // Reason: For some reasons, 'catching up' process took far too long; I can see a lot of ints and data exchange in local status.
        // Comparisons: Tested with external renderer and default renderer, both yields the same result. The test-app, however, takes a much shorter 'delay' time; I didn't remember encountering such problems before, with scriptableObject not yet implemented and fetching from remap-512 (which seems to be off?)
        // Solving: No idea yet; need to trace the problem further
        
        if (argCount == 2)
        {
            NPString fetcherName = NPVARIANT_TO_STRING(args[0]);
            NPString fetcherPrefix = NPVARIANT_TO_STRING(args[1]);
            
            int i = 0;
            while ((strcmp(fetcherName.UTF8Characters, renderWindows[i].windowName_) || i==publishingNum) && (i<MAX_CLIENTS))
            {
                i++;
            }
            if (i == MAX_CLIENTS)
            {
                printf("startFetch: Trying to start fetching from name '%s', prefix '%s'.\n", fetcherName.UTF8Characters, fetcherPrefix.UTF8Characters);
                
                BrowserRenderer *bRenderer = new BrowserRenderer();
                
                ParamsStruct videoParams, audioParams;
                
                libInstance->currentParams(videoParams, audioParams);
                videoParams.producerId = fetcherName.UTF8Characters;
                videoParams.ndnHub = fetcherPrefix.UTF8Characters;
                
                libInstance->configure(videoParams, audioParams);
                
                bRenderer->bufferIndex_ = renderWindowNum;
                
                // Still need to walk through the differences here; renderBuffer_ is only a pointer, supposedly
                //renderWindows[renderBufferCount].renderBuffer_ = (uint8_t *)malloc(videoParams.renderHeight * videoParams.renderHeight * 3);
                
                renderWindows[renderWindowNum].renderBuffer_ = NULL;
                //bzero(renderWindows[renderBufferCount].renderBuffer_, videoParams.renderHeight * videoParams.renderHeight * 3);
                
                renderWindows[renderWindowNum].setBottom(0);
                renderWindows[renderWindowNum].setLeft(defaultWindowWidth * renderWindowNum);
                renderWindows[renderWindowNum].generateRect();
                
                renderWindows[renderWindowNum].bRenderer_ = bRenderer;
                
                strcpy(renderWindows[renderWindowNum].windowName_, fetcherName.UTF8Characters);
                
                libInstance->startFetching(fetcherName.UTF8Characters, bRenderer);
                
                // schedule timer fires timer event every interval, in which paint event is fired
                if (publishingNum == -1 && fetchingNum == 0)
                {
                    browser->scheduletimer(instance_, 30, true, refreshTimerFunc);
                }
                
                renderWindowNum++;
                fetchingNum ++;
                
                BOOLEAN_TO_NPVARIANT(true, *result);
            }
            else
            {
                BOOLEAN_TO_NPVARIANT(false, *result);
            }
            rc = true;
        }
        else
        {
            printf("startFetch: Wrong number of arguments.\n");
        }
    }
    if (name == pluginMethods[ID_START_PUBLISHING])
    {
        if (argCount == 2)
        {
            NPString publisherName = NPVARIANT_TO_STRING(args[0]);
            NPString publisherPrefix = NPVARIANT_TO_STRING(args[1]);
            
            printf("startPublish: Trying to start publishing with name '%s', prefix '%s'.\n", publisherName.UTF8Characters, publisherPrefix.UTF8Characters);
            
            BrowserRenderer *bRenderer = new BrowserRenderer();
            
            ParamsStruct videoParams, audioParams;
            
            libInstance->currentParams(videoParams, audioParams);
            //videoParams.producerId = publisherName.UTF8Characters;
            
            bRenderer->bufferIndex_ = renderWindowNum;
            
            renderWindows[renderWindowNum].renderBuffer_ = NULL;
            //(uint8_t *)malloc(videoParams.renderHeight * videoParams.renderHeight * 3);
            //bzero(renderWindows[renderBufferCount].renderBuffer_, videoParams.renderHeight * videoParams.renderHeight * 3);
            
            renderWindows[renderWindowNum].setBottom(0);
            renderWindows[renderWindowNum].setLeft(defaultWindowWidth * renderWindowNum);
            renderWindows[renderWindowNum].generateRect();
            
            renderWindows[renderWindowNum].bRenderer_ = bRenderer;
            
            strcpy(renderWindows[renderWindowNum].windowName_, publisherName.UTF8Characters);
            
            libInstance->startPublishing(publisherName.UTF8Characters, bRenderer);
            
            // schedule timer fires timer event every interval, in which paint event is fired
            
            if (publishingNum == -1 && fetchingNum == 0)
            {
                browser->scheduletimer(instance_, 30, true, refreshTimerFunc);
            }
            
            publishingNum = renderWindowNum;
            renderWindowNum++;
            rc = true;
            
            BOOLEAN_TO_NPVARIANT(true, *result);
        }
        else
        {
            printf("startPublish: Wrong number of arguments.\n");
            BOOLEAN_TO_NPVARIANT(false, *result);
        }
    }
    // stop fetching and stop publishing does not handle rearranging windows and stopping timer event
    if (name == pluginMethods[ID_STOP_FETCHING])
    {
        if (argCount == 2)
        {
            NPString fetcherName = NPVARIANT_TO_STRING(args[0]);
            NPString fetcherPrefix = NPVARIANT_TO_STRING(args[1]);
            
            int i = 0;
            while ((strcmp(fetcherName.UTF8Characters, renderWindows[i].windowName_) || i==publishingNum) && (i<MAX_CLIENTS))
            {
                i++;
            }
            if (i < MAX_CLIENTS)
            {
                libInstance->stopFetching(fetcherName.UTF8Characters);
            
                i++;
                
                while (i < MAX_CLIENTS && renderWindows[i].bRenderer_ != NULL)
                {
                    renderWindows[i].setLeft(renderWindows[i].getLeft() - defaultWindowWidth);
                    renderWindows[i].generateRect();
                    
                    ((BrowserRenderer *)renderWindows[i].bRenderer_)->bufferIndex_ = i - 1;
                    
                    i++;
                }
                
                renderWindowNum --;
                fetchingNum --;
                
                BOOLEAN_TO_NPVARIANT(true, *result);
                // unregister timer event if needed
            }
            else
            {
                BOOLEAN_TO_NPVARIANT(false, *result);
            }
            rc = true;
        }
        else
        {
            printf("stopFetch: Wrong number of arguments.\n");
            BOOLEAN_TO_NPVARIANT(false, *result);
        }
    }
    if (name == pluginMethods[ID_STOP_PUBLISHING])
    {
        if (argCount == 0)
        {
            libInstance->stopPublishing();
            
            int i = publishingNum + 1;
            
            while (i < MAX_CLIENTS && renderWindows[i].bRenderer_ != NULL)
            {
                renderWindows[i].setLeft(renderWindows[i].getLeft() - defaultWindowWidth);
                renderWindows[i].generateRect();
                
                ((BrowserRenderer *)renderWindows[i].bRenderer_)->bufferIndex_ = i - 1;
                
                i++;
            }
            
            // Free the 'corresponding' renderWindow as well? Decrement renderWindowNum. And if there are no more renderWindows,
            // Stop sending paint msgs; Something could go wrong with decrement process?
            
            renderWindowNum --;
            publishingNum = -1;
            
            rc = true;
            
            BOOLEAN_TO_NPVARIANT(true, *result);
            // unregister time event if needed.
        }
        else
        {
            printf("stopPublish: Wrong number of arguments");
            BOOLEAN_TO_NPVARIANT(false, *result);
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
    printf("*** HasProperty function called. ***\n");
    
    int i = 0;
    for (i = 0; i < PLUGIN_PROPERTY_NUM; i++)
    {
        if (name == pluginProperties[i])
        {
            return true;
        }
    }
    return false;
}

// static
bool MyScriptableNPObject::_GetProperty(NPObject *npobj, NPIdentifier name, NPVariant *result)
{
    ((MyScriptableNPObject*)npobj)->GetProperty(name, result);
    return true;
}
bool MyScriptableNPObject::GetProperty(NPIdentifier name, NPVariant *result)
{
    bool rc = false;
    printf("*** GetProperty called. ***\n");
    
    if  (name == pluginProperties[ID_VERSION])
    {
        char * returnStr = (NPUTF8*)(browser->memalloc(strlen(PLUGIN_VERSION) + 1));
        // versionStr is allocated in main thread, globally
        if  (returnStr != NULL)
        {
            rc = true;
            memset(returnStr, 0x00, strlen(PLUGIN_VERSION) + 1);
            memcpy(returnStr, PLUGIN_VERSION, strlen(PLUGIN_VERSION));
            STRINGZ_TO_NPVARIANT(returnStr, *result);
        }
    }
    if (name == pluginProperties[ID_PUBLISHING_NUM])
    {
        //printf("Property isPublishing exists");
        INT32_TO_NPVARIANT(publishingNum, *result);
        rc = true;
    }
    if (name == pluginProperties[ID_FETCHING_NUM])
    {
        INT32_TO_NPVARIANT(fetchingNum, *result);
        rc = true;
    }
    // renderWindowNum should always be equal with fetchingNum (isPublishing ? 1 : 0)
    if (name == pluginProperties[ID_RENDER_WINDOW_NUM])
    {
        INT32_TO_NPVARIANT(renderWindowNum, *result);
        rc = true;
    }
    // Here it is assumed that type size_t can be casted to NP_Variant as an INT32 input
    if (name == pluginProperties[ID_DEFAULT_WINDOW_WIDTH])
    {
        INT32_TO_NPVARIANT(defaultWindowWidth, *result);
        rc = true;
    }
    if (name == pluginProperties[ID_DEFAULT_WINDOW_HEIGHT])
    {
        INT32_TO_NPVARIANT(defaultWindowHeight, *result);
        rc = true;
    }
    
    return rc;
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
