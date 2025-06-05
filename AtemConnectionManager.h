#include "BMDSwitcherAPI.h"
#include <string>
#include <iostream>
#include <vector>
#include "MqttConnectionManager.h"

class AtemConnectionManager{
public:
    AtemConnectionManager(const char*, MqttConnectionManager*);
private:
    IBMDSwitcher* m_switcher;
    IBMDSwitcherMixEffectBlock *m_mixBlock;
    MqttConnectionManager *m_mqttClient;
    IBMDSwitcherInput *m_input = nullptr;
    IBMDSwitcherInputIterator *m_inputIterator = nullptr;
    std::vector<CFStringRef> m_currentProgramTally;
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
