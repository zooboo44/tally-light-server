#include "AtemConnectionManager.h"

AtemConnectionManager::AtemConnectionManager(const char* ipAddress, MqttConnectionManager* mqtt):m_mqttClient(mqtt){
    //connect to mqtt
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
    
    
    //connect to atem
    IBMDSwitcherMixEffectBlockIterator *mixEffectIterator = nullptr;
    m_switcher->CreateIterator(IID_IBMDSwitcherMixEffectBlockIterator, (void**)&mixEffectIterator);
    mixEffectIterator->Next(&m_mixBlock);
    
    //create callback
    AtemInputCallback *callback = new AtemInputCallback(this);
    m_mixBlock->AddCallback(callback);
    
    //generate map of inputIDs
    IBMDSwitcherInputIterator *inputIterator = nullptr;
    IBMDSwitcherInput *input = nullptr;
    m_switcher->CreateIterator(IID_IBMDSwitcherInputIterator, (void**)&inputIterator);
    bool *inputIsTallied = new bool;
    bool *inputIsPreviewed = new bool;
    std::cout << "\nInitialize Map:" << std::endl;
    while(inputIterator->Next(&input) == S_OK){
        input->IsProgramTallied(inputIsTallied);
        input->IsPreviewTallied(inputIsPreviewed);
        if(*inputIsTallied){
            std::cout << getLongName(input) << " " << "program" << std::endl;
            m_currentProgram[getLongName(input)] = "program";
        }
        if(*inputIsPreviewed){
            std::cout << getLongName(input) << " " << "preview" << std::endl;
            m_currentProgram[getLongName(input)] = "preview";
        }
        else{
            std::cout << getLongName(input) << " " << "off" << std::endl;
            m_currentProgram[getLongName(input)] = "off";
        }
    }
    
    std::cout << "\n\n\nPrint initialized map:\n";
    for(const auto& pair : m_currentProgram){
        std::cout << pair.first << " " << pair.second << std::endl;
    }
    
    //clean up
    switcherDiscovery->Release();
    mixEffectIterator->Release();
    inputIterator->Release();
    input->Release();
    delete(inputIsTallied);
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
    bool inputIsTallied;
    if(eventType == bmdSwitcherMixEffectBlockEventTypeProgramInputChanged){
        //BMDSwitcherInputId inputId;
        //ask brian about derefrence nullptr if inputID* = nullptr; later in code I GetInputId(inputId) and it didnt work
        
        while(inputIterator->Next(&input) == S_OK){
            input->IsProgramTallied(&inputIsTallied);
            if(inputIsTallied == true){
                m_parentManager->m_mqttClient->publish(m_parentManager->getLongName(input), "program");
                m_parentManager->m_currentProgram[m_parentManager->getLongName(input)] = "program";
            }
            else{
                m_parentManager->m_mqttClient->publish(m_parentManager->getLongName(input), "off");
                m_parentManager->m_currentProgram[m_parentManager->getLongName(input)] = "off";
            }
        }
    }
    
    if(eventType == bmdSwitcherMixEffectBlockEventTypeInTransitionChanged){
        while(inputIterator->Next(&input) == S_OK){
            input->IsProgramTallied(&inputIsTallied);
            if(inputIsTallied == true){
                m_parentManager->m_mqttClient->publish(m_parentManager->getLongName(input), "program");
                m_parentManager->m_currentProgram[m_parentManager->getLongName(input)] = "program";
            }
            else{
                m_parentManager->m_mqttClient->publish(m_parentManager->getLongName(input), "off");
                m_parentManager->m_currentProgram[m_parentManager->getLongName(input)] = "off";
            }
        }
    }
    
    if(eventType == bmdSwitcherMixEffectBlockEventTypePreviewInputChanged){
        while(inputIterator->Next(&input) == S_OK){
            input->IsPreviewTallied(&inputIsTallied);
            if(inputIsTallied == true){
                m_parentManager->m_mqttClient->publish(m_parentManager->getLongName(input), "preview");
                m_parentManager->m_currentProgram[m_parentManager->getLongName(input)] = "preview";
            }
            else{
                m_parentManager->m_mqttClient->publish(m_parentManager->getLongName(input), "off");
                m_parentManager->m_currentProgram[m_parentManager->getLongName(input)] = "off";
            }
        }
    }
    
    std::cout << "\n\nIterate through map" << std::endl;
    for(const auto& pair : m_parentManager->m_currentProgram){
        std::cout << pair.first << " " << pair.second << std::endl;
    }
    
    //input->Release();
    inputIterator->Release();
    return S_OK;
}
