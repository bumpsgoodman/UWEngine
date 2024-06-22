/*
* List
*
* 작성자: bumpsgoodman
* 작성일: 2023.12.18
*/

#include "Precompiled.h"
#include "UWEngineCommon/Util/List.h"

void __stdcall ListAddHead(ListNode** ppHead, ListNode** ppTail, ListNode* pNode)
{
    if (*ppHead == nullptr)
    {
        *ppHead = *ppTail = pNode;
        pNode->pNext = nullptr;
        pNode->pPrev = nullptr;
    }
    else
    {
        // 이미 추가된 노드거나 널 포인터를 추가할 경우
        if (*ppHead == pNode)
        {
            CRASH();
        }

        pNode->pNext = *ppHead;
        pNode->pPrev = nullptr;

        (*ppHead)->pPrev = pNode;
        *ppHead = pNode;
    }
}

void __stdcall ListAddTail(ListNode** ppHead, ListNode** ppTail, ListNode* pNode)
{
    if (*ppTail == nullptr)
    {
        *ppTail = *ppHead = pNode;
        pNode->pNext = nullptr;
        pNode->pPrev = nullptr;
    }
    else
    {
        // 이미 추가된 노드거나 널 포인터를 추가할 경우
        if (*ppTail == pNode)
        {
            CRASH();
        }

        pNode->pNext = nullptr;
        pNode->pPrev = *ppTail;

        (*ppTail)->pNext = pNode;
        *ppTail = pNode;
    }
}

void __stdcall ListDeleteHead(ListNode** ppHead, ListNode** ppTail)
{
    *ppHead = (*ppHead)->pNext;
    if (*ppHead == nullptr)
    {
        *ppTail = nullptr;
    }
    else
    {
        (*ppHead)->pPrev = nullptr;
    }
}

void __stdcall ListDeleteTail(ListNode** ppHead, ListNode** ppTail)
{
    *ppTail = (*ppTail)->pPrev;
    if (*ppTail == nullptr)
    {
        *ppHead = nullptr;
    }
    else
    {
        (*ppTail)->pNext = nullptr;
    }
}

void __stdcall ListInsertNode(ListNode** ppHead, ListNode** ppTail, ListNode* prevNode, ListNode* pNode)
{
    if (*ppHead == prevNode)
    {
        ListAddHead(ppHead, ppTail, pNode);
    }
    else if (*ppTail == prevNode)
    {
        ListAddTail(ppHead, ppTail, pNode);
    }
    else
    {
        pNode->pNext = prevNode;
        pNode->pPrev = prevNode->pPrev;
        prevNode->pPrev->pNext = pNode;
        prevNode->pPrev = pNode;
    }
}

void __stdcall ListDeleteNode(ListNode** ppHead, ListNode** ppTail, ListNode* pDeleteNode)
{
    if (*ppHead == pDeleteNode)
    {
        ListDeleteHead(ppHead, ppTail);
    }
    else if (*ppTail == pDeleteNode)
    {
        ListDeleteTail(ppHead, ppTail);
    }
    else
    {
        pDeleteNode->pPrev->pNext = pDeleteNode->pNext;
        pDeleteNode->pNext->pPrev = pDeleteNode->pPrev;

        pDeleteNode->pPrev = nullptr;
        pDeleteNode->pNext = nullptr;
    }
}