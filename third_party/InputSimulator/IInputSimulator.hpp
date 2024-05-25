#pragma once
#include "IKeyboardSimulator.hpp"
#include "IMouseSimulator.hpp"
#include "IInputDeviceStateAdaptor.hpp"

namespace WindowsInput
{
    class IInputSimulator
    {
    public:
        virtual IKeyboardSimulator &GetKeyboardSimulator() = 0;
        virtual IMouseSimulator &GetMouseSimulator() = 0;
        virtual IInputDeviceStateAdaptor &GetInputDeviceStateAdaptor() = 0;
    };
}