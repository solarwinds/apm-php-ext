#include "settings_service_c_wrapper.h"
#include "settings_service.h"
#include <cstring>
#include <new>

void* Settings_Service_Allocate(char *collector, char *service_key) {
    if (collector != nullptr && std::strlen(collector) > 0 && service_key != nullptr && std::strlen(service_key) > 0) {
        std::string collector_str = collector;
        std::string service_key_str = service_key;
        auto p = new(std::nothrow) Solarwinds::SettingsService(service_key_str, collector_str);
        return static_cast<void *>(p);
    }
    return nullptr;
}

void Settings_Service_Free(void* service) {
    if (service != nullptr) {
        auto s = static_cast<Solarwinds::SettingsService *>(service);
        delete s;
    }
}

bool Settings_Service_Get_Settings(void *service, char* ans) {
    if (service != nullptr) {
        auto s = static_cast<Solarwinds::SettingsService *>(service);
        auto settings = s->getSettings();
        snprintf(ans, SETTINGS_BUFFER_SIZE, "%s", settings.c_str());
        return true;
    }
    return false;
}
