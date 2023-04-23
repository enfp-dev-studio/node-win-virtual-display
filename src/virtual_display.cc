#include "virtual_display.h"
#include <napi.h>

#include <windows.h>
#include <dxgi1_6.h>

#pragma comment(lib, "dxgi.lib")

class VDisplay : public Napi::ObjectWrap<VDisplay> {
public:
  static Napi::Function GetClass(Napi::Env);
  VDisplay(const Napi::CallbackInfo &info);
  Napi::Value CreateVirtualDisplay(const Napi::CallbackInfo &info);
  Napi::Value DestroyVirtualDisplay(const Napi::CallbackInfo &info);

private:
  IDXGIOutputDuplication *_duplication;
};

VDisplay::VDisplay(const Napi::CallbackInfo &info) : ObjectWrap(info), _duplication(nullptr) {
}

Napi::Function VDisplay::GetClass(Napi::Env env) {
  return DefineClass(
      env, "VDisplay",
      {
          VDisplay::InstanceMethod("createVirtualDisplay",
                                   &VDisplay::CreateVirtualDisplay),
          VDisplay::InstanceMethod("destroyVirtualDisplay",
                                   &VDisplay::DestroyVirtualDisplay),
      });
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  Napi::String name = Napi::String::New(env, "VDisplay");
  exports.Set(name, VDisplay::GetClass(env));
  return exports;
}

Napi::Value VDisplay::CreateVirtualDisplay(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  if (_duplication) {
    return Napi::Boolean::New(env, false);
  }

  IDXGIFactory1 *factory = nullptr;
  IDXGIAdapter *adapter = nullptr;
  IDXGIOutput *output = nullptr;

  HRESULT hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void **)&factory);
  if (FAILED(hr)) {
    return Napi::Boolean::New(env, false);
  }

  hr = factory->EnumAdapters(0, &adapter);
  if (FAILED(hr)) {
    factory->Release();
    return Napi::Boolean::New(env, false);
  }

  hr = adapter->EnumOutputs(0, &output);
  if (FAILED(hr)) {
    adapter->Release();
    factory->Release();
    return Napi::Boolean::New(env, false);
  }

  DXGI_OUTPUT_DESC desc;
  output->GetDesc(&desc);

  IDXGIOutput1 *output1 = nullptr;
  hr = output->QueryInterface(__uuidof(IDXGIOutput1), (void **)&output1);
  if (FAILED(hr)) {
    output->Release();
    adapter->Release();
    factory->Release();
    return Napi::Boolean::New(env, false);
  }

  hr = output1->DuplicateOutput(adapter, &_duplication);
  if (FAILED(hr)) {
    output1->Release();
    output->Release();
    adapter->Release();
    factory->Release();
    return Napi::Boolean::New(env, false);
  }

  output1->Release();
  output->Release();
  adapter->Release();
  factory->Release();

  return Napi::Boolean::New(env, true);
}

Napi::Value VDisplay::DestroyVirtualDisplay(const Napi::CallbackInfo &info) {
  if (_duplication) {
    _duplication->Release();
    _duplication = nullptr;
    return Napi::Boolean::New(info.Env(), true);
  } else {
    return Napi::Boolean::New(info.Env(), false);
  }
}

Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
  return VirtualDisplay::Init(env, exports);
}

NODE_API_MODULE(addon, InitAll)