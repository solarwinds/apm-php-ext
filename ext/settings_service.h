#ifndef EXT_SETTINGS_SERVICE_H
#define EXT_SETTINGS_SERVICE_H

#include "service.h"
#include <mutex>
#include <string>
#include <curl/curl.h>

namespace Solarwinds {
class SettingsService : public Service {
  static constexpr int DEFAULT_REFRESH_INTERVAL_S = 60;

public:
  SettingsService(const std::string &service_key, const std::string &collector,
                 int refresh_interval_s = DEFAULT_REFRESH_INTERVAL_S);
  virtual ~SettingsService();
  virtual void task() override;
  std::string getSettings();

private:
  std::mutex settings_mutex_;
  std::string settings_;
  CURL *curl_;
  struct curl_slist *headers_;
};
} // namespace Solarwinds

#endif // EXT_SETTINGS_SERVICE_H
