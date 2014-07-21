//
//  renderWindow.cpp
//  BasicPlugin
//
//  Created by ZhehaoWang on 7/15/14.
//
//

#include "renderWindow.h"
#include "BasicPlugin.h"

RenderWindow::RenderWindow()
{
    bottom_ = 0;
    left_ = 0;
    width_ = defaultWindowWidth;
    height_ = defaultWindowHeight;
    
    bRenderer_ = NULL;
    bufferFilled_ = false;
}

RenderWindow::~RenderWindow()
{
    free (renderBuffer_);
}

size_t RenderWindow::getHeight()
{
    return height_;
}

size_t RenderWindow::getWidth()
{
    return width_;
}

bool RenderWindow::setWidth(size_t width)
{
    width_ = width;
    return true;
}

bool RenderWindow::setHeight(size_t height)
{
    height_ = height;
    return true;
}

int RenderWindow::getLeft()
{
    return left_;
}

int RenderWindow::getBottom()
{
    return bottom_;
}

bool RenderWindow::setLeft(int left)
{
    left_ = left;
    return true;
}

bool RenderWindow::setBottom(int bottom)
{
    bottom_ = bottom;
    return true;
}

bool RenderWindow::generateRect()
{
    rect_ = CGRectMake(left_, bottom_, width_, height_);
    return true;
}

CGRect RenderWindow::getRect()
{
    return rect_;
}