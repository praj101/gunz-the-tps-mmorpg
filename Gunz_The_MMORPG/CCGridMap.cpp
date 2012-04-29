#include "stdafx.h"
#include "CCGridMap.h"


void compile_test(void)
{
	CCGridMap<int> m;
	m.Create(0, 0, 100, 100, 10, 10);
	CCGridMap<int>::HREF hPos = m.Add(0, 0, 0, 1);
	m.Move(10, 10, 10, 1, hPos);
	list<int> nearest;
	m.Get(&nearest, 0, 0, 0, 10);
}