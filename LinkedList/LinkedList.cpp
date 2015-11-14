//
#include "LinkedList.h"

//
LinkedList::LinkedList()
{
	pFirst = 0;
	pLast = 0;
	cItemCount = 0;
}

LinkedList::~LinkedList()
{
	LinkedItem* pItem;
	while ((pItem = GetFirstItem()) != 0)
		RemoveItem(pItem);
}

//============================================================================
//	AddItem
//----------------------------------------------------------------------------
//============================================================================
LinkedItem* LinkedList::AddItem(LPVOID lpData)
{
	LinkedItem* pItem = new LinkedItem;

	if (!pItem)
		return 0;

	pItem->lpData = lpData;
	pItem->pNext = 0;

	if (!pFirst)
		pFirst = pItem;

	if (pLast)
		pLast->pNext = pItem;

	pItem->pPrev = pLast;
	pLast = pItem;

	cItemCount++;

	return pItem;
}

//============================================================================
//	RemoveItem
//----------------------------------------------------------------------------
//============================================================================
LinkedItem* LinkedList::RemoveItem(LinkedItem* pItem)
{
	LinkedItem* pNext;

	if (!pFirst || !pLast || !pItem)
		return 0;

	if (pItem->pPrev)
		pItem->pPrev->pNext = pItem->pNext;

	if (pItem->pNext)
		pItem->pNext->pPrev = pItem->pPrev;

	if (pItem == pFirst)
		pFirst = pItem->pNext;

	if (pItem == pLast)
		pLast = pItem->pPrev;

	pNext = pItem->pNext;

	delete pItem;

	cItemCount--;

	return pNext;
}

//============================================================================
//	GetFirstItem
//----------------------------------------------------------------------------
//============================================================================
LinkedItem* LinkedList::GetFirstItem()
{
	return pFirst;
}

//============================================================================
//	GetLastItem
//----------------------------------------------------------------------------
//============================================================================
LinkedItem* LinkedList::GetLastItem()
{
	return pLast;
}

//============================================================================
//	GetPrevItem
//----------------------------------------------------------------------------
//============================================================================
LinkedItem* LinkedList::GetPrevItem(LinkedItem* pItem)
{
	return (pItem ? pItem->pPrev : 0);
}

//============================================================================
//	GetNextItem
//----------------------------------------------------------------------------
//============================================================================
LinkedItem* LinkedList::GetNextItem(LinkedItem* pItem)
{
	return (pItem ? pItem->pNext : 0);
}

//============================================================================
//	GetItemCount
//----------------------------------------------------------------------------
//============================================================================
UINT LinkedList::GetItemCount()
{
	return cItemCount;
}
