#include "sendscreenvideodatathread.h"
#include<QTime>
#include"ffmpeg.h"
#include"rgb2yuv.h"
#include"ffmpegEncoder.h"
#include"MonitorMaster.h"

#include<QThread>

#define SS_DEBUG 0

SendScreenVideoDataThread::SendScreenVideoDataThread(QObject *parent) : QObject(parent)
{

}

struct cap_screen_t
{
    HDC memdc;
    HBITMAP hbmp;
    unsigned char* buffer;
    int            length;

    int width;
    int height;
    int bitcount;
    int left, top;
};

extern MonitorMaster::VEC_MONITOR_INFO vecMonitorListInfo;

int init_cap_screen(struct cap_screen_t* sc, int indexOfMonitor = 0)
{
    MonitorMaster::GetAllMonitorInfo();
    if (indexOfMonitor >= vecMonitorListInfo.size())
        indexOfMonitor = 0;
    DEVMODE devmode;
    BOOL bRet;
    BITMAPINFOHEADER bi;
    sc->width = vecMonitorListInfo[indexOfMonitor]->nWidth;
    sc->height = vecMonitorListInfo[indexOfMonitor]->nHeight;
    sc->bitcount = vecMonitorListInfo[indexOfMonitor]->nBits;
    sc->left = vecMonitorListInfo[indexOfMonitor]->area.left;
    sc->top = vecMonitorListInfo[indexOfMonitor]->area.top;
    memset(&bi, 0, sizeof(bi));
    bi.biSize = sizeof(bi);
    bi.biWidth = sc->width;
    bi.biHeight = -sc->height; //从上朝下扫描
    bi.biPlanes = 1;
    bi.biBitCount = sc->bitcount; //RGB
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    HDC hdc = GetDC(NULL); //屏幕DC
    sc->memdc = CreateCompatibleDC(hdc);
    sc->buffer = NULL;
    sc->hbmp = CreateDIBSection(hdc, (BITMAPINFO*)&bi, DIB_RGB_COLORS, (void**)&sc->buffer, NULL, 0);
    ReleaseDC(NULL, hdc);
    SelectObject(sc->memdc, sc->hbmp); ///
    sc->length = sc->height* (((sc->width*sc->bitcount / 8) + 3) / 4 * 4);
    return 0;
}

HCURSOR FetchCursorHandle()
{
    CURSORINFO hCur;
    ZeroMemory(&hCur, sizeof(hCur));
    hCur.cbSize = sizeof(hCur);
    GetCursorInfo(&hCur);
    return hCur.hCursor;
}

void AddCursor(HDC hMemDC, POINT origin)
{

    POINT xPoint;
    GetCursorPos(&xPoint);
    xPoint.x -= origin.x;
    xPoint.y -= origin.y;
    if (xPoint.x < 0 || xPoint.y < 0)
        return;
    HCURSOR hcur = FetchCursorHandle();
    ICONINFO iconinfo;
    BOOL ret;
    ret = GetIconInfo(hcur, &iconinfo);
    if (ret)
    {
        xPoint.x -= iconinfo.xHotspot;
        xPoint.y -= iconinfo.yHotspot;
        if (iconinfo.hbmMask) DeleteObject(iconinfo.hbmMask);
        if (iconinfo.hbmColor) DeleteObject(iconinfo.hbmColor);
    }
    DrawIcon(hMemDC, xPoint.x, xPoint.y, hcur);
}

int blt_cap_screen(struct cap_screen_t* sc)
{
    HDC hdc = GetDC(NULL);
    BitBlt(sc->memdc, 0, 0, sc->width, sc->height, hdc, sc->left, sc->top, SRCCOPY); // 截屏
    AddCursor(sc->memdc, POINT{ sc->left, sc->top }); // 增加鼠标进去
    ReleaseDC(NULL, hdc);
    return 0;
}

//屏幕广播线程入口
void SendScreenVideoDataThread::start(){

    qDebug()<<"SendScreenVideoDataThread start "<<QThread::currentThread();
    Sleep(3000);
    struct cap_screen_t sc;
    BYTE* out;
    AVFrame* frame;

    ffmpeg_init();
    init_cap_screen(&sc, 1);  // 1代表第二块显示器，0代表第一块显示器，依次类推，如果显示器数量不足则为0
    out = (BYTE*)malloc(sc.length);

    YUVencoder enc(sc.width, sc.height);

    // 以下为3个不同的编码器，任选一个即可
    x264Encoder ffmpeg264(sc.width, sc.height,  "save.h264"); // save.h264为本地录屏文件 可以使用vlc播放器播放

    for (int i = 0; ; i++)
    {
        blt_cap_screen(&sc);//20-30ms 我认为可以单独使用一个线程专门来截屏，提高帧率

#if SSDEBUG
         qDebug()<<"blt_cap_screen end "<<QTime::currentTime();
#endif
        frame = enc.encode(sc.buffer, sc.length, sc.width, sc.height, out, sc.length);	//rgb转yuv 8ms
#if SSDEBUG
        qDebug()<<"rgb2yuv end "<<QTime::currentTime();
#endif
        if (frame == NULL)
        {
            printf("Encoder error!!\n"); Sleep(1000); continue;
        }

        // 一下是3种编码方式，任选一种均可
        ffmpeg264.encode(frame);	//h.264视频编码，保存到文件       瓶颈2：网络速率 50-500-1000ms  qt的网络封装导致速度慢？
#if SSDEBUG
        qDebug()<<"teac enc end "<<QTime::currentTime();
#endif
        //QApplication::processEvents();

    }
    ffmpeg264.flush(frame);
    return ;
}
