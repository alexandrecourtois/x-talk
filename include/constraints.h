#pragma once

template<class T, class B> struct Derived_from {
    static void constraints(T* p) {
    }
    Derived_from() {
        void(*p)(T*) = constraints;
    }
};
