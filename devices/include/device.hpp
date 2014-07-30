

#ifndef __VSC_DEVICE_H_
#define __VSC_DEVICE_H_

#define NOMINMAX
#ifdef WIN32
#include <Windows.h>
#include <Winbase.h>
#endif 
//#include "ffmpeg_impl.hpp"
#include "utility.hpp"
#include "conf.hpp"
#include "datasink.hpp"
#include "devicemanagement.h"
#include "ptzmanagement.h"
#include "media_management/profiles.h"
#include "media_management/streamuri.h"
#include "device_management/capabilities.h"
#include "mediamanagement.h"
#include "vdb.hpp"
#include "vplay.hpp"
#include "debug.hpp"



using namespace UtilityLib;
using namespace std;
using namespace ONVIF;

typedef enum
{
    F_PTZ_UP = 1,
    F_PTZ_DOWN,
    F_PTZ_LEFT,
    F_PTZ_RIGHT,
    F_PTZ_ZOOM_IN,
    F_PTZ_ZOOM_OUT,
    F_PTZ_STOP,
    F_PTZ_LAST
} FPtzAction;

typedef enum
{
    DEV_OFF2ON = 1,
    DEV_ON2OFF,
    DEV_NO_CHANGE,
    DEV_LAST
} DeviceStatus;


class CvCapture_FFMPEG;
typedef std::list<NotificationQueue*> CmdQueueList;
//typedef UtilityListIterator<NotificationQueue> CmdQueueIterator;
#ifdef WIN32
typedef void (__cdecl * DeviceDeleteCallbackFunctionPtr)(u32 nId, void * pParam);
#else
typedef void ( * DeviceDeleteCallbackFunctionPtr)(u32 nId, void * pParam);
#endif
typedef std::map<void *, DeviceDeleteCallbackFunctionPtr> DeviceDeleteCallbackMap;

class DeviceParam
{
public:
    DeviceParam();
    DeviceParam(const DeviceParam &pParam);
    DeviceParam(VSCDeviceData &pData);
    ~DeviceParam();
    DeviceParam & operator=(const DeviceParam &pParam)
    {
        memset(&m_Conf, 0, sizeof(m_Conf));

        memcpy(&m_Conf, &(pParam.m_Conf), sizeof(m_Conf));
        m_strUrl = pParam.m_strUrl;
        return *this;
    }

public:
    BOOL UpdateUrl();
    BOOL UpdateUrlOnvif();
    BOOL CheckOnline();

public:
    VSCDeviceData m_Conf;
    BOOL m_bOnvifUrlGetted;
    astring m_strUrl;
};

class Device
{
public:
    Device(VDB &pVdb, const DeviceParam &pParam);
    ~Device();
	
public:
    BOOL Start();
    BOOL Stop();
    BOOL StartRecord();
    BOOL StopRecord();
    BOOL SetRecord(BOOL bRecording);
    DeviceStatus CheckDevice();
	
public:
    static void Run(void * pParam);
    void Run1();
    static BOOL DataHandler(void* pData, VideoFrame& frame);
    BOOL DataHandler1(VideoFrame& frame);

public:
    void Lock(){m_Lock.lock();}
    void UnLock(){m_Lock.unlock();}
    void LockTime(){m_DataTimeLock.lock();}
    void UnLockTime(){m_DataTimeLock.unlock();}

public:
    NotificationQueue * GetRawDataQueue();
    BOOL ReleaseRawDataQueue(NotificationQueue * pQueue);
    NotificationQueue * GetDataQueue();
    BOOL GetDataQueue(NotificationQueue * pQueue);
    BOOL RegDeleteCallback(DeviceDeleteCallbackFunctionPtr pCallback, void * pParam);
    BOOL UnRegDeleteCallback(void * pParam);
    BOOL Cleanup();
    BOOL GetDeviceOnline()
    {
        BOOL online = true;
        return m_Online;
    }
	
