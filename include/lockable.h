#pragma once

#include <pch.h>

class Lockable {
    private:
        static std::vector<std::unique_ptr<std::recursive_mutex>> __mutexes;

        unsigned long __mutex_index;

    public:
        Lockable() {
            this->__mutex_index = __mutexes.size();
            __mutexes.emplace_back(std::make_unique<std::recursive_mutex>());
        }

        void lock() {
            __mutexes[__mutex_index]->lock();
        }

        void unlock() {
            __mutexes[__mutex_index]->unlock();
        }
};