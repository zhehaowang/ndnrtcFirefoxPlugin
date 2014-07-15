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
        // buffer and renderBuffer are allocated when startPublish or startFetch is called.
        buffer_ = NULL;
        renderBuffer_ = NULL;
        
    };
    ~BrowserRenderer()
    {
        free(buffer_);
        free(renderBuffer_);
    };
    
    uint8_t* getFrameBuffer(int width, int height);
    
    void renderRGBFrame(int64_t timestamp, int width, int height,
                        const uint8_t* buffer);
    
    int getTop();
    int getLeft();
    
    size_t getWidth();
    size_t getHeight();
    
    bool setTop(int top);
    bool setLeft(int left);
    
    bool setWidth(size_t width);
    bool setHeight(size_t height);
    
    uint8_t *renderBuffer_;
    int bufferIndex_;
private:
    int bufferSize_;
    
    uint8_t *buffer_;
    
    // top and left of the top-left point in a specific window
    int top_;
    int left_;
    // width and height of a specific window
    size_t width_;
    size_t height_;
    
    //PluginInstance *pInstance_;
};

#endif /* defined(__ndnrtc__renderer_stub__) */
