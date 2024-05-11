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

GLOBAL_FUNC void __stdcall ListAddHead(ListNode** ppHead, ListNode** ppTail, ListNode* pNode);
GLOBAL_FUNC void __stdcall ListAddTail(ListNode** ppHead, ListNode** ppTail, ListNode* pNode);

GLOBAL_FUNC void __stdcall ListDeleteHead(ListNode** ppHead, ListNode** ppTail);
GLOBAL_FUNC void __stdcall ListDeleteTail(ListNode** ppHead, ListNode** ppTail);

GLOBAL_FUNC void __stdcall ListInsertNode(ListNode** ppHead, ListNode** ppTail, ListNode* pPrevNode, ListNode* pNode);
GLOBAL_FUNC void __stdcall ListDeleteNode(ListNode** ppHead, ListNode** ppTail, ListNode* pDeleteNode);