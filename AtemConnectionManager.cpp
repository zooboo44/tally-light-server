#include "AtemConnectionManager.h"

AtemConnectionManager::AtemConnectionManager(const char* ipAddress, MqttConnectionManager* mqtt):m_mqttClient(mqtt){
    m_mqttClient->connect();
    IBMDSwitcherDiscovery* switcherDiscovery = CreateBMDSwitcherDiscoveryInstance();
    BMDSwitcherConnectToFailure connectionFailureReason;
    CFStringRef ipAddressString = CFStringCreateWithCString(kCFAllocatorDefault, ipAddress, kCFStringEncodingUTF8);
    
    if(switcherDiscovery->ConnectTo(ipAddressString, &m_switcher, &connectionFailureReason) != S_OK){
        std::cout << "Error connecting" << std::endl;
        exit(1);
    }
    CFRelease(ipAddressString);
    std::cout << "Connection succeded!!!!" << std::endl;
    
    IBMDSwitcherMixEffectBlockIterator *mixEffectIterator = nullptr;
    m_switcher->CreateIterator(IID_IBMDSwitcherMixEffectBlockIterator, (void**)&mixEffectIterator);
    mixEffectIterator->Next(&m_mixBlock);
    
    AtemInputCallback *callback = new AtemInputCallback(this);
    m_mixBlock->AddCallback(callback);
    
    switcherDiscovery->Release();
    mixEffectIterator->Release();
}

AtemConnectionManager::AtemInputCallback::AtemInputCallback(AtemConnectionManager* parent):parentManager(parent){}

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

HRESULT STDMETHODCALLTYPE AtemConnectionManager::AtemInputCallback::Notify(BMDSwitcherMixEffectBlockEventType eventType){
    if(eventType == bmdSwitcherMixEffectBlockEventTypeTransitionPositionChanged){
        logProgramInput();
    }
    return S_OK;
}

void AtemConnectionManager::AtemInputCallback::logProgramInput(){
    IBMDSwitcherInput *input = nullptr;
    IBMDSwitcherInputIterator* inputIterator = nullptr;
    parentManager->m_switcher->CreateIterator(IID_IBMDSwitcherInputIterator, (void**)&inputIterator);
    bool *inputIsTallied = new bool;
    
    std::cout << "Tallied: " << std::endl;
    while(inputIterator->Next(&input) == S_OK){
        if(input->IsProgramTallied(inputIsTallied) == S_OK && *inputIsTallied == true){
            CFStringRef longName = nullptr;
            input->GetLongName(&longName);
            char nameBuffer[256];
            CFStringGetCString(longName, nameBuffer, sizeof(nameBuffer), kCFStringEncodingUTF8);
            std::cout << nameBuffer << std::endl;
            CFRelease(longName);
        }
    }
    
    input->Release();
    inputIterator->Release();
    delete(inputIsTallied);
    
}
