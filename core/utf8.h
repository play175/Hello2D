/*!
 * Hello2D Game Engine
 * yoyo 2015 ShenZhen China
 * repo:https://github.com/play175/Hello2D
 * website:http://yoyo.play175.com
 * MIT Licensed
 */
#ifndef __UTF8_H
#define __UTF8_H

//utf8字符长度1-6，可以根据每个字符第一个字节判断整个字符长度
//0xxxxxxx
//110xxxxx 10xxxxxx
//1110xxxx 10xxxxxx 10xxxxxx
//11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
//111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
//1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
//
//定义查找表，长度256，表中的数值表示以此为起始字节的utf8字符长度
static unsigned char utf8_look_for_table[] = {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
	4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 1, 1
};

#define UTFLEN(x)  utf8_look_for_table[(x)]


#define utf8_for_each(str,strsize,chrptr,chrlen,index) \
	for (chrptr = str,index = 0,chrlen = UTFLEN((unsigned char)*str);\
    chrptr < str + strsize; \
    index++, chrptr += UTFLEN((unsigned char)*chrptr),chrlen = (index < strsize)?UTFLEN((unsigned char)*chrptr) : 0 )


//计算str字符数目
static inline int utf8_strlen(char *str,int strlen) {
	int len = 0;
	char *ptr;
	for (ptr = str;
	        ptr < str + strlen;
	        len++, ptr += UTFLEN((unsigned char)*ptr));
	return len;
}

static inline int utf8_crlflen(char *str,int strlen) {
	int len = 0;
	char *ptr;
	for (ptr = str;
	        ptr < str + strlen;
	        len+=(*ptr == '\n')?1:0, ptr += UTFLEN((unsigned char)*ptr));
	return len;
}

//get子串
static inline char *utf8_substr(char *str, unsigned int start, unsigned int end) {
	int len = utf8_strlen(str,strlen(str));

	if (start >= len) return NULL;
	if (end > len) end = len;

	char *sptr = str;
	int i;
	for (i = 0; i < start; ++i,sptr+=UTFLEN((unsigned char)*sptr));

	char *eptr = sptr;
	for (i = start; i < end; ++i,eptr += UTFLEN((unsigned char)*eptr));

	int sublen = eptr - sptr;
	char *sub = (char*)malloc(sublen + 1);
	memcpy(sub, sptr, sublen);
	sub[sublen] = 0;

	return sub;
}

#endif