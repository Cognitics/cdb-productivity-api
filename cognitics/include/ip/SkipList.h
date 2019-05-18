/****************************************************************************
Copyright (c) 2019 Cognitics, Inc.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
****************************************************************************/

#pragma once

//Used for random number generation
#include <cstdlib>

#define PRINT_OUT

#ifdef PRINT_OUT
#include <iostream>
#endif
namespace ip
{
/**
*\def MAX_LIST_LEVEL
*\brief
*    This definition will restrict the height of the Skip List, it should be chosen
*    so that MAX_LIST_LEVEL > log(1/p)n. Where p is the probability of adding a level
*    and n is the max number of elements in the list. For example, a probability of 1/2
*    (considered standard) and MAX_LIST_LEVEL of 17 will allow successfull usage of
*    a list conatining up to 131,072 elements. Many more than is needed in most cases,
*    but since increasing the size of MAX_LIST_LEVEL only increases storage needed
*    by the memory of a single pointer. Which in most cases is not a large cost at all.
*/
#define MAX_LIST_LEVEL 17

/**
*\def SKIP_PROB
*\brief
*    Standard Skip List probability of 1/2. This makes the Skip List perform as well
*    as balanced binary search trees, if the list is long enough.
*/
#define SKIP_PROB 0.5

/**\def NULL_SLN
*\brief
*    Definition of a Skip List Node null pointer.
*/
#define NULL_SLN (SkipListNode<T>*)0

/**
================================================================================
Skip List
================================================================================
*    
*    Skip List data strucutre, sorted with a given probability
*/

/**\class SkipListNode
*\brief
*    Struct definition for a SkipListNode conatining an array of pointers to the 
*   next values, as well as pointers to the prev value. The prev is used to point
*   only to the previous value on the lowest list, alowing faster reverse access.
*/
template <class T>
struct SkipListNode
{
    T                   data;    //data
    SkipListNode<T>**   next;    //array of pointers to the next nodes
    SkipListNode<T>*    prev;    //pointer to the previous node

    /**\fn SkipListNode::SkipListNode(int level, const T& data)
    *\brief
    *    Have to manually set the memory of memset since the node is a template
    *    and can have any amount of memory per node.
    */
    SkipListNode(int level, const T& data)
    {
        next = new SkipListNode<T>*[level+1];
        memset(next, 0, sizeof(SkipListNode<T>*)*(level + 1));
        this->data = data;
    }

    /**\brief
    *    Destructor that deletes the array of pointers to next. 
    */
    ~SkipListNode(void) { delete [] next; }
};

/**\class SkipList
*\brief Class defining a SkipList structure of maximum length maxListCount.
*/
template <class T, typename L>
class SkipList
{
private:
    SkipListNode<T>*    head;   //pointer to the header...doesn't store any data
    SkipListNode<T>*    tail;   //pointer to a NULL vaule of this type.

    int         level;          //current level of the list;
    int         listSize;       //size of list

    L         otherData;    //pointer to other data that might be required

    //---->RANDOM NUMBER GENERATION
    /**\brief
    *    Returns a random bool based on the given probability using the rand() function
    */
    bool getRandBool (void)
    {
        return ((double)(rand()/RAND_MAX) > SKIP_PROB) ? true : false;
    }

    /**\brief
    *    Returns a random height based on a log probability, this creates an artificial
    *    tree like structure to the height of the nodes.
    */
    int getRandomLevel (void)
    {
        int level = 1;
        while (getRandBool())
            level++;
        return (level < MAX_LIST_LEVEL) ? level : MAX_LIST_LEVEL;
    }

public:
    //---->INITIALIZE
    /**\brief
    *    Initializes the header and tail pointers and sets the level and size to 0;
    */
    SkipList(void)
    {
        head = new SkipListNode<T>(MAX_LIST_LEVEL, T());
        tail = NULL_SLN;    //null pointer to a skip list node
        level = 0;
        listSize = 0;
    }

    ~SkipList(void) { delete head; }    //recursively delete the list.

    void setOther(L ot) {
        otherData = ot;
    } //set the pointer to the other data needed

    int    getSize(void) { return listSize; }

    SkipListNode<T>* getFirst(void) { return head->next[0]; }

    //---->SEARCH
    /**\brief
    *    Searches for the node with the given data. This search is performed in
    *    O(logn) time, and is carried out very similarly to a binary search.
    *    The search is performed by moving forward along the highest level until
    *    the compare relates that the current position is greater than the data.
    *    The search then steps down a layer and runs the same comparison loop again.
    *    This continues until the search is at the lowest level. Resulting in either
    *    returning a null (tail) pointer or the result.
    */
    SkipListNode<T>* find(T data)
    {
        SkipListNode<T>*    x = head;

        for (int i = level; i>=0; i--)
        {
            while (x->next[i] != NULL_SLN && compare(otherData,data,x->next[i]->data) > 0)
            {    x = x->next[i];  }
        }
        x = x->next[0];
        return x;
    }

