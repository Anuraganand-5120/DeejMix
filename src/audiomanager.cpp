#include "audiomanager.h"
#include <QDebug>

#ifdef Q_OS_WIN
#include <windows.h>
#include <mmdeviceapi.h>
#include <audiopolicy.h>
#include <endpointvolume.h>
#include <math.h>
#include <psapi.h>

static QString getProcessName(DWORD pid) {
    QString name;
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (hProcess) {
        WCHAR buffer[MAX_PATH];
        if (GetModuleFileNameExW(hProcess, NULL, buffer, MAX_PATH)) {
            name = QString::fromWCharArray(buffer).section('\\', -1).toLower();
        }
        CloseHandle(hProcess);
    }
    return name;
}

// Helper to convert linear 0.0-1.0 to a logarithmic scale
float linearToLogarithmic(float linearValue) {
    // A simple exponential curve matching human hearing
    // volume = (e^(alpha * x) - 1) / (e^alpha - 1)
    if (linearValue <= 0.0f) return 0.0f;
    if (linearValue >= 1.0f) return 1.0f;
    
    const float alpha = 6.908f; // ln(1000) for a 60dB dynamic range
    return (exp(alpha * linearValue) - 1.0f) / (exp(alpha) - 1.0f);
}

#endif

AudioManager::AudioManager(QObject *parent) : QObject(parent), m_initialized(false)
{
}

AudioManager::~AudioManager()
{
#ifdef Q_OS_WIN
    if (m_initialized) {
        CoUninitialize();
    }
#endif
}

bool AudioManager::initialize()
{
#ifdef Q_OS_WIN
    // Qt initializes COM as STA (COINIT_APARTMENTTHREADED). Using MULTITHREADED causes RPC_E_CHANGED_MODE.
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (FAILED(hr) && hr != RPC_E_CHANGED_MODE && hr != (HRESULT)0x80010106L) {
        qDebug() << "Failed to initialize COM library" << hr;
        return false;
    }
    m_initialized = true;
    return true;
#else
    return false;
#endif
}

void AudioManager::setAppVolume(const QString& appName, float volume, bool useLogarithmic)
{
    if (!m_initialized) return;

#ifdef Q_OS_WIN
    float finalVolume = useLogarithmic ? linearToLogarithmic(volume) : volume;
    
    IMMDeviceEnumerator *deviceEnumerator = NULL;
    HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (LPVOID *)&deviceEnumerator);
    if (FAILED(hr) || !deviceEnumerator) return;

    IMMDevice *defaultDevice = NULL;
    hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
    deviceEnumerator->Release();
    if (FAILED(hr) || !defaultDevice) return;

    IAudioSessionManager2 *sessionManager = NULL;
    hr = defaultDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, NULL, (void**)&sessionManager);
    defaultDevice->Release();
    if (FAILED(hr) || !sessionManager) return;

    IAudioSessionEnumerator *sessionEnumerator = NULL;
    hr = sessionManager->GetSessionEnumerator(&sessionEnumerator);
    sessionManager->Release();
    if (FAILED(hr) || !sessionEnumerator) return;

    int count;
    hr = sessionEnumerator->GetCount(&count);
    if (SUCCEEDED(hr)) {
        QString targetName = appName.toLower();
        for (int i = 0; i < count; i++) {
            IAudioSessionControl *sessionControl = NULL;
            if (SUCCEEDED(sessionEnumerator->GetSession(i, &sessionControl)) && sessionControl) {
                IAudioSessionControl2 *sessionControl2 = NULL;
                if (SUCCEEDED(sessionControl->QueryInterface(__uuidof(IAudioSessionControl2), (void**)&sessionControl2)) && sessionControl2) {
                    DWORD pid = 0;
                    sessionControl2->GetProcessId(&pid);
                    if (pid > 0 && getProcessName(pid) == targetName) {
                        ISimpleAudioVolume *simpleAudioVolume = NULL;
                        if (SUCCEEDED(sessionControl->QueryInterface(__uuidof(ISimpleAudioVolume), (void**)&simpleAudioVolume)) && simpleAudioVolume) {
                            simpleAudioVolume->SetMasterVolume(finalVolume, NULL);
                            simpleAudioVolume->Release();
                        }
                    }
                    sessionControl2->Release();
                }
                sessionControl->Release();
            }
        }
    }
    sessionEnumerator->Release();
