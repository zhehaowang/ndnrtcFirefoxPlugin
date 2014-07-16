//
//  renderWindow.cpp
//  BasicPlugin
//
//  Created by ZhehaoWang on 7/15/14.
//
//

#include "renderWindow.h"

renderWindow::renderWindow()
{
    top_ = 0;
    left_ = 0;
}

renderWindow::~renderWindow()
{
    top_ = 0;
    left_  = 0;
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

int renderWindow::getTop()
{
    return top_;
}

bool renderWindow::setLeft(int left)
{
    left_ = left;
    return true;
}

bool renderWindow::setTop(int top)
{
    top_ = top;
    return true;
}