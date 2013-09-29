// OPStream.cpp: implementation of the OPStream class.
//
//////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <string.h>
#include <string>
#include <queue>
#include "IOHandle.hpp"

#define OP_BUFFER_LEN 16380

class Buffer {
public:
    Buffer() : m_size(0) {}

    int get(char **buf, int len, int *rlen)
    {
        char *p = *buf;
        if (m_data.empty()) {
            *rlen = 0;
            return 0;
        }
        block_t b = m_data.front();
        m_data.pop();
        if (b.length > len) {
            if (!p) {
                p = (char *)malloc(b.length);
            }else{
                p = (char *)realloc(p, b.length);
            }
            *buf = p;
        }
        memcpy(p, b.data, b.length);
        *rlen = b.length;
        if (b.data) free(b.data);
        return 0;
    }

    int put(char *buf, int len, int eof)
    {
        block_t b;

        b.length = len;
        b.data = (char *)malloc(len);
        memcpy(b.data, buf, len);
        m_data.push(b);
        m_size+= b.length;
        return 0;
    }
    int size() { return m_size; }

private:
    struct block_t {
        size_t length;
        char *data;
    };
    size_t m_size;
    std::queue<block_t> m_data;
};


template <typename Buffer>
class OPStream {
public:
    OPStream(Buffer &b) : m_buffer(b)
    {
        s_cur = s_buf;
        s_end = s_cur + OP_BUFFER_LEN;
    
        r_size = 0;
        r_eof = 0;
        r_len = 0;
        r_buf = NULL;
        r_cur = NULL;
        r_end = NULL;
    }
    
    ~OPStream()
    {
    	if (r_buf) {
    		free(r_buf);
    		r_buf = NULL;
    	}
    }
    
    int flush(int eof)
    {
    	int len = s_cur - s_buf;
    	if (len == 0)
    		return 0;
    
    	int rv = m_buffer.put(s_buf, len, eof);
    	s_cur = s_buf;
    	return rv;
    }
    
    int writeByte(char c)
    {
    	int rv = 0;
    	if (s_end - s_cur < 1){
    		rv = flush(0);
    	}
    	s_cur[0] = c;
    	s_cur++;
    	return rv;
    }
    
    int writeShort(short s)
    {
    	int rv = 0;
    	if (s_end - s_cur < 2){
    		rv = flush(0);
    	}
    	put_short(s_cur, s);
    	s_cur += 2;
    	return rv;
    }
    
    int writeInt(int i)
    {
    	int rv = 0;
    	if (s_end - s_cur < 4){
    		rv = flush(0);
    	}
    	put_int(s_cur, i);
    	s_cur += 4;
    	return rv;
    }
    
    int writeLong(long long l)
    {
    	int rv = 0;
    	if (s_end - s_cur < 8){
    		rv = flush(0);
    	}
    	put_long(s_cur, l);
    	s_cur += 8;
    	return rv;
    }
    
    int writeFloat(float f)
    {
    	int rv = 0;
    	if (s_end - s_cur < 4){
    		rv = flush(0);
    	}
    	put_float(s_cur, f);
    	s_cur += 4;
    	return rv;
    }
    
    int writeDouble(double d)
    {
    	int rv = 0;
    	if (s_end - s_cur < 8) {
    		rv = flush(0);
    	}
    	put_double(s_cur, d);
    	s_cur += 8;
    	return rv;
    }
    
    int writeBytes(const char *buf, int len)
    {
    	int rv = 0;
    	int avail = s_end - s_cur;
    
    	if (avail > len) {
    		memcpy(s_cur, buf, len);
    		s_cur += len;
    	}
    	else {
    		rv = flush(0);
    		while (rv==0 && len >= OP_BUFFER_LEN)
    		{
    			rv = m_buffer.put(buf, OP_BUFFER_LEN, 0);
    			buf += OP_BUFFER_LEN;
    			len -= OP_BUFFER_LEN;
    		}
    		if (rv == 0) {
    			memcpy(s_cur, buf, len);
    			s_cur += len;
    		}
    	}
    	return rv;
    }
    
    int writeUTF(const char *buf, int len)
    {
    	int rv = 0;
    	if (s_end - s_cur < 2 + len) {
    		rv = flush(0);
    		if (rv != 0) return rv;
    	}
    	rv = writeShort(len);
    	if (rv == 0)
    		rv = writeBytes(buf, len);
    	return rv;
    }
    
    int writeUTF(const std::string &str)
    {
    	int len = str.length();
    	int rv = 0;
    	if (s_end - s_cur < 2 + len) {
    		rv = flush(0);
    		if (rv != 0) return rv;
    	}
    	rv = writeShort(len);
    	if (rv == 0)
    		rv = writeBytes(str.c_str(), len);
    	return rv;
    }
    
    //// readers
    
    void reset()
    {
    	s_cur = s_buf;
        r_cur = r_buf;
    	r_eof = 0;
    	r_size = 0;
    }
    
