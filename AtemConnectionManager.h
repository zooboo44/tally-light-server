#include "BMDSwitcherAPI.h"
#include <string>
#include <iostream>
#include "MqttConnectionManager.h"

class AtemConnectionManager{
public:
    AtemConnectionManager(const char*, MqttConnectionManager*);
private:
    IBMDSwitcher* m_switcher;
    IBMDSwitcherMixEffectBlock *m_mixBlock;
    MqttConnectionManager *m_mqttClient;
    
    class AtemInputCallback : public IBMDSwitcherMixEffectBlockCallback{
    public:
        AtemInputCallback(AtemConnectionManager*);
        HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, LPVOID* ppv) override;
        ULONG STDMETHODCALLTYPE AddRef() override;
        ULONG STDMETHODCALLTYPE Release() override;
        virtual HRESULT STDMETHODCALLTYPE Notify(BMDSwitcherMixEffectBlockEventType eventType) override;
    private:
        ULONG refCount;
        AtemConnectionManager *parentManager;
        void logProgramInput();
    };
};
