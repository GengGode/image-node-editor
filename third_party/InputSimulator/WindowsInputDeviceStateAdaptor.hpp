#pragma once
#include <Windows.h>

#include "VirtualKeyCode.hpp"
#include "IInputDeviceStateAdaptor.hpp"

namespace WindowsInput
{
    class WindowsInputDeviceStateAdaptor : public IInputDeviceStateAdaptor
    {
    public:
        bool IsKeyDown(VirtualKeyCode keyCode)
        {
            SHORT result = GetKeyState(static_cast<int>(keyCode));
            return result < 0;
        }

        bool IsKeyUp(VirtualKeyCode keyCode)
        {
            return !IsKeyDown(keyCode);
        }

        bool IsHardwareKeyDown(VirtualKeyCode keyCode)
        {
            SHORT result = GetAsyncKeyState(static_cast<int>(keyCode));
            return result < 0;
        }

        bool IsHardwareKeyUp(VirtualKeyCode keyCode)
        {
            return !IsHardwareKeyDown(keyCode);
        }

        bool IsTogglingKeyInEffect(VirtualKeyCode keyCode)
        {
            SHORT result = GetKeyState(static_cast<int>(keyCode));
            return (result & 0x01) == 0x01;
        }
    };
}