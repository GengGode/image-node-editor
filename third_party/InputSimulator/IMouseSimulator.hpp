#pragma once
#include "IKeyboardSimulator.hpp"

#include <chrono>
#include <string>
#include <vector>

namespace WindowsInput
{
    class IKeyboardSimulator;
    class IMouseSimulator
    {
    public:
        virtual int GetMouseWheelClickSize() = 0;
        virtual void SetMouseWheelClickSize(int value) = 0;

        virtual IKeyboardSimulator *GetKeyboardSimulator() = 0;

        virtual IMouseSimulator &MoveMouseBy(int pixelDeltaX, int pixelDeltaY) = 0;
        virtual IMouseSimulator &MoveMouseTo(double absoluteX, double absoluteY) = 0;
        virtual IMouseSimulator &MoveMouseToPositionOnVirtualDesktop(double absoluteX, double absoluteY) = 0;

        virtual IMouseSimulator &LeftButtonDown() = 0;
        virtual IMouseSimulator &LeftButtonUp() = 0;
        virtual IMouseSimulator &LeftButtonClick() = 0;
        virtual IMouseSimulator &LeftButtonDoubleClick() = 0;

        virtual IMouseSimulator &MiddleButtonDown() = 0;
        virtual IMouseSimulator &MiddleButtonUp() = 0;
        virtual IMouseSimulator &MiddleButtonClick() = 0;
        virtual IMouseSimulator &MiddleButtonDoubleClick() = 0;

        virtual IMouseSimulator &RightButtonDown() = 0;
        virtual IMouseSimulator &RightButtonUp() = 0;
        virtual IMouseSimulator &RightButtonClick() = 0;
        virtual IMouseSimulator &RightButtonDoubleClick() = 0;

        virtual IMouseSimulator &XButtonDown(int buttonId) = 0;
        virtual IMouseSimulator &XButtonUp(int buttonId) = 0;
        virtual IMouseSimulator &XButtonClick(int buttonId) = 0;
        virtual IMouseSimulator &XButtonDoubleClick(int buttonId) = 0;

        virtual IMouseSimulator &VerticalScroll(int scrollAmountInClicks) = 0;
        virtual IMouseSimulator &HorizontalScroll(int scrollAmountInClicks) = 0;

        virtual IMouseSimulator &Sleep(int millisecondsTimeout) = 0;
        virtual IMouseSimulator &Sleep(const std::chrono::duration<int, std::milli> &timeout) = 0;
    };
}