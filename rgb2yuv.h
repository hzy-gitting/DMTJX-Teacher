#pragma once
#define    WIN32_LEAN_AND_MEAN 
#include "ffmpeg.h"
#include <Windows.h>
class YUVencoder
{
public:
	~YUVencoder();
	YUVencoder(int w, int h);
	AVFrame* encode(unsigned char* rgb32, int rgb32_len,
        int width, int height);

private:

	AVFrame* avframe;
	
	AVPicture* spic;
	int width;
	int height;
	
	YUVencoder() { } //取消带参数构造函数
};
