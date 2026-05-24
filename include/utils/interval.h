#pragma once

template <class T>
class Interval {
    T min;
    T max;

    public:
        constexpr Interval(T min, T max) : min(min), max(max) {};

        friend constexpr bool operator<=(const T& any, const Interval& i) {
            return any >= i.min && any <= i.max;
        }
};