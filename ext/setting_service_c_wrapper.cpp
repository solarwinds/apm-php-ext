#include "setting_service_c_wrapper.h"
#include "setting_service.h"
#include <cstring>
#include <new>

void* Setting_Service_Allocate(char *collector, char *service_key) {
    if (collector != nullptr && std::strlen(collector) > 0 && service_key != nullptr && std::strlen(service_key) > 0) {
        std::string collector_str = collector;
        std::string service_key_str = service_key;
        auto p = new(std::nothrow) Solarwinds::SettingService(service_key_str, collector_str);
        return static_cast<void *>(p);
    }
    return nullptr;
}

void Setting_Service_Free(void* service) {
    if (service != nullptr) {
        auto s = static_cast<Solarwinds::SettingService *>(service);
        delete s;
    }
}

bool Setting_Service_Get_Setting(void *service, char* ans) {
    if (service != nullptr) {
        auto s = static_cast<Solarwinds::SettingService *>(service);
        auto setting = s->getSetting();
        strncpy(ans, setting.c_str(), setting.size());
        ans[setting.size()] = '\0';
        return true;
    }
    return false;
}
