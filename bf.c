/*
* Copyright (C) 2013, William H. Welna All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*
* THIS SOFTWARE IS PROVIDED BY William H. Welna ''AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL William H. Welna BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <pstdint.h>

char a[5000];
char *p=a;

void bf(char *s, unsigned int l) {
	unsigned int x, nest_counter=0;
	while(l>0) {
		switch(*s) {
			case '<': p--; goto bf_end; // using a goto to decrease code size / repeating
			case '>': p++; goto bf_end;
			case '+': (*p)++; goto bf_end;
			case '-': (*p)--; goto bf_end;
			case '.': putchar((unsigned char)*p); goto bf_end;
			case ',': *p=(unsigned char)getchar(); goto bf_end;
			case '[':
				  ++s; --l; for(x=0; *(s+x) != ']' || nest_counter!=0; ) {
					  if(*(s+x) == '[') ++nest_counter;
					  if(*(s+x) == ']') --nest_counter;
					  ++x;
				  }
				  while(*p) bf(s, x);
				  s+=x; l-=x;
			default:
bf_end:
				  --l; ++s;
				  break;
		}
	}
}

char *loadfile(char *f, uint32_t *s) {
	HANDLE hFile = CreateFile(f, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile) { // File is open!
		int totalread, size;
		char *ret;
		if(*s == 0)
			size = GetFileSize(hFile, NULL);
		else
			size = *s;
		ret = calloc(size, sizeof(char)+1);
		if(ret==NULL) {
			CloseHandle(hFile);
			*s=0;
			return NULL;
		}
		if(ReadFile(hFile, ret, size, &totalread, NULL)!=FALSE) {
			CloseHandle(hFile);
			*s=totalread;
			return ret;
		}
		else {
			CloseHandle(hFile);
			*s=0;
			free(ret);
			return NULL;
		}
	} else
		return NULL;
}

int main(int argc, char **argv) {
	char *b;
	uint32_t s=0;
	memset(a, 0, 5000);
	if(argc!=2)
		fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
	else {
		b=loadfile(argv[1], &s);
		if(s && b) {
			bf(b, s);
			free(b);
		} else
			fprintf(stderr, "%s: Can't open file %s\n", argv[0], argv[1]);
	}
	return 0;
}