    BOOL GetUrl(std::string &url)
    {
        url = m_param.m_strUrl;
        return TRUE;
    }
    BOOL GetDeviceRtspUrl(astring & strUrl)
    {
    	if (m_OnlineUrl == FALSE)
    	{
    		return FALSE;
    	}
	strUrl = m_param.m_strUrl;
	return TRUE;
    }
    BOOL AttachPlayer(HWND hWnd, int w, int h);
    BOOL UpdateWidget(HWND hWnd, int w, int h);
    BOOL DetachPlayer(HWND hWnd);
	BOOL DrawPtzDirection(HWND hWnd, int x1, int y1, int x2,  int y2);
	BOOL ClearPtzDirection(HWND hWnd);
	BOOL ShowAlarm(HWND hWnd);
	BOOL PtzAction(FPtzAction action, float speed);
	BOOL UpdatePTZConf();

public:
    BOOL WriteFrame(unsigned char* dataRGB, int step, int width, int height, int cn,
            unsigned char* data, int size);

private:
    CmdQueueList m_RawDataList;
    CmdQueueList m_DataList;
    BOOL m_bStarted;
    DeviceDeleteCallbackMap m_DeleteMap;

    VPlay m_vPlay;

private:
    s32 m_nDeviceType;
    s32 m_nDeviceSubType;
    DeviceParam m_param;
    tthread::thread *m_pThread;
    CvCapture_FFMPEG* m_Cap;
    fast_mutex m_Lock;
    s32 m_nLastGetDataTime;
    fast_mutex m_DataTimeLock;
private:
    VDB &m_pVdb;
    RecordSession *m_pRecord;
    BOOL m_Online;
    BOOL m_OnlineUrl;

private:
    ContinuousMove m_continuousMove;
    ONVIF::Stop m_stop;
    PtzManagement *m_ptz;
    BOOL m_ptzInited;
};

typedef DeviceParam* LPDeviceParam;
typedef Device* LPDevice;

inline string GetProgramDir()
{
#ifdef WIN32
    char exeFullPath[MAX_PATH]; // Full path

    string strPath = "";

    GetModuleFileNameA(NULL,exeFullPath, MAX_PATH);
    strPath=(string)exeFullPath;    // Get full path of the file

    int pos = strPath.find_last_of('\\', strPath.length());

    return strPath.substr(0, pos);  // Return the directory without the file name
#else
    return "ve/";
#endif
}


inline DeviceParam::DeviceParam()
{
    static int CameraNum = 0;
    memset(&m_Conf, 0, sizeof(m_Conf));
    m_Conf.data.conf.nId = 0;
    m_Conf.data.conf.nType = VSC_DEVICE_CAM;
    m_Conf.data.conf.nSubType = VSC_SUB_DEVICE_ONVIF;

    sprintf(m_Conf.data.conf.Name, "Camera");
    //strcpy(m_Conf.data.conf.Name, "CAMERA ");

    strcpy(m_Conf.data.conf.IP, "192.168.0.1");
    strcpy(m_Conf.data.conf.Port, "80");
    strcpy(m_Conf.data.conf.User, "admin");
    strcpy(m_Conf.data.conf.Password, "admin");

    strcpy(m_Conf.data.conf.RtspLocation, "/");
    string filePath = GetProgramDir() +  "/camera.mov";
    strcpy(m_Conf.data.conf.FileLocation, filePath.c_str());
    strcpy(m_Conf.data.conf.OnvifAddress, "/onvif/device_service");
    strcpy(m_Conf.data.conf.CameraIndex, "1");

    m_Conf.data.conf.UseProfileToken = 0;
    //m_Conf.data.conf.Recording = 1;//Default start recording
    m_Conf.data.conf.Recording = 0;//Default start recording//xsmart
    strcpy(m_Conf.data.conf.OnvifProfileToken, "quality_h264");
    m_bOnvifUrlGetted = FALSE;

}

