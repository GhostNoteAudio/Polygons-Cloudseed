#pragma once

#include "Arduino.h"
#include <cmath>
#include <algorithm>

namespace Revo
{
    static inline float Clip1(float value)
    {
        return value > 1 ? 1 : (value < -1 ? -1 : value);
    }

    static inline float Clip(float value, float min, float max)
    {
        return value > max ? max : (value < min ? min : value);
    }

    static inline int Clip(int value, int min, int max)
    {
        return value > max ? max : (value < min ? min : value);
    }

    static inline void ZeroBuffer(float* buffer, int len)
    {
        for (int i = 0; i < len; i++)
            buffer[i] = 0.0f;
    }

    static inline void ZeroBuffer(int16_t* buffer, int len)
    {
        for (int i = 0; i < len; i++)
            buffer[i] = 0;
    }

    static inline void Copy(float* source, float* dest, int len)
    {

        memcpy(dest, source, len * sizeof(float));
    }

    static inline void Gain(float* buffer, float gain, int len)
    {
        for (int i = 0; i < len; i++)
        {
            buffer[i] *= gain;
        }
    }

    static inline void Mix(float* target, float* source, float gain, int len)
    {
        for (int i = 0; i < len; i++)
            target[i] += source[i] * gain;
    }

    template<typename T>
    static double DB2gain(T input)
    {
        return std::pow(10, input / 20.0);
    }

    template<typename T>
    static double Gain2DB(T input)
    {
        if (input < 0.0000001)
            return -100000;

        return 20.0f * std::log10(input);
    }

    static inline double Response4Oct(double input)
    {
        return std::min((std::pow(16, input) - 1.0) * 0.066666666667, 1.0);
    }

    static inline double Response2Dec(double input)
    {
        return std::min((std::pow(100, input) - 1.0) * 0.01010101010101, 1.0);
    }
}
