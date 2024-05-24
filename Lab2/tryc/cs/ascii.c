#include <stdio.h>
int main()
{
	for(int i = 0; i <= 255; i ++)
	{
		if(i % 16 == 5)
			printf("5: %c ", i);
		else if(i % 16 == 6)
			printf("6: %c ", i);
		else if(i % 16 == 7)
			printf("7: %c ", i);
		else if(i % 16 == 9)
			printf("9: %c ", i);
		else if(i % 16 == 14)
			printf("e: %c ", i);
		else if(i % 16 == 15)
			printf("f: %c ", i);
	}
	return 0;
}