inline DeviceParam::DeviceParam(VSCDeviceData &pData)
{
    memset(&m_Conf, 0, sizeof(m_Conf));

    memcpy(&m_Conf, &(pData), sizeof(m_Conf));
    //UpdateUrl();
}

inline std::string Replace(std::string &str, const char *string_to_replace, const char *new_string)
{
  // Find the first string to replace
  int index = str.find(string_to_replace);
  // while there is one
  while(index != std::string::npos)
  {
    // Replace it
    str.replace(index, strlen(string_to_replace), new_string);
    // Find the next one
    index = str.find(string_to_replace, index + strlen(new_string));
  }
  return str;
}

inline BOOL DeviceParam::CheckOnline()
{
    astring IP = m_Conf.data.conf.IP;
    astring Port = m_Conf.data.conf.Port;
    astring User = m_Conf.data.conf.User;
    astring Password = m_Conf.data.conf.Password;
    astring OnvifAddress = m_Conf.data.conf.OnvifAddress;

    astring OnvifDeviceService = "http://" + IP + ":" + Port + OnvifAddress;
	astring url = "rtsp://" + IP + ":" + "554" + "/";

    DeviceManagement *pDm = new DeviceManagement(OnvifDeviceService.c_str(), 
                            User.c_str(), Password.c_str());
    
    if (pDm  == NULL)
    {
        VDC_DEBUG( "%s new DeviceManagement error \n",__FUNCTION__);
        return FALSE;
    }
    
    Capabilities * pMediaCap = pDm->getCapabilitiesMedia();
    if (pMediaCap == NULL)
    {
        VDC_DEBUG( "%s getCapabilitiesMedia error \n",__FUNCTION__);
        delete pDm;
        return FALSE;
    }

    return TRUE;
}

inline BOOL DeviceParam::UpdateUrlOnvif()
{
    astring IP = m_Conf.data.conf.IP;
    astring Port = m_Conf.data.conf.Port;
    astring User = m_Conf.data.conf.User;
    astring Password = m_Conf.data.conf.Password;
    astring OnvifAddress = m_Conf.data.conf.OnvifAddress;

    astring OnvifDeviceService = "http://" + IP + ":" + Port + OnvifAddress;
	astring url = "rtsp://" + IP + ":" + "554" + "/";

    DeviceManagement *pDm = new DeviceManagement(OnvifDeviceService.c_str(), 
                            User.c_str(), Password.c_str());
    
    if (pDm  == NULL)
    {
        VDC_DEBUG( "%s new DeviceManagement error \n",__FUNCTION__);
        return FALSE;
    }

    Capabilities * pMediaCap = pDm->getCapabilitiesMedia();
    if (pMediaCap == NULL)
    {
        VDC_DEBUG( "%s getCapabilitiesMedia error \n",__FUNCTION__);
        delete pDm;
        return FALSE;
    }

    MediaManagement *pMedia = new MediaManagement(pMediaCap->mediaXAddr(), 
                                User.c_str(), Password.c_str());
    if (pMedia == NULL)
    {
        VDC_DEBUG( "%s new MediaManagement error \n",__FUNCTION__);
        delete pDm;
        delete pMediaCap;
        return FALSE;
    }

    Profiles *pProfileS = pMedia->getProfiles();
    if (pProfileS == NULL)
    {
        VDC_DEBUG( "%s new getProfiles error \n",__FUNCTION__);
        delete pDm;
        delete pMediaCap;
        delete pMedia;
        return FALSE;
    }
    if (pProfileS->m_toKenPro.size() > 0)
    {
        VDC_DEBUG( "%s m_toKenPro size %d \n",__FUNCTION__, pProfileS->m_toKenPro.size());
		QString strToken;
		if (m_Conf.data.conf.UseProfileToken == 1)
		{
			strToken = m_Conf.data.conf.OnvifProfileToken;
		}else
		{
			strToken = pProfileS->m_toKenPro[0];
			strcpy(m_Conf.data.conf.OnvifProfileToken, pProfileS->m_toKenPro[0].toStdString().c_str());
		}
		StreamUri *pUri = pMedia->getStreamUri(strToken);
		if (pUri)
		{
			url = pUri->uri().toStdString();
			delete pUri;
		}
        
    }
	std::string strUrl = url;
	
	/* rtsp://admin:12345@192.168.1.1:554/Streaming/Channels/1\
	?transportmode=unicast&profile=Profile_1 */
	astring urlWithUser = "rtsp://" + User + ":" + Password + "@";
	//Replace(strUrl, "rtsp://", urlWithUser.c_str());

	m_strUrl = strUrl;
     m_bOnvifUrlGetted = TRUE;

    delete pDm;
    delete pMediaCap;
    delete pMedia;
    delete pProfileS;
    return TRUE;
}

