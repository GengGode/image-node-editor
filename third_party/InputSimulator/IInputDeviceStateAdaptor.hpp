#pragma once

namespace WindowsInput
{
    enum class VirtualKeyCode : unsigned short;
    class IInputDeviceStateAdaptor
    {
    public:
        virtual bool IsKeyDown(VirtualKeyCode keyCode) = 0;
        virtual bool IsKeyUp(VirtualKeyCode keyCode) = 0;
        virtual bool IsHardwareKeyDown(VirtualKeyCode keyCode) = 0;
        virtual bool IsHardwareKeyUp(VirtualKeyCode keyCode) = 0;
        virtual bool IsTogglingKeyInEffect(VirtualKeyCode keyCode) = 0;
    };
}
