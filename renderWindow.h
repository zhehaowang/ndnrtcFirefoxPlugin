//
//  renderWindow.h
//  BasicPlugin
//
//  Created by ZhehaoWang on 7/15/14.
//
//

#include <mutex>
#include "BrowserRenderer.h"

#ifndef __BasicPlugin__renderWindow__
#define __BasicPlugin__renderWindow__

#define MAX_WINDOW_NAME_LENGTH 50

class renderWindow
{
public:
    renderWindow();
    ~renderWindow();
    
    int getBottom();
    int getLeft();
    
    size_t getWidth();
    size_t getHeight();
    
    bool setBottom(int bottom);
    bool setLeft(int left);
    
    bool setWidth(size_t width);
    bool setHeight(size_t height);
    
    uint8_t *renderBuffer_;
    
    bool generateRect();
    CGRect getRect();
    
    std::mutex renderBufferLock_;

    // how is cross referencing happening here? Using a pointer to BrowserRenderer will result in an error.
    void *bRenderer_;
    
    bool bufferFilled_;
    
    char windowName_[MAX_WINDOW_NAME_LENGTH];
private:
    // top and left of the top-left point in a specific window
    int bottom_;
    int left_;
    // width and height of a specific window
    size_t width_;
    size_t height_;
    
    CGRect rect_;
};

#endif /* defined(__BasicPlugin__renderWindow__) */
