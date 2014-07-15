//
//  renderer-stub.h
//  ndnrtc
//
//  Created by Peter Gusev on 6/26/14.
//  Copyright (c) 2014 Peter Gusev. All rights reserved.
//

#ifndef __ndnrtc__renderer_stub__
#define __ndnrtc__renderer_stub__

#include <iostream>
#include "ndnrtc-library.h"
#include "BasicPlugin.h"

#include "external-renderer.h"

using namespace ndnrtc;

class BrowserRenderer : public ndnrtc::IExternalRenderer
{
public:
    BrowserRenderer()
    {
        //pInstance_ = NULL;
        bufferSize_ = 0;
        buffer_ = NULL;
        //printf("Debug: Browser renderer created.\n");
    };
    ~BrowserRenderer(){};
    
    BrowserRenderer(PluginInstance *pInstance)
    {
        //pInstance_ = pInstance;
        bufferSize_ = 0;
        buffer_ = NULL;
        //printf("Debug: Browser renderer created witbh parameter.\n");
    };
    
    uint8_t* getFrameBuffer(int width, int height);
    void renderRGBFrame(int64_t timestamp, int width, int height,
                        const uint8_t* buffer);
    
    int debugFunction(int a, int b)
    {
        printf("Debug: debug function called.\n");
        return a+b;
    };
private:
    int bufferSize_;
    uint8_t *buffer_;
    
    //PluginInstance *pInstance_;
};

#endif /* defined(__ndnrtc__renderer_stub__) */
