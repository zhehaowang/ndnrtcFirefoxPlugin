//
//  renderer-stub.cpp
//  ndnrtc
//
//  Created by Peter Gusev on 6/26/14.
//  Copyright (c) 2014 Peter Gusev. All rights reserved.
//

#include "BrowserRenderer.h"

uint8_t*
BrowserRenderer::getFrameBuffer(int width, int height)
{
    int requiredSize = width*height*3;
    
    //renderBufferLock.lock();
    if (!buffer_ || bufferSize_ < requiredSize)
    {
        //both malloc and realloc works
        
        buffer_ = (uint8_t*)realloc((void*)buffer_, requiredSize);
        //buffer_ = (uint8_t*)malloc(requiredSize);
        
        bufferSize_ = requiredSize;
    }
    //renderBufferLock.unlock();
    
    return buffer_;
}

void
BrowserRenderer::renderRGBFrame(int64_t timestamp, int width, int height,
                             const uint8_t *buffer)
{
    // this function can either be a rendering callback, or a callback that updates the buffer to be rendered
    // in the case of our NPAPI browser integration, it's the latter.
    
    // Interesting, it seems that the lock here does not really do much...
    
    // The problem with this function here is not cross-thread reference. It's the wrong declaration of browser variable;
    // However, this method does seem to be outperformed by firing events periodically.
    //browser->invalidaterect(pInstance_->npp, &(pInstance_->window.clipRect));
    
    // Still the problem of having lock versus not, the lock does not seem to have any impacts at all?
    
    //renderWindows[bufferIndex_].renderBufferLock_.lock();
    
    //uint8_t *temp = renderWindows[bufferIndex_].renderBuffer_;
    
    // this is the line that actually works...should be able to work without this one
    renderWindows[bufferIndex_].renderBuffer_ = buffer_;
    
    renderWindows[bufferIndex_].bufferFilled_ = true;
    
    // For a local testing connection, starting a publisher first and consumer second won't bring me any data.
    // Issue being debugged.
    //printf("renderRGBFrame called for render window %d.\n", bufferIndex_);
    
    //buffer_ = temp;
    
    //renderWindows[bufferIndex_].renderBufferLock_.unlock();
    
    return;
}

uint8_t * BrowserRenderer::getBuffer()
{
    return buffer_;
}

bool BrowserRenderer::setBuffer(uint8_t * buffer)
{
    buffer_ = buffer;
    return true;
}