#include "../WorldServer.h"

unsigned long MakeStrHash(const char *str)
{
	register unsigned long result = 0xDEADC0DE;
	register unsigned long tkey =   0x7FED7FED;
	register char c = 0;
	while (*(str)){
		c = (*str<'a'||*str>'z')?*str:*str-32;
		result += tkey;
		tkey *= 0x21;
		result ^= keys[c];
		tkey += result + c + 3;
		str++;
	}
	return result;
}
