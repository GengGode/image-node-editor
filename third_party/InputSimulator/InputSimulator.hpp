#pragma once
#include "KeyboardSimulator.hpp"
#include "MouseSimulator.hpp"
#include "WindowsInputDeviceStateAdaptor.hpp"

namespace WindowsInput
{
    class InputSimulator : public IInputSimulator
    {
    private:
        IKeyboardSimulator *keyboardSimulator_;
        IMouseSimulator *mouseSimulator_;
        IInputDeviceStateAdaptor *inputDeviceStateAdaptor_;

    public:
        InputSimulator(IKeyboardSimulator *keyboardSimulator, IMouseSimulator *mouseSimulator, IInputDeviceStateAdaptor *inputDeviceStateAdaptor)
            : keyboardSimulator_(keyboardSimulator), mouseSimulator_(mouseSimulator), inputDeviceStateAdaptor_(inputDeviceStateAdaptor) {}

        InputSimulator()
        {
            keyboardSimulator_ = new KeyboardSimulator(this);
            mouseSimulator_ = new MouseSimulator(this);
            inputDeviceStateAdaptor_ = new WindowsInputDeviceStateAdaptor();
        }

        ~InputSimulator()
        {
            delete keyboardSimulator_;
            delete mouseSimulator_;
            delete inputDeviceStateAdaptor_;
        }

        IKeyboardSimulator &GetKeyboardSimulator() override { return *keyboardSimulator_; }
        IMouseSimulator &GetMouseSimulator() override { return *mouseSimulator_; }
        IInputDeviceStateAdaptor &GetInputDeviceStateAdaptor() override { return *inputDeviceStateAdaptor_; }
    };
}