inline BOOL DeviceParam::UpdateUrl()
{
    //TODO RTSP ONVIF call onvif sdk to get a Stream URL
    if (m_Conf.data.conf.nSubType == VSC_SUB_DEVICE_FILE)
    {
        string filePre = "file:///";
        m_strUrl = filePre + m_Conf.data.conf.FileLocation;
    }

    if (m_Conf.data.conf.nSubType == VSC_SUB_DEVICE_RTSP)
    {
        char url[512];
        sprintf(url, "rtsp://%s:%s%s",
                m_Conf.data.conf.IP,
                m_Conf.data.conf.Port, m_Conf.data.conf.RtspLocation);
        m_strUrl = url;

    }

    if (m_Conf.data.conf.nSubType == VSC_SUB_DEVICE_ONVIF)
    {
        return UpdateUrlOnvif();
    }
    VDC_DEBUG( "%s url %s\n",__FUNCTION__, m_strUrl.c_str());
    return TRUE;
}

inline DeviceParam::DeviceParam(const DeviceParam &pParam)
{
    memset(&m_Conf, 0, sizeof(m_Conf));

    memcpy(&m_Conf, &(pParam.m_Conf), sizeof(m_Conf));
}


inline DeviceParam::~DeviceParam()
{
}

inline Device::Device(VDB &pVdb, const DeviceParam &pParam)
:m_bStarted(FALSE), m_param(pParam), m_nLastGetDataTime(0), 
m_pVdb(pVdb), m_pRecord(NULL), m_Online(FALSE), m_OnlineUrl(FALSE), m_ptzInited(FALSE), 
m_ptz(NULL)
{
    
    if (strcmp(pParam.m_Conf.data.conf.Name, "Camera") == 0)
      sprintf((char *)pParam.m_Conf.data.conf.Name, "Camera %d", pParam.m_Conf.data.conf.nId);
    m_param = pParam;
    return ;
    /* Always return, just let factory thread to update  */
    if (m_param.UpdateUrl() == FALSE)
    {
        return;
    }

    m_vPlay.Init(TRUE, m_param.m_strUrl, m_param.m_Conf.data.conf.User,
			m_param.m_Conf.data.conf.Password);
    VDC_DEBUG( "%s url %s\n",__FUNCTION__, m_param.m_strUrl.c_str());

    StartRecord();
    m_Online = TRUE;
    m_OnlineUrl = TRUE;
    UpdatePTZConf();
}

inline Device::~Device()
{
}

inline DeviceStatus Device::CheckDevice()
{
    if (m_param.CheckOnline() == TRUE)
    {
        /* Camera from offline to online */
        if (m_OnlineUrl == FALSE)
        {
            if (m_param.UpdateUrl() == FALSE)
            {
                return  DEV_NO_CHANGE;
            }
            m_vPlay.Init(TRUE, m_param.m_strUrl, m_param.m_Conf.data.conf.User,
			m_param.m_Conf.data.conf.Password);
            VDC_DEBUG( "%s url %s\n",__FUNCTION__, m_param.m_strUrl.c_str());
			
            StartRecord();
            m_OnlineUrl = TRUE;
            UpdatePTZConf();
        }
        if (m_Online == FALSE)
        {
            m_Online = TRUE;
            return DEV_OFF2ON;
        }
    }else
    {
        if (m_Online == TRUE)
        {
            m_Online = FALSE;
            return DEV_ON2OFF;
        }
    }

    return DEV_NO_CHANGE;
    
}
    
