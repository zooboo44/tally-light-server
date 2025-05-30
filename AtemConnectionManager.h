#include "BMDSwitcherAPI.h"
#include <string>

class AtemConnectionManager{
public:
    AtemConnectionManager(const char*);
private:
    IBMDSwitcher* m_switcher;
    IBMDSwitcherMixEffectBlock* m_mixBlock;
    class AtemInputCallback : public IBMDSwitcherMixEffectBlockCallback{
    public:
        AtemInputCallback();
        HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, LPVOID* ppv) override;
        ULONG STDMETHODCALLTYPE AddRef() override;
        ULONG STDMETHODCALLTYPE Release() override;
        virtual HRESULT STDMETHODCALLTYPE Notify(BMDSwitcherMixEffectBlockEventType eventType) override;
    private:
        ULONG refCount;
    };
};