#endif
}

void AudioManager::setMasterVolume(float volume, bool useLogarithmic)
{
    if (!m_initialized) return;

#ifdef Q_OS_WIN
    float finalVolume = useLogarithmic ? linearToLogarithmic(volume) : volume;
    
    IMMDeviceEnumerator *deviceEnumerator = NULL;
    HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (LPVOID *)&deviceEnumerator);
    if (FAILED(hr) || !deviceEnumerator) return;
    
    IMMDevice *defaultDevice = NULL;
    hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
    if (FAILED(hr) || !defaultDevice) {
        deviceEnumerator->Release();
        return;
    }
    
    IAudioEndpointVolume *endpointVolume = NULL;
    hr = defaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void**)&endpointVolume);
    if (FAILED(hr) || !endpointVolume) {
        defaultDevice->Release();
        deviceEnumerator->Release();
        return;
    }
    
    endpointVolume->SetMasterVolumeLevelScalar(finalVolume, NULL);
    
    endpointVolume->Release();
    defaultDevice->Release();
    deviceEnumerator->Release();
#endif
}

void AudioManager::setMicrophoneVolume(float volume, bool useLogarithmic)
{
    if (!m_initialized) return;

#ifdef Q_OS_WIN
    float finalVolume = useLogarithmic ? linearToLogarithmic(volume) : volume;
    
    IMMDeviceEnumerator *deviceEnumerator = NULL;
    HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (LPVOID *)&deviceEnumerator);
    if (FAILED(hr) || !deviceEnumerator) return;
    
    IMMDevice *defaultDevice = NULL;
    hr = deviceEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &defaultDevice);
    if (FAILED(hr) || !defaultDevice) {
        deviceEnumerator->Release();
        return;
    }
    
    IAudioEndpointVolume *endpointVolume = NULL;
    hr = defaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void**)&endpointVolume);
    if (FAILED(hr) || !endpointVolume) {
        defaultDevice->Release();
        deviceEnumerator->Release();
        return;
    }
    
    endpointVolume->SetMasterVolumeLevelScalar(finalVolume, NULL);
    
    endpointVolume->Release();
    defaultDevice->Release();
    deviceEnumerator->Release();
#endif
}

QStringList AudioManager::getActiveAudioSessions()
{
    QStringList apps;
    if (!m_initialized) return apps;

#ifdef Q_OS_WIN
    IMMDeviceEnumerator *deviceEnumerator = NULL;
    HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (LPVOID *)&deviceEnumerator);
    if (FAILED(hr) || !deviceEnumerator) return apps;

    IMMDevice *defaultDevice = NULL;
    hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
    deviceEnumerator->Release();
    if (FAILED(hr) || !defaultDevice) return apps;

    IAudioSessionManager2 *sessionManager = NULL;
    hr = defaultDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, NULL, (void**)&sessionManager);
    defaultDevice->Release();
    if (FAILED(hr) || !sessionManager) return apps;

    IAudioSessionEnumerator *sessionEnumerator = NULL;
    hr = sessionManager->GetSessionEnumerator(&sessionEnumerator);
    sessionManager->Release();
    if (FAILED(hr) || !sessionEnumerator) return apps;

    int count;
    hr = sessionEnumerator->GetCount(&count);
    if (SUCCEEDED(hr)) {
        for (int i = 0; i < count; i++) {
            IAudioSessionControl *sessionControl = NULL;
            if (SUCCEEDED(sessionEnumerator->GetSession(i, &sessionControl)) && sessionControl) {
                IAudioSessionControl2 *sessionControl2 = NULL;
                if (SUCCEEDED(sessionControl->QueryInterface(__uuidof(IAudioSessionControl2), (void**)&sessionControl2)) && sessionControl2) {
                    DWORD pid = 0;
                    sessionControl2->GetProcessId(&pid);
                    if (pid > 0) {
                        QString procName = getProcessName(pid);
                        if (!procName.isEmpty() && !apps.contains(procName)) {
                            apps.append(procName);
                        }
                    }
                    sessionControl2->Release();
                }
                sessionControl->Release();
            }
        }
    }
    sessionEnumerator->Release();
    apps.sort(Qt::CaseInsensitive);
#endif
    
    return apps;
}