inline NotificationQueue * Device::GetRawDataQueue()
{
    NotificationQueue *pQueue = NULL;

    Lock();
    pQueue = new NotificationQueue;

    m_RawDataList.push_back(pQueue);
    UnLock();

    return pQueue;

}
inline BOOL Device::ReleaseRawDataQueue(NotificationQueue * pQueue)
{
    if (pQueue == NULL)
    {
        return FALSE;
    }

    Lock();
	
    m_RawDataList.remove(pQueue);

    delete pQueue;

    UnLock();
    return TRUE;
}

inline BOOL Device::AttachPlayer(HWND hWnd, int w, int h)
{
    m_vPlay.AttachWidget(hWnd, w, h);

    return TRUE;
}

inline BOOL Device::UpdateWidget(HWND hWnd, int w, int h)
{
    m_vPlay.UpdateWidget(hWnd, w, h);

    return TRUE;
}

inline BOOL Device::DetachPlayer(HWND hWnd)
{
    m_vPlay.DetachWidget(hWnd);
    
    return TRUE;
}

inline BOOL Device::DrawPtzDirection(HWND hWnd, int x1, int y1, int x2,  int y2)
{
	m_vPlay.DrawPtzDirection(hWnd, x1, y1, x2, y2);
	return TRUE;
}
inline BOOL Device::ClearPtzDirection(HWND hWnd)
{
	m_vPlay.ClearPtzDirection(hWnd);
	return TRUE;
}

inline BOOL Device::ShowAlarm(HWND hWnd)
{
	m_vPlay.ShowAlarm(hWnd);
	return TRUE;
}

inline BOOL Device::UpdatePTZConf()
{
    QString strToken;
    astring IP = m_param.m_Conf.data.conf.IP;
    astring Port = m_param.m_Conf.data.conf.Port;
    astring User = m_param.m_Conf.data.conf.User;
    astring Password = m_param.m_Conf.data.conf.Password;
    astring OnvifAddress = m_param.m_Conf.data.conf.OnvifAddress;

    astring OnvifDeviceService = "http://" + IP + ":" + Port + OnvifAddress;

    DeviceManagement *pDm = new DeviceManagement(OnvifDeviceService.c_str(), 
                            User.c_str(), Password.c_str());
    
    if (pDm  == NULL)
    {
        VDC_DEBUG( "%s new DeviceManagement error \n",__FUNCTION__);
        return FALSE;
    }

    Capabilities * pMediaCap = pDm->getCapabilitiesMedia();
    if (pMediaCap == NULL)
    {
        VDC_DEBUG( "%s getCapabilitiesPtz error \n",__FUNCTION__);
        delete pDm;
        return FALSE;
    }

    MediaManagement *pMedia = new MediaManagement(pMediaCap->mediaXAddr(), 
                                User.c_str(), Password.c_str());
    if (pMedia == NULL)
    {
        VDC_DEBUG( "%s new MediaManagement error \n",__FUNCTION__);
        delete pDm;
        delete pMediaCap;
        return FALSE;
    }

    Profiles *pProfileS = pMedia->getProfiles();
    if (pProfileS == NULL)
    {
        VDC_DEBUG( "%s new getProfiles error \n",__FUNCTION__);
        delete pDm;
        delete pMediaCap;
        delete pMedia;
        return FALSE;
    }
    if (pProfileS->m_toKenPro.size() > 0)
    {
        	VDC_DEBUG( "%s m_toKenPro size %d \n",__FUNCTION__, pProfileS->m_toKenPro.size());
		
		if (m_param.m_Conf.data.conf.UseProfileToken == 1)
		{
			strToken = m_param.m_Conf.data.conf.OnvifProfileToken;
		}else
		{
			strToken = pProfileS->m_toKenPro[0];		
		}
        
    }else
    {
    	return FALSE;
    }

    Capabilities * pPtz = pDm->getCapabilitiesPtz();
    if (pPtz == NULL)
    {
        VDC_DEBUG( "%s getCapabilitiesPtz error \n",__FUNCTION__);
        delete pDm;
        delete pMediaCap;
        delete pMedia;
        return FALSE;
    }

    string strPtz = pPtz->ptzXAddr().toStdString();
    m_ptz  = new PtzManagement(pPtz->ptzXAddr(), 
                                User.c_str(), Password.c_str());
    if (m_ptz == NULL)
    {
        VDC_DEBUG( "%s getCapabilitiesPtz error \n",__FUNCTION__);
        delete pDm;
        delete pMediaCap;
        delete pMedia;
		delete pPtz;
        return FALSE;
    }

    m_continuousMove.setProfileToken(strToken);
    m_stop.setProfileToken(strToken);
    m_ptzInited = TRUE;
}

