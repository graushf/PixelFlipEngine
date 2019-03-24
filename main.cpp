#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <conio.h>

// ***********************************

#include "MemoryPool.h"
#include "PXFRandom.h"

//************************************

int main()
{
	printf("Hola mundo \n\n");
	
	MemoryPool* memoryPool = new MemoryPool();
	bool bSuccess = memoryPool->Init(64, 8);
	
	if (bSuccess) 
	{
		memoryPool->Alloc();
	}
	
	printf("Random: --------------------------------------\n");
	PXFRandom r;
	r.Randomize();

	for (int i = 0; i < 100; ++i) 
	{
		unsigned int num = r.Random(100);
		printf("[%d] => Random Result = %d\n", i, num);
	}
	system("PAUSE");

	return 0;
}