//
#pragma once

#include <windows.h>

typedef struct LinkedItem_t
{
	LinkedItem_t*	pPrev;
	LinkedItem_t*	pNext;
	LPVOID			lpData;
} LinkedItem;

class LinkedList
{
private:
	LinkedItem*	pFirst;
	LinkedItem*	pLast;
	UINT		cItemCount;
public:
	LinkedList();
	virtual ~LinkedList();
	LinkedItem* AddItem(LPVOID lpData);
	LinkedItem* RemoveItem(LinkedItem* pItem);
	LinkedItem* GetFirstItem();
	LinkedItem* GetLastItem();
	LinkedItem* GetPrevItem(LinkedItem* pItem);
	LinkedItem* GetNextItem(LinkedItem* pItem);
	UINT LinkedList::GetItemCount();
};
