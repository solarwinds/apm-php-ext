#include "setting_service.h"
#ifndef _WIN32
#include <sys/types.h>
#include <unistd.h>
#endif
#include <time.h>
#include "php.h"

namespace Solarwinds {
    // Callback to write received data into a std::string
    size_t WriteCallback(char* contents, size_t size, size_t nmemb, void* userdata) {
        size_t totalSize = size * nmemb;
        std::string* response = static_cast<std::string*>(userdata);
        response->append(contents, totalSize);
        return totalSize;
    }

    SettingService::SettingService(const std::string& service_key, const std::string& collector, int refresh_interval_ms) : Service(refresh_interval_ms), headers_(nullptr) {
        // hostname
        char hostname[256] = {0};
        gethostname(hostname, sizeof(hostname));
        // service name
        auto pos = service_key.find_last_of(':');
        auto service_name = (pos != std::string::npos) ? service_key.substr(pos+1) : "unknown";
        // curl init
        curl_ = curl_easy_init();
        // url
        auto url = "https://" + collector + "/v1/settings/" + service_name + "/" + hostname;
        curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl_, CURLOPT_FOLLOWLOCATION, 1L);
        // ssl options
        curl_easy_setopt(curl_, CURLOPT_SSL_OPTIONS, CURLSSLOPT_NATIVE_CA);
        // Authorization header
        auto auth = "Authorization: Bearer " + service_key;
        headers_ = curl_slist_append(headers_, auth.c_str());
        curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, headers_);
        // 10 seconds https timeout
        curl_easy_setopt(curl_, CURLOPT_TIMEOUT, 10L);
        // write callback function
        curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, WriteCallback);
        // service start
        start();
    }

    SettingService::~SettingService() {
        // service stop
        stop();
        // free headers
        curl_slist_free_all(headers_);
        // curl cleanup
        curl_easy_cleanup(curl_);
    }

    void SettingService::task() {
        // write callback data
        std::string response_body;
        curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &response_body);
        auto res = curl_easy_perform(curl_);
        if (res != CURLE_OK) {
            php_printf("Time: %lu curl_easy_perform() failed: %s\n", (long)time(NULL), curl_easy_strerror(res));
            return;
        }
        long http_code = 0;
        curl_easy_getinfo(curl_, CURLINFO_RESPONSE_CODE, &http_code);
        if (http_code != 200) {
            php_printf("Time: %lu HTTP request failed with code: %ld\n", (long)time(NULL), http_code);
            return;
        }
        {
            std::unique_lock<std::mutex> lock(setting_mutex_);
            setting_ = response_body;
        }
        // php_printf("Time: %lu Setting updated: %s\n", (long)time(NULL), setting_.c_str());
    }

    std::string SettingService::getSetting(){
        std::unique_lock<std::mutex> lock(setting_mutex_);
        return setting_;
    }
}
