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
#include "renderWindow.h"

using namespace ndnrtc;

class BrowserRenderer : public ndnrtc::IExternalRenderer
{
public:
    BrowserRenderer()
    {
        //pInstance_ = NULL;
        bufferSize_ = 0;
        // buffer and renderBuffer are allocated when startPublish or startFetch is called.
        buffer_ = NULL;
    };
    ~BrowserRenderer()
    {
        free(buffer_);
    };
    
    uint8_t* getFrameBuffer(int width, int height);
    
    void renderRGBFrame(int64_t timestamp, int width, int height,
                        const uint8_t* buffer);
    
    int bufferIndex_;
    
    uint8_t *getBuffer();
    bool setBuffer(uint8_t * buffer);
private:
    int bufferSize_;
    
    uint8_t *buffer_;
    //PluginInstance *pInstance_;
};

#endif /* defined(__ndnrtc__renderer_stub__) */
