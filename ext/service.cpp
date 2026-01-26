#include "service.h"
#include <chrono>
#include <system_error>
#include <time.h>
#include "php.h"
#include "logging.h"

using namespace std::chrono_literals;

namespace Solarwinds {
    Service::Service(int interval) : stopping_(false), interval_(interval) {
        init_ = false;
    }

    Service::~Service() {
    }

    void Service::start() {
        try {
            th_ = std::thread([&]() {
                while (true) {
                    std::unique_lock<std::mutex> lock(mutex_);
                    if (cv_.wait_for(lock, interval_ * 1s, [&]() { return stopping_ || !init_; })) {
                        if (stopping_) {
                            break;
                        }
                        if (!init_) {
                            init_ = true;
                            task();
                        }
                    } else {
                        task();
                    }
                }
            });
        } catch (const std::system_error& e) {
            log_with_time("System error: %s", e.what());
        }
    }
    void Service::stop() {
        try {
            {
                std::unique_lock<std::mutex> lock(mutex_);
                stopping_ = true;
            }
            cv_.notify_all();
            th_.join();
        } catch (const std::system_error& e) {
            log_with_time("System error: %s", e.what());
        }
    }
}
