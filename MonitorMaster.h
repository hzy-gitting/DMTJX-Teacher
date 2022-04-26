#include <atlstr.h>
#include <vector>
#include <WinDef.h>
#include <tchar.h>

namespace MonitorMaster
{
	
	// ��ʾ����Ϣ
	struct MonitorInfo
	{
		WCHAR       szDevice[CCHDEVICENAME];				// ��ʾ������
		RECT area; // ��ʾ����������
		DWORD nWidth, nHeight, nFreq, nBits;
	};

	typedef std::vector<MonitorInfo*> VEC_MONITOR_INFO;  // ���е���ʾ����Ϣ	
	

	// ö����ʾ���ص�����
	BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor,
		HDC hdc,
		LPRECT lpRMonitor,
		LPARAM dwData);

	// �õ�������ʾ������Ϣ
	void GetAllMonitorInfo();

	//�õ���Ļ��ǰ�ֱ���
	void GetCurrentReselotion(int& nWidth, int& nHeight, int& nFreq, int& nBits);

	//������ĻIDȡ��ȡ��Ļ�Ķ�Ӧ�ֱ���
	void GetCurrentReselotion(LPCWSTR lpszDeviceName, int& nWidth, int& nHeight, int& nFreq, int& nBits);

	int EnumMonitorInfo();

	//�޸ķֱ���
	int ChangMonitorReselotion(HMONITOR hMonitor, const int nWidth, const int nHight, const int nFre, const int nColorBits);
};