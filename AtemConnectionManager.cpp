#include "AtemConnectionManager.h"
#include <iostream>

AtemConnectionManager::AtemConnectionManager(const char* ipAddress){
    IBMDSwitcherDiscovery* switcherDiscovery = CreateBMDSwitcherDiscoveryInstance();
    BMDSwitcherConnectToFailure connectionFailureReason;
    CFStringRef ipAddressString = CFStringCreateWithCString(kCFAllocatorDefault, ipAddress, kCFStringEncodingUTF8);
    
    if(switcherDiscovery->ConnectTo(ipAddressString, &m_switcher, &connectionFailureReason) != S_OK){
        std::cout << "Error connecting" << std::endl;
        exit(1);
    }
    CFRelease(ipAddressString);
    std::cout << "Connection succeded!!!!" << std::endl;
    
    IBMDSwitcherMixEffectBlockIterator *mixEffectIterator;
    m_switcher->CreateIterator(IID_IBMDSwitcherMixEffectBlockIterator, (void**)&mixEffectIterator);
    mixEffectIterator->Next(&m_mixBlock);
    
    AtemInputCallback *callback = new AtemInputCallback();
    m_mixBlock->AddCallback(callback);
    
    switcherDiscovery->Release();
    mixEffectIterator->Release();
}

AtemConnectionManager::AtemInputCallback::AtemInputCallback(){}

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
    if(eventType == bmdSwitcherMixEffectBlockEventTypeProgramInputChanged){
        std::cout << "Program input changed" << std::endl;
    }
    return S_OK;
}
