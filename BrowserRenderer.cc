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
        buffer_ = (uint8_t*)realloc((void*)buffer_, requiredSize);
        bufferSize_ = requiredSize;
    }
    //renderBufferLock.unlock();
    
    return buffer_;
}

void
BrowserRenderer::renderRGBFrame(int64_t timestamp, int width, int height,
                             const uint8_t *buffer)
{
    // this function can either be a rendering callback, or a callback that updated the info to be rendered
    // in the case of NPAPI browser integration, it's the latter.
    
    // Interesting, it seems that the lock here does not really do much...
    
    // The problem with this function here is not cross-thread reference, it's the wrong declaration of browser variable;
    // However, this method does seem to be outperformed by firing events periodically.
    //browser->invalidaterect(pInstance_->npp, &(pInstance_->window.clipRect));
    
    renderBufferLock.lock();
    //uint8_t *temp = renderBuffer;
    //memcpy(renderBuffer, buffer_, 921600);
    renderBuffer = buffer_;
    //buffer_ = temp;
    //printf("Render buffer ready, buffer size: %d\n", bufferSize_);
    renderBufferLock.unlock();
    
    return;
}