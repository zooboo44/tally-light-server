#include "BMDSwitcherAPI.h"
#include <string>
#include <iostream>
#include <unordered_map>
#include "MqttConnectionManager.h"

class AtemConnectionManager{
public:
    AtemConnectionManager(const char*, MqttConnectionManager*);
    HRESULT atemConnect(const char*);
private:
    IBMDSwitcher* m_switcher;
    IBMDSwitcherMixEffectBlock *m_mixBlock;
    MqttConnectionManager *m_mqttClient;
    IBMDSwitcherDiscovery* switcherDiscovery;
    BMDSwitcherConnectToFailure connectionFailureReason;
    std::unordered_map<std::string, std::string> m_currentProgram;
    std::string getLongName(IBMDSwitcherInput *);
    
    class AtemInputCallback : public IBMDSwitcherMixEffectBlockCallback{
    public:
        AtemInputCallback(AtemConnectionManager*);
        HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, LPVOID* ppv) override;
        ULONG STDMETHODCALLTYPE AddRef() override;
        ULONG STDMETHODCALLTYPE Release() override;
        virtual HRESULT STDMETHODCALLTYPE Notify(BMDSwitcherMixEffectBlockEventType eventType) override;
    private:
        ULONG refCount;
        AtemConnectionManager *m_parentManager;
    };
    
    class AtemSwitcherCallback : public IBMDSwitcherCallback{
    public:
        AtemSwitcherCallback();
        HRESULT STDMETHODCALLTYPE QueryInterface (REFIID iid, LPVOID* ppv) override;
        ULONG STDMETHODCALLTYPE AddRef() override;
        ULONG STDMETHODCALLTYPE Release() override;
        virtual HRESULT STDMETHODCALLTYPE Notify(BMDSwitcherEventType eventType, BMDSwitcherVideoMode coreVideoMode) override;
    private:
        ULONG refCount;
        AtemConnectionManager *m_parentManager;
    };
};
