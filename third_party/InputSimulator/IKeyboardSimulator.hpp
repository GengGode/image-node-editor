#pragma once
#include "IMouseSimulator.hpp"
#include "VirtualKeyCode.hpp"

#include <chrono>
#include <string>
#include <vector>

namespace WindowsInput
{
    class IMouseSimulator;
    class IKeyboardSimulator
    {
    public:
        virtual IMouseSimulator *GetMouseSimulator() = 0;

        virtual IKeyboardSimulator &KeyDown(VirtualKeyCode keyCode) = 0;
        virtual IKeyboardSimulator &KeyDown(const std::vector<VirtualKeyCode> &keyCodes) = 0;

        virtual IKeyboardSimulator &KeyPress(VirtualKeyCode keyCode) = 0;
        virtual IKeyboardSimulator &KeyPress(const std::vector<VirtualKeyCode> &keyCodes) = 0;

        virtual IKeyboardSimulator &KeyUp(VirtualKeyCode keyCode) = 0;
        virtual IKeyboardSimulator &KeyUp(const std::vector<VirtualKeyCode> &keyCodes) = 0;

        virtual IKeyboardSimulator &ModifiedKeyStroke(const std::vector<VirtualKeyCode> &modifierKeyCodes, const std::vector<VirtualKeyCode> &keyCodes) = 0;
        virtual IKeyboardSimulator &ModifiedKeyStroke(const std::vector<VirtualKeyCode> &modifierKeyCodes, VirtualKeyCode keyCode) = 0;
        virtual IKeyboardSimulator &ModifiedKeyStroke(VirtualKeyCode modifierKey, const std::vector<VirtualKeyCode> &keyCodes) = 0;
        virtual IKeyboardSimulator &ModifiedKeyStroke(VirtualKeyCode modifierKeyCode, VirtualKeyCode keyCode) = 0;

        virtual IKeyboardSimulator &TextEntry(const std::string &text) = 0;
        virtual IKeyboardSimulator &TextEntry(char character) = 0;

        virtual IKeyboardSimulator &Sleep(int millisecondsTimeout) = 0;
        virtual IKeyboardSimulator &Sleep(const std::chrono::duration<int, std::milli> &timeout) = 0;
    };
}