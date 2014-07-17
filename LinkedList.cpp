//
//  LinkedList.cpp
//  BasicPlugin
//
//  Created by ZhehaoWang on 7/17/14.
//
//

#include "LinkedList.h"

node::node(void * data)
{
    data_ = data;
}

node::~node()
{
    free(data_);
}

linkedList::~linkedList()
{
    freeAll();
}

linkedList::linkedList()
{
    head_ = NULL;
    curr_ = NULL;
    tail_ = NULL;
}

bool linkedList::addNode(node *newNode)
{
    if (head_ == NULL)
    {
        head_ = newNode;
        tail_ = newNode;
        curr_ = head_;
    }
    else
    {
        tail_->next_ = newNode;
        tail_ = newNode;
    }
    return true;
}

bool linkedList::freeAll()
{
    node * temp = NULL;
    
    if (head_ != NULL)
    {
        curr_ = head_;
        while (curr_ != NULL)
        {
            temp = curr_->next_;
            free (curr_->data_);
            free (curr_);
            curr_ = temp;
        }
    }
    return true;
}