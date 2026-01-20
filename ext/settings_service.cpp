#include "settings_service.h"
#ifndef _WIN32
#include <sys/types.h>
#include <unistd.h>
#else
#include <winsock.h>
#endif
#include <time.h>
#include "php.h"
#include "logging.h"

namespace Solarwinds {
    constexpr static int MAX_HOSTNAME_LENGTH = 256;

    // Callback to write received data into a std::string
    size_t WriteCallback(char* contents, size_t size, size_t nmemb, void* userdata) {
        size_t totalSize = size * nmemb;
        std::string* response = static_cast<std::string*>(userdata);
        response->append(contents, totalSize);
        return totalSize;
    }

    SettingsService::SettingsService(const std::string& service_key, const std::string& collector, int refresh_interval_s) : Service(refresh_interval_s), headers_(nullptr) {
        // hostname
        char hostname[Solarwinds::MAX_HOSTNAME_LENGTH] = {0};
        if (gethostname(hostname, sizeof(hostname)) != 0) {
            // On failure, ensure hostname is an empty string
            hostname[0] = '\0';
        }
        // service name
        auto pos = service_key.find_last_of(':');
        auto token = (pos != std::string::npos) ? service_key.substr(0, pos) : service_key;
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
        auto auth = "Authorization: Bearer " + token;
        headers_ = curl_slist_append(headers_, auth.c_str());
        curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, headers_);
        // 10 seconds https timeout
        curl_easy_setopt(curl_, CURLOPT_TIMEOUT, 10L);
        // write callback function
        curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, WriteCallback);
        // service start
        start();
    }

    SettingsService::~SettingsService() {
        // service stop
        stop();
        // free headers
        curl_slist_free_all(headers_);
        // curl cleanup
        curl_easy_cleanup(curl_);
    }

    void SettingsService::task() {
        // write callback data
        std::string response_body;
        curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &response_body);
        auto res = curl_easy_perform(curl_);
        if (res != CURLE_OK) {
            log_with_time("curl_easy_perform() failed: %s", curl_easy_strerror(res));
            return;
        }
        long http_code = 0;
        curl_easy_getinfo(curl_, CURLINFO_RESPONSE_CODE, &http_code);
        if (http_code != 200) {
            log_with_time("HTTP request failed with code: %ld", http_code);
            return;
        }
        {
            std::unique_lock<std::mutex> lock(settings_mutex_);
            settings_ = response_body;
        }
        // log_with_time("Settings updated: %s", settings_.c_str());
    }

    std::string SettingsService::getSettings(){
        std::unique_lock<std::mutex> lock(settings_mutex_);
        return settings_;
    }
}
