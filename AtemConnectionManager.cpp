#include "AtemConnectionManager.h"

AtemConnectionManager::AtemConnectionManager(const char* ipAddress, MqttConnectionManager* mqtt):m_mqttClient(mqtt){
    //connect to mqtt
    m_mqttClient->connect();
    
    //connect to Atem
    atemIpAddress = CFStringCreateWithCString(kCFAllocatorDefault, ipAddress, kCFStringEncodingUTF8);
    if(atemConnect() != S_OK){
        std::cerr << "Failed to connect to Atem" << std::endl;
        exit(1);
    }
    
    //clean up
    CFRelease(atemIpAddress);
}

HRESULT AtemConnectionManager::atemConnect(){
    switcherDiscovery = CreateBMDSwitcherDiscoveryInstance();
    if(switcherDiscovery->ConnectTo(atemIpAddress, &m_switcher, &connectionFailureReason) != S_OK){
        std::cout << "Error connecting" << std::endl;
        return S_FALSE;
    }
    //connect to atem
    IBMDSwitcherMixEffectBlockIterator *mixEffectIterator = nullptr;
    m_switcher->CreateIterator(IID_IBMDSwitcherMixEffectBlockIterator, (void**)&mixEffectIterator);
    mixEffectIterator->Next(&m_mixBlock);
    
    //create callbacks
    AtemInputCallback *callback = new AtemInputCallback(this);
    m_mixBlock->AddCallback(callback);
    
    AtemSwitcherCallback *switcherCallback = new AtemSwitcherCallback(this);
    m_switcher->AddCallback(switcherCallback);
    
    //clean up
    mixEffectIterator->Release();
    switcherDiscovery->Release();
    return S_OK;
}

AtemConnectionManager::AtemInputCallback::AtemInputCallback(AtemConnectionManager* parent):m_parentManager(parent){}

HRESULT STDMETHODCALLTYPE AtemConnectionManager::AtemInputCallback::QueryInterface(REFIID iid, LPVOID *ppv){
    if (memcmp(&iid, &IID_IBMDSwitcherMixEffectBlockCallback, sizeof(CFUUIDBytes)) == 0) {
        *ppv = static_cast<IBMDSwitcherMixEffectBlockCallback*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE AtemConnectionManager::AtemInputCallback::AddRef(){
    return ++refCount;
}

ULONG STDMETHODCALLTYPE AtemConnectionManager::AtemInputCallback::Release(){
    ULONG newRef = --refCount;
    if (newRef == 0) delete this;
    return newRef;
}

std::string AtemConnectionManager::getLongName(IBMDSwitcherInput *input){
    CFStringRef longName;
    input->GetLongName(&longName);
    char nameBuffer[256];
    CFStringGetCString(longName, nameBuffer, sizeof(nameBuffer), kCFStringEncodingUTF8);
    CFRelease(longName);
    return nameBuffer;
}

HRESULT STDMETHODCALLTYPE AtemConnectionManager::AtemInputCallback::Notify(BMDSwitcherMixEffectBlockEventType eventType){
    IBMDSwitcherInput *input = nullptr;
    IBMDSwitcherInputIterator *inputIterator = nullptr;
    m_parentManager->m_switcher->CreateIterator(IID_IBMDSwitcherInputIterator, (void**)&inputIterator);
    bool programTally;
    bool previewTally;
    
    if(eventType == bmdSwitcherMixEffectBlockEventTypeProgramInputChanged || eventType == bmdSwitcherMixEffectBlockEventTypePreviewInputChanged || eventType == bmdSwitcherMixEffectBlockEventTypeInTransitionChanged){
        while(inputIterator->Next(&input) == S_OK){
            input->IsProgramTallied(&programTally);
            input->IsPreviewTallied(&previewTally);
            std::string currentInputName = m_parentManager->getLongName(input);
            if(programTally){
                m_parentManager->m_mqttClient->publish(currentInputName, "program");
                continue;
            }
            if(previewTally){
                m_parentManager->m_mqttClient->publish(currentInputName, "preview");
                continue;
            }
            else{
                m_parentManager->m_mqttClient->publish(currentInputName, "off");
                continue;
            }
        }
    }
    
    //input->Release();
    inputIterator->Release();
    return S_OK;
}

AtemConnectionManager::AtemSwitcherCallback::AtemSwitcherCallback(AtemConnectionManager* parent):m_parentManager(parent){}

HRESULT STDMETHODCALLTYPE AtemConnectionManager::AtemSwitcherCallback::QueryInterface(REFIID iid, LPVOID *ppv){
    if (memcmp(&iid, &IID_IBMDSwitcherMixEffectBlockCallback, sizeof(CFUUIDBytes)) == 0) {
        *ppv = static_cast<IBMDSwitcherCallback*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE AtemConnectionManager::AtemSwitcherCallback::AddRef(){
    return ++refCount;
}

ULONG STDMETHODCALLTYPE AtemConnectionManager::AtemSwitcherCallback::Release(){
    ULONG newRef = --refCount;
    if (newRef == 0) delete this;
    return newRef;
}


HRESULT STDMETHODCALLTYPE AtemConnectionManager::AtemSwitcherCallback::Notify(BMDSwitcherEventType eventType, BMDSwitcherVideoMode coreVideoMode){
    if(eventType == bmdSwitcherEventTypeDisconnected){
        m_parentManager->m_switcher = nullptr;
        int maxAttempts = 5;
        int currentAttempt = 0;
        std::cerr << "Disconnected" << std::endl;
        std::cerr << "Attempting to reconnect" << std::endl;
        for(; currentAttempt < maxAttempts; currentAttempt++){
            if(m_parentManager->atemConnect() == S_OK){
                std::cout << "Reconnected!" << std::endl;
                break;
            }
        }
    }
    return S_OK;
}