inline BOOL Device::PtzAction(FPtzAction action, float speed)
{
	if (m_Online == FALSE || m_ptzInited == FALSE)
	{
		VDC_DEBUG( "%s PTZ Camera is Offline\n",__FUNCTION__);
		return TRUE;
	}
	switch (action)
	{
		case F_PTZ_UP:	
		{
			m_continuousMove.setPanTiltX(0);
			m_continuousMove.setPanTiltY(speed);
			m_continuousMove.setZoomX(0);
			m_continuousMove.setTimeout("PT0H0M3.600S");
			m_ptz->continuousMove(&m_continuousMove);
			break;
		}
		case F_PTZ_DOWN:	
		{
			m_continuousMove.setPanTiltX(0);
			m_continuousMove.setPanTiltY(0 - speed);
			m_continuousMove.setZoomX(0);
			m_continuousMove.setTimeout("PT0H0M3.600S");
			m_ptz->continuousMove(&m_continuousMove);
			break;
		}
		case F_PTZ_LEFT:	
		{
			m_continuousMove.setPanTiltX(0 - speed);
			m_continuousMove.setPanTiltY(0);
			m_continuousMove.setZoomX(0);
			m_continuousMove.setTimeout("PT0H0M3.600S");
			m_ptz->continuousMove(&m_continuousMove);
			break;
		}
		case F_PTZ_RIGHT:	
		{
			m_continuousMove.setPanTiltX(speed);
			m_continuousMove.setPanTiltY(0);
			m_continuousMove.setZoomX(0);
			m_continuousMove.setTimeout("PT0H0M3.600S");
			m_ptz->continuousMove(&m_continuousMove);
			break;
		}
		case F_PTZ_ZOOM_IN:	
		{
			m_continuousMove.setPanTiltX(0);
			m_continuousMove.setPanTiltY(0);
			m_continuousMove.setZoomX(speed);
			m_continuousMove.setTimeout("PT0H0M3.600S");
			m_ptz->continuousMove(&m_continuousMove);
			break;
		}
		case F_PTZ_ZOOM_OUT:	
		{
			m_continuousMove.setPanTiltX(0);
			m_continuousMove.setPanTiltY(0);
			m_continuousMove.setZoomX(0 - speed);
			m_continuousMove.setTimeout("PT0H0M3.600S");
			m_ptz->continuousMove(&m_continuousMove);
			break;
		}
		case F_PTZ_STOP:	
		{
			m_stop.setPanTilt(true);
			m_stop.setZoom(false);
			m_ptz->stop(&m_stop);
			break;
		}
		default:
			break;
	}
}

inline BOOL Device::RegDeleteCallback(DeviceDeleteCallbackFunctionPtr pCallback, void * pParam)
{
    m_DeleteMap[pParam] = pCallback;
    return TRUE;
}

