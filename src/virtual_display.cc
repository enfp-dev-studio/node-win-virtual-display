#include <napi.h>
#include <vector>
#include <thread>
#include "parsec-vdd.h"

using namespace parsec_vdd;

class VirtualDisplay : public Napi::ObjectWrap<VirtualDisplay>
{
    void StartUpdater()
    {
        if (!running)
        {
            running = true;
            updater = std::thread([this]
                                  {
                while (running) {
                    VddUpdate(vdd);
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                } });
        }
    }

    void StopUpdater()
    {
        if (running)
        {
            running = false;
            if (updater.joinable())
            {
                updater.join();
            }
        }
    }
    std::vector<int> displays;
    std::thread updater;
    std::atomic<bool> running;
    HANDLE vdd;

public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    VirtualDisplay(const Napi::CallbackInfo &info);

private:
    static Napi::FunctionReference constructor;
    Napi::Value CreateVirtualDisplay(const Napi::CallbackInfo &info);
    Napi::Value DestroyVirtualDisplay(const Napi::CallbackInfo &info);
};

Napi::FunctionReference VirtualDisplay::constructor;

Napi::Object VirtualDisplay::Init(Napi::Env env, Napi::Object exports)
{
    Napi::Function func = DefineClass(env, "VirtualDisplay", {
                                                                 InstanceMethod("createVirtualDisplay", &VirtualDisplay::CreateVirtualDisplay),
                                                                 InstanceMethod("destroyVirtualDisplay", &VirtualDisplay::DestroyVirtualDisplay),
                                                             });
    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();

    exports.Set("VirtualDisplay", func);
    return exports;
}

VirtualDisplay::VirtualDisplay(const Napi::CallbackInfo &info) : Napi::ObjectWrap<VirtualDisplay>(info)
{
    // Constructor logic goes here
}

Napi::Value VirtualDisplay::CreateVirtualDisplay(const Napi::CallbackInfo &info)
{
    this->vdd = OpenDeviceHandle(&VDD_ADAPTER_GUID);

    Napi::Env env = info.Env();

    if (displays.size() == 0 && displays.size() < VDD_MAX_DISPLAYS)
    {
        int index = VddAddDisplay(vdd);
        StartUpdater();
        displays.push_back(index);


        return Napi::Number::New(env, index);
    }
    else
    {
        return env.Null(); // 또는 오류 메시지 반환
    }
    // Logic to create virtual display goes here
}

Napi::Value VirtualDisplay::DestroyVirtualDisplay(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    //
    Napi::HandleScope scope(env);

    // 'console' 객체를 가져옵니다.
    Napi::Object global = env.Global();
    Napi::Object console = global.Get("console").As<Napi::Object>();

    // 'log' 함수를 가져옵니다.
    Napi::Function log = console.Get("log").As<Napi::Function>();

    log.Call(console, {Napi::String::New(env, "DestroyVirtualDisplay")});
    //
    if (!displays.empty())
    {
        int index = displays.back();
        StopUpdater();
        VddRemoveDisplay(vdd, index);
        displays.pop_back();
        return Napi::Number::New(env, index);
    }
    else
    {
        return env.Null(); // 또는 오류 메시지 반환
    }
    // Logic to destroy virtual display goes here
}

Napi::Object InitAll(Napi::Env env, Napi::Object exports)
{
    return VirtualDisplay::Init(env, exports);
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, InitAll)