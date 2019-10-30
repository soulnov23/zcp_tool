#ifndef __BUFFER_H__
#define __BUFFER_H__

class buffer
{
public:
	//初始化大小为64K，每次超过都扩大2倍
	buffer();
	~buffer();

	void append(const char* data, int len);
	void remove(int len);
	int size();
	
private:
	char* m_buffer;
	int m_size;
	int m_max_size;
};

#endif