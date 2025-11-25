#pragma once
#ifndef H_NUMERIC
#define H_NUMERIC

#include <cmath>
#include <limits>

 //其目的就是用来比较数据是否相等，变相的 float1 - float2  < 0.000001
template<class T>
typename std::enable_if<!std::numeric_limits<T>::is_integer, bool>::type
almost_equal(T x, T y, int ulp = 2)
{

    return std::abs(x - y) <= std::numeric_limits<T>::epsilon() * std::abs(x + y) * ulp
        || std::abs(x - y) < std::numeric_limits<T>::min();
}

template<class T>
T half(T x) {}

template <>
float half(float x) { return 0.5f * x; }

template <>
double half(double x) { return 0.5 * x; }

#endif
