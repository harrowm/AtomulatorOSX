#include <string.h>

#ifndef HAVE_STRLCAT
/*
* '_cups_strlcat()' - Safely concatenate two strings.
*/

int strlcat(char *dst, const char *src, size_t size)
{
	int srclen;
	int dstlen;

	dstlen = strlen(dst);
	size -= dstlen + 1;

	if (!size)
		return (dstlen);        /* No room, return immediately... */

	srclen = strlen(src);

	if (srclen > size)
		srclen = size;

	memcpy(dst + dstlen, src, srclen);
	dst[dstlen + srclen] = '\0';

	return (dstlen + srclen);
}
#endif 

#ifndef HAVE_STRLCPY
/*
* '_cups_strlcpy()' - Safely copy two strings.
*/

int strlcpy(char *dst, const char *src, size_t size)
{
	int srclen;

	size--;
	srclen = strlen(src);

	if (srclen > size)
		srclen = size;

	memcpy(dst, src, srclen);
	dst[srclen] = '\0';

	return (srclen);
}
#endif /* !HAVE_STRLCPY */