    /**\brief
    *    Uses the find algorithm above. If the data is found, returns true. If it is not
    *    in the list, false is returned.
    */
    bool contains(T data)
    {
        SkipListNode<T>*    x = find(data);
        return (x != NULL_SLN) && (compare(otherData,data,x->data)==0);
    }

    //---->INSERT
    /**\brief
    *    Uses the find algorithm above. If the data is found, returns true. If it is not
    *    in the list, false is returned.
    */
    SkipListNode<T>* insert(T const data)
    {
        SkipListNode<T>*    x = head;
        SkipListNode<T>*    update[MAX_LIST_LEVEL +  1];
        memset(update, 0, sizeof(SkipListNode<T>*)*(MAX_LIST_LEVEL + 1));

        //search to find location to insert into
        for (int i = level; i>= 0; i--)
        {
            while (x->next[i] != NULL_SLN && compare(otherData,data,x->next[i]->data) > 0)
            {    x = x->next[i];  }
            update[i] = x;
        }
        x = x->next[0];
        //check to make sure the data is not already in the list
        if (x == NULL_SLN || compare(otherData,data,x->data)!=0)
        {
            int lvl = getRandomLevel();
            if(lvl>level)    //case where the new level is higher than the current level
            {
                for (int i = level+1; i<= lvl; i++)
                {
                    update[i]= head;
                }
                level = lvl;
            }

            x = new SkipListNode<T>(lvl, data);    //create new node to insert
            for (int i = 0; i<= lvl; i++)        //insert node into list and update connections
            {
                x->next[i] = update[i]->next[i];
                update[i]->next[i] = x;
            }
            //update base list prev pointers
            if(x->next[0]!=NULL_SLN) x->next[0]->prev = x;
            x->prev = update[0];
            listSize++;
        }

        return x;   //return a pointer to the node inserted
    }

    //---->DELETE
    /**\brief
    *    Uses a similar algorithm to 
    */
    void remove(T data)
    {
        SkipListNode<T>*    x = head;
        SkipListNode<T>*    update[MAX_LIST_LEVEL +  1];
        memset(update, 0, sizeof(SkipListNode<T>*)*(MAX_LIST_LEVEL + 1));

        //search to find location to delete
        for (int i = level; i>= 0; i--)
        {
            while (x->next[i] != NULL_SLN && compare(otherData,data,x->next[i]->data) > 0)
            {    x = x->next[i];  }
            update[i] = x;
        }
        x = x->next[0];

        //if the data is found
        if (compare(otherData,x->data,data)== 0)
        {
            listSize--;
            //replace links to and from x
            for (int i =  0; i <= level; i++)
            {
                if(update[i]->next[i] != x) break;
                update[i]->next[i] = x->next[i];
            }
            if(update[0]->next[0] != NULL_SLN) update[0]->next[0]->prev = update[0];

            delete x;

            //recheck level to ensure it is accurate
            while (level > 0 && head->next[level] == NULL_SLN)
                level--;
        }
    }

    //---->SWAP
    /**\brief
    *    This swaps the data of two nodes if they are found in the list. 
    *    WARNING:    This can destroy the structure order if the swap is not valid, causing
    *    problems if the data has not already been compared to ensure this is a valid
    *    swap. This is a shortcut for swappping two items that have had their data changed
    *    or in a case were you have already validated that this is a legitimate swap. This 
    *    will avoid having to do any delete or insertion routines.
    */
    void swap(T a, T b)
    {
        //don't need to check if a or b are null since they should be defined as data types
        //and not pointers.

        SkipListNode<T>*    nodeA = find(a);
        SkipListNode<T>*    nodeB = find(b);

        if(nodeA==NULL_SLN || nodeB==NULL_SLN) return;

        T      temp = nodeA->data;
        nodeA->data = nodeB->data;
        nodeB->data = temp;
    }

#ifdef PRINT_OUT
    void print()
    {
        ccl::ObjLog log("SkipListNode");
        SkipListNode<T>*    x = head->next[0];
        log << "{ ";
        while (x != NULL_SLN)
        {
            //std::cout << x->data;
            log << " [" << x->data.bottom.x << "," << x->data.bottom.y << "] -> [" <<  x->data.top.x << "," << x->data.top.y << "] ";
            x = x->next[0];
            if (x != NULL_SLN) std::cout << ",";
        }
        log << " }" << log.endl;
    }
#endif

};

}