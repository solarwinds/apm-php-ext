#ifndef EXT_SERVICE_H
#define EXT_SERVICE_H

#include <condition_variable>
#include <mutex>
#include <thread>

namespace Solarwinds {
class Service {
public:
  Service(int interval);
  virtual ~Service();
  virtual void task() = 0;

protected:
  void start();
  void stop();

  std::mutex mutex_;
  std::condition_variable cv_;
  std::thread th_;
  bool stopping_;
  int interval_;
  bool init_;
};
} // namespace Solarwinds

#endif // EXT_SERVICE_H