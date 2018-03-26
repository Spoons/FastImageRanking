#pragma once

    template <class T>
T FastMax(const T& left, const T& right)
{
    return left > right ? left : right;
}


    template <class T>
T FastMin(const T& left, const T& right)
{
    return left < right ? left : right;
}