    int skip(int len)
    {
    	char buf[1024];
    	int n, rv = 0;
    	while (len > 0)
    	{
    		n = len > 1024 ? 1024 : len;
    		rv = readBytes(buf, n);
    		if (rv != 0)
    			break;
    		len-= n;
    	}
    	return rv;
    }
    
    int clear()
    {
    	if (r_eof) return 0;
    
        int len = 0;
    	int rv = 0;
    	while (!r_eof)
    	{
            rv = m_buffer.get(&rbuf, r_len, &len);
    		if (rv != 0) break;
            if (len > r_len) r_len = len;
            r_cur = r_buf;
            r_eof = (len==0 ? 1 : 0);
    	}
        r_cur = r_buf;
        r_eof = 0;
        r_size = 0;
    	return rv;
    }
    
    // return: 0 - ok
    //		  -1 - to close connection
    //		   1 - to reuse connection
    //
    
    int read()
    {
    	if (r_eof) return -1;
        int len = 0;
    	int rv = m_buffer.get(&r_buf, r_len, &len);
    	if (rv == 0) {
            if (len > r_len) r_len = len;
            r_cur = r_buf;
            r_end = r_buf + len;
            r_size += len;
            r_eof = (len==0 ? 1 : 0);
    	}
    	return rv;
    }
   
    int readByte(char *c)
    {
    	return readBytes(c, 1);
    }
    
    int readShort(short *s)
    {
    	char buf[2];
    	int rv = readBytes(buf, 2);
    	if (rv==0) {
    		*s = get_short(buf);
    	}
    	return rv;
    }
    
    int readInt(int *l)
    {
    	char buf[4];
    	int rv = readBytes(buf, 4);
    	if (rv==0) {
    		*l = get_int(buf);
    	}
    	return rv;
    }
    
    int readInt(long *l)
    {
    	char buf[4];
    	int rv = readBytes(buf, 4);
    	if (rv==0) {
    		*l = get_int(buf);
    	}
    	return rv;
    }
    
    int readLong(long long *l)
    {
    	char buf[8];
    	int rv = readBytes(buf, 8);
    	if (rv==0) {
    		*l = get_long(buf);
    	}
    	return rv;
    }
    
    int readFloat(float *f)
    {
    	char buf[4];
    	int rv = readBytes(buf, 4);
    	if (rv==0) {
    		*f = get_float(buf);
    	}
    	return rv;
    }
    
    int readDouble(double *d)
    {
    	char buf[8];
    	int rv = readBytes(buf, 8);
    	if (rv==0) {
    		*d = get_double(buf);
    	}
    	return rv;
    }
    
    int readBytes(char *buf, int len)
    {
    	int l, rv = 0;
    	
    	int avail = r_end - r_cur;
    	if (len <= avail)
    	{
    		memcpy(buf, r_cur, len);
    		r_cur += len;
    		return 0;
    	}
    	memcpy(buf, r_cur, avail);
    	r_cur = r_end;
    	buf += avail;
    	len -= avail;
    	while (len > 0) {
    		rv = read();
    		if (rv != 0) break;
    		avail = r_end - r_cur;
    		l = avail > len ? len : avail;
    		memcpy(buf, r_cur, l);
    		r_cur += l;
    		buf += l;
    		len -= l;
    	}
    	return rv;
    }
    
    int readUTF(char *buf, int buflen, int *len)
    {
    	unsigned short l = 0;
    
        *len = 0;
    	int rv = readShort((short *)&l);
    	if (rv == 0)
    	{
            if (l > 0 && l < buflen) {
    		    *len = l;
    		    rv = readBytes(buf, l);
            }else{
                rv = l;
            }
    	}
    	return rv;
    }
    
    int readUTF(char **buf, int *len)
    {
    	short l = 0;
    
        *len = 0;
    	int rv = readShort(&l);
    	if (rv == 0)
    	{
            if (l >= 0) {
    		    *len = l;
                char *p = (char *)malloc(l+1);
                if (!p) return -1;
    		    rv = readBytes(p, l);
                p[l] = 0;
                *buf = p;
            }else{
                rv = l;
                *buf = 0;
            }
    	}
    	return rv;
    }
    
    int readUTF(std::string &str)
    {
    	char buf[1024];
    	int n;
    
    	short len = 0;
    	int rv = readShort(&len);
    
    	str = "";
    	while (rv == 0 && len > 0)
    	{
    		n = len > 1024 ? 1024 : len;
    		rv = readBytes(buf, n);
    		if (rv == 0) {
    			str.append(buf, n);
    			len-= n;
    		}
    	}
    	return rv;
    }
	
private:
	Buffer &m_buffer;
	char s_buf[OP_BUFFER_LEN], *s_cur, *s_end;
	char *r_buf, *r_cur, *r_end;
	int r_size, r_len, r_eof;
};
