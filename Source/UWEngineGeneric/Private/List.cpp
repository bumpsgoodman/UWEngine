/*
* List
*
* 작성자: bumpsgoodman
* 작성일: 2023.12.18
*/

#include "Precompiled.h"
#include "List.h"

GLOBAL_FUNC UWMETHOD(void) ListAddHead(ListNode** ppHead, ListNode** ppTail, ListNode* pNode)
{
    ASSERT(ppHead != NULL, "ppHead == NULL");
    ASSERT(ppTail != NULL, "ppTail == NULL");
    ASSERT(pNode != NULL, "pNode == NULL");

    if (*ppHead == NULL)
    {
        *ppHead = *ppTail = pNode;
        pNode->pNext = NULL;
        pNode->pPrev = NULL;
    }
    else
    {
        pNode->pNext = *ppHead;
        pNode->pPrev = NULL;

        (*ppHead)->pPrev = pNode;
        *ppHead = pNode;
    }
}

GLOBAL_FUNC UWMETHOD(void) ListAddTail(ListNode** ppHead, ListNode** ppTail, ListNode* pNode)
{
    ASSERT(ppHead != NULL, "ppHead == NULL");
    ASSERT(ppTail != NULL, "ppTail == NULL");
    ASSERT(pNode != NULL, "pNode == NULL");

    if (*ppTail == NULL)
    {
        *ppTail = *ppHead = pNode;
        pNode->pNext = NULL;
        pNode->pPrev = NULL;
    }
    else
    {
        pNode->pNext = NULL;
        pNode->pPrev = *ppTail;

        (*ppTail)->pNext = pNode;
        *ppTail = pNode;
    }
}

GLOBAL_FUNC UWMETHOD(void) ListDeleteHead(ListNode** ppHead, ListNode** ppTail)
{
    ASSERT(ppHead != NULL, "ppHead == NULL");
    ASSERT(ppTail != NULL, "ppTail == NULL");

    if (*ppHead == NULL)
    {
        ASSERT(false, "*ppHead == NULL");
        return;
    }

    *ppHead = (*ppHead)->pNext;
    if (*ppHead == NULL)
    {
        *ppTail = NULL;
    }
    else
    {
        (*ppHead)->pPrev = NULL;
    }
}

GLOBAL_FUNC UWMETHOD(void) ListDeleteTail(ListNode** ppHead, ListNode** ppTail)
{
    ASSERT(ppHead != NULL, "ppHead == NULL");
    ASSERT(ppTail != NULL, "ppTail == NULL");

    if (*ppTail == NULL)
    {
        ASSERT(false, "*ppTail == NULL");
        return;
    }

    *ppTail = (*ppTail)->pPrev;
    if (*ppTail == NULL)
    {
        *ppHead = NULL;
    }
    else
    {
        (*ppTail)->pNext = NULL;
    }
}

GLOBAL_FUNC UWMETHOD(void) ListInsertNode(ListNode** ppHead, ListNode** ppTail, ListNode* prevNode, ListNode* pNode)
{
    ASSERT(ppHead != NULL, "ppHead == NULL");
    ASSERT(ppTail != NULL, "ppTail == NULL");
    ASSERT(prevNode != NULL, "prevNode == NULL");
    ASSERT(pNode != NULL, "pNode == NULL");

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

GLOBAL_FUNC UWMETHOD(void) ListDeleteNode(ListNode** ppHead, ListNode** ppTail, ListNode* removedNode)
{
    ASSERT(ppHead != NULL, "ppHead == NULL");
    ASSERT(ppTail != NULL, "ppTail == NULL");
    ASSERT(removedNode != NULL, "removedNode == NULL");

    if (*ppHead == removedNode)
    {
        ListDeleteHead(ppHead, ppTail);
    }
    else if (*ppTail == removedNode)
    {
        ListDeleteTail(ppHead, ppTail);
    }
    else
    {
        removedNode->pPrev->pNext = removedNode->pNext;
        removedNode->pNext->pPrev = removedNode->pPrev;

        removedNode->pPrev = NULL;
        removedNode->pNext = NULL;
    }
}