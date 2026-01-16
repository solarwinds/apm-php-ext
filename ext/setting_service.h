#ifndef EXT_SETTING_SERVICE_H
#define EXT_SETTING_SERVICE_H

#include "service.h"
#include <mutex>
#include <string>
#include <curl/curl.h>

namespace Solarwinds {
class SettingService : public Service {
  static constexpr int DEFAULT_REFRESH_INTERVAL_S = 60;

public:
  SettingService(const std::string &service_key, const std::string &collector,
                 int refresh_interval_ms = DEFAULT_REFRESH_INTERVAL_S);
  virtual ~SettingService();
  virtual void task() override;
  std::string getSetting();

private:
  std::mutex setting_mutex_;
  std::string setting_;
  CURL *curl_;
  struct curl_slist *headers_;
};
} // namespace Solarwinds

#endif // EXT_SETTING_SERVICE_H