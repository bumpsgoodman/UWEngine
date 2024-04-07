/*
* List
*
* 작성자: bumpsgoodman
* 작성일: 2023.12.18
* 
* List는 링크 및 언링크만 해줌
* 실제 메모리 할당/해제는 사용자가 직접 해야 함
*/

#pragma once

#include "UWEngineCommon/Defines.h"

struct ListNode
{
    void* pElement;
    struct ListNode* pNext;
    struct ListNode* pPrev;
};

struct List
{
    ListNode* pHead;
    ListNode* pTail;
};

GLOBAL_FUNC UWMETHOD(void) ListAddHead(ListNode** ppHead, ListNode** ppTail, ListNode* pNode);
GLOBAL_FUNC UWMETHOD(void) ListAddTail(ListNode** ppHead, ListNode** ppTail, ListNode* pNode);

GLOBAL_FUNC UWMETHOD(void) ListDeleteHead(ListNode** ppHead, ListNode** ppTail);
GLOBAL_FUNC UWMETHOD(void) ListDeleteTail(ListNode** ppHead, ListNode** ppTail);

GLOBAL_FUNC UWMETHOD(void) ListInsertNode(ListNode** ppHead, ListNode** ppTail, ListNode* pPrevNode, ListNode* pNode);
GLOBAL_FUNC UWMETHOD(void) ListDeleteNode(ListNode** ppHead, ListNode** ppTail, ListNode* pDeleteNode);