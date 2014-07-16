//
//  renderWindow.cpp
//  BasicPlugin
//
//  Created by ZhehaoWang on 7/15/14.
//
//

#include "renderWindow.h"
#include "BasicPlugin.h"

renderWindow::renderWindow()
{
    bottom_ = 0;
    left_ = 0;
    width_ = defaultWindowWidth;
    height_ = defaultWindowHeight;
}

renderWindow::~renderWindow()
{
    free (renderBuffer_);
}

size_t renderWindow::getHeight()
{
    return height_;
}

size_t renderWindow::getWidth()
{
    return width_;
}

bool renderWindow::setWidth(size_t width)
{
    width_ = width;
    return true;
}

bool renderWindow::setHeight(size_t height)
{
    height_ = height;
    return true;
}

int renderWindow::getLeft()
{
    return left_;
}

int renderWindow::getBottom()
{
    return bottom_;
}

bool renderWindow::setLeft(int left)
{
    left_ = left;
    return true;
}

bool renderWindow::setBottom(int bottom)
{
    bottom_ = bottom;
    return true;
}

bool renderWindow::generateRect()
{
    rect_ = CGRectMake(left_, bottom_, width_, height_);
    return true;
}

CGRect renderWindow::getRect()
{
    return rect_;
}