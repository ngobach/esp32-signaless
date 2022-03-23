#pragma once

#include <chrono>
#include <cstdio>
#include <string>
#include <initializer_list>

uint32_t millis() {
    uint32_t now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    return now;
}

class FakeSerial {
public:
    void println(const char *str) {
        ::printf("Foobar\n");
    }

    template<typename...Args>
    void printf(Args... args) {
        ::printf(args...);
    }
};

FakeSerial Serial;
