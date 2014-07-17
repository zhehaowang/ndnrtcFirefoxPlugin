//
//  LinkedList.h
//  BasicPlugin
//
//  Created by ZhehaoWang on 7/17/14.
//
//

#ifndef __BasicPlugin__LinkedList__
#define __BasicPlugin__LinkedList__

class Node
{
public:
    Node(void * data);
    ~Node();
    
    Node * next_;
    void * data_;
};

class LinkedList
{
public:
    LinkedList();
    ~LinkedList();
    
    Node * head_;
    Node * tail_;
    Node * curr_;
    
    bool addNode(Node *newNode);
    bool freeAll();
};

#endif /* defined(__BasicPlugin__LinkedList__) */
