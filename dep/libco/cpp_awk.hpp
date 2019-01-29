#ifndef __CPP_AWK_H__
#define __CPP_AWK_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
class awk
{
public:
	char m_linebuf[4096];
	char m_linesave[4096];
	char *m_ptr[1024];
	char m_tok[32];
	int NF;
	int len;
	FILE *m_fp;
	explicit awk(FILE *fp,const char *tok):m_fp(fp)
	{
		m_linebuf[0] = '\0';
		m_linesave[0] = '\0';
		memset(m_ptr,0,sizeof(m_ptr));
		strncpy(m_tok,tok,sizeof(m_tok));
		NF = 0;
		len=0;
	}
	char *operator[](int i)
	{
		return (m_ptr[i]?m_ptr[i]:(char*)"");
	}
	uint32_t operator()(int i)
	{
		return strtoul( (m_ptr[i]?m_ptr[i]:(char*)""),NULL,10);
	}
	void split(char *tok)
	{
		len=strlen(m_linebuf);
		if( len > 0 )
		{
			char *end = m_linebuf + len - 1;
			while(end > m_linebuf)
			{
				if(*end == '\n')
				{
					*end = 0;
					--len;
					--end;
				}
				else
				{
					break;
				}
			}
		}
		strcpy(m_linesave,m_linebuf);
		memset(m_ptr,0,sizeof(m_ptr));
		char *p1;
		char *p = strtok_r(m_linebuf,tok,&p1);
		m_ptr[0] = m_linesave;
		NF=0;
		while(p)
		{
			++NF;
			m_ptr[NF] = p;
			p = strtok_r(NULL,tok,&p1);
		}

	}
	void clear()
	{
		m_linebuf[sizeof(m_linebuf)-1] = 0;
		m_linebuf[0] = 0;
		m_linesave[sizeof(m_linebuf)-1] = 0;
		m_linesave[0] = 0;
		len = 0;

	}
	char *get()
	{
		char *p = fgets(m_linebuf,sizeof(m_linebuf),m_fp);
		if(!p) return p;
		split(m_tok);
		return m_linesave;
		
	}
};
#endif

