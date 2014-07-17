//
//  LinkedList.h
//  BasicPlugin
//
//  Created by ZhehaoWang on 7/17/14.
//
//

#ifndef __BasicPlugin__LinkedList__
#define __BasicPlugin__LinkedList__

class node
{
public:
    node(void * data);
    ~node();
    
    node * next_;
    void * data_;
};

class linkedList
{
public:
    linkedList();
    ~linkedList();
    
    node * head_;
    node * tail_;
    node * curr_;
    
    bool addNode(node *newNode);
    bool freeAll();
};

#endif /* defined(__BasicPlugin__LinkedList__) */