inline BOOL Device::UnRegDeleteCallback(void * pParam)
{
    m_DeleteMap.erase(pParam);
    return TRUE;
}

inline NotificationQueue * Device::GetDataQueue()
{
    return NULL;
}
inline BOOL Device::GetDataQueue(NotificationQueue * pQueue)
{
    return TRUE;
}

inline BOOL Device::Start()
{

    return TRUE;
}
inline BOOL Device::Stop()
{


    return TRUE;
}

inline BOOL Device::SetRecord(BOOL bRecording)
{
    if (bRecording == TRUE)
    {
        m_param.m_Conf.data.conf.Recording = 1;
    }else
    {
        m_param.m_Conf.data.conf.Recording = 0;
    }

    return TRUE;
}

inline BOOL Device::StartRecord()
{
    if (m_param.m_Conf.data.conf.Recording == 0)
    {
        return FALSE;
    }
    VDC_DEBUG( "%s Start Record\n",__FUNCTION__);
    m_vPlay.StartGetData(this, (VPlayDataHandler)Device::DataHandler);

    return TRUE;
}
inline BOOL Device::StopRecord()
{
    if (m_param.m_Conf.data.conf.Recording == 1)
    {
        return FALSE;
    }
    VDC_DEBUG( "%s Stop Record\n",__FUNCTION__);
    m_vPlay.StopGetData();
    if (m_pRecord)
    {
        u32 endTime = m_pRecord->GetEndTime();
        if (endTime != 0)
        {
        	 m_pVdb.FinishRecord(m_pRecord);
        }
        delete m_pRecord;
        m_pRecord = NULL;
    }

    return TRUE;
}

 inline BOOL Device::DataHandler(void* pData, VideoFrame& frame)
{
    int dummy = errno;
    LPDevice pThread = (LPDevice)pData;

    if (pThread)
    {
        return pThread->DataHandler1(frame);
    }
}

 inline BOOL Device::DataHandler1(VideoFrame& frame)
{

    if (m_pRecord == NULL)
    {
        m_pRecord = m_pVdb.StartRecord(m_param.m_Conf.data.conf.nId, (int)(frame.secs), R_MANUAL);
        if (m_pRecord == NULL)
        {
            return TRUE;
        }
    }
    // VDC_DEBUG("Recording Size %d stream %d frame %d (%d, %d)\n", frame.dataLen,      
	// 	frame.streamType, frame.frameType, frame.secs, frame.msecs);
    if (m_pRecord->PushAFrame(&frame) == MF_WRTIE_REACH_END)
    {
    	u32 endTime = m_pRecord->GetEndTime();
    	if (endTime != 0)
    	{
    		m_pVdb.FinishRecord(m_pRecord);
    	}
        delete m_pRecord;
        m_pRecord = m_pVdb.StartRecord(m_param.m_Conf.data.conf.nId, (int)(frame.secs), 1);
    	if (m_pRecord == NULL)
    	{
    		return TRUE;
    	}
        m_pRecord->PushAFrame(&frame);	 
    }

    return TRUE;
}

inline BOOL Device::Cleanup()
{

    VDC_DEBUG( "%s Callback begin\n",__FUNCTION__);
    /* Call the callbacks for this device */
    DeviceDeleteCallbackMap::iterator it = m_DeleteMap.begin();

    for(; it!=m_DeleteMap.end(); ++it)
    {
        void *pParam = (*it).first;
        DeviceDeleteCallbackFunctionPtr pFunc = (*it).second;
        if (pFunc)
        {
            pFunc(m_param.m_Conf.data.conf.nId, pParam);
        }
    }
    VDC_DEBUG( "%s Callback end\n",__FUNCTION__);

    Stop();

    return TRUE;
}

inline void Device::Run(void * pParam)
{

}

inline BOOL Device::WriteFrame(unsigned char* dataRGB, int step, int width, int height, int cn,
        unsigned char* data, int size)
{


    return TRUE;
}


#endif // __VSC_DEVICE_H_
