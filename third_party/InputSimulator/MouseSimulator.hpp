#pragma once
#include <Windows.h>
#include <vector>
#include <thread>
#include <chrono>
#include <stdexcept>
#include <cmath>

#include "IInputMessageDispatcher.hpp"
#include "IKeyboardSimulator.hpp"
#include "IMouseSimulator.hpp"
#include "IInputSimulator.hpp"
#include "WindowsInputMessageDispatcher.hpp"
#include "InputBuilder.hpp"

namespace WindowsInput
{
    class MouseSimulator : public IMouseSimulator
    {
    public:
        MouseSimulator(IInputSimulator *inputSimulator) : inputSimulator_(inputSimulator), messageDispatcher_(new WindowsInputMessageDispatcher())
        {
            mouseWheelClickSize_ = 120;
        }

        MouseSimulator(IInputSimulator *inputSimulator, IInputMessageDispatcher *messageDispatcher) : inputSimulator_(inputSimulator), messageDispatcher_(messageDispatcher)
        {
            if (!messageDispatcher)
            {
                throw std::runtime_error("The MouseSimulator cannot operate with a null IInputMessageDispatcher.");
            }
            mouseWheelClickSize_ = 120;
        }

        IKeyboardSimulator *GetKeyboardSimulator() override { return &inputSimulator_->GetKeyboardSimulator(); }

        int GetMouseWheelClickSize() override { return mouseWheelClickSize_; }
        void SetMouseWheelClickSize(int value) override { mouseWheelClickSize_ = value; }

        IMouseSimulator &MoveMouseBy(int pixelDeltaX, int pixelDeltaY)
        {
            std::vector<INPUT> inputList = InputBuilder().AddRelativeMouseMovement(pixelDeltaX, pixelDeltaY).ToArray();
            sendSimulatedInput(inputList);
            return *this;
        }
        IMouseSimulator &MoveMouseTo(double absoluteX, double absoluteY)
        {
            std::vector<INPUT> inputList = InputBuilder().AddAbsoluteMouseMovement(static_cast<int>(std::trunc(absoluteX)), static_cast<int>(std::trunc(absoluteY))).ToArray();
            sendSimulatedInput(inputList);
            return *this;
        }

        IMouseSimulator &MoveMouseToPositionOnVirtualDesktop(double absoluteX, double absoluteY)
        {
            std::vector<INPUT> inputList = InputBuilder().AddAbsoluteMouseMovementOnVirtualDesktop(static_cast<int>(std::trunc(absoluteX)), static_cast<int>(std::trunc(absoluteY))).ToArray();
            sendSimulatedInput(inputList);
            return *this;
        }

        IMouseSimulator &LeftButtonDown()
        {
            std::vector<INPUT> inputList = InputBuilder().AddMouseButtonDown(MouseButton::LeftButton).ToArray();
            sendSimulatedInput(inputList);
            return *this;
        }
        IMouseSimulator &LeftButtonUp()
        {
            std::vector<INPUT> inputList = InputBuilder().AddMouseButtonUp(MouseButton::LeftButton).ToArray();
            sendSimulatedInput(inputList);
            return *this;
        }
        IMouseSimulator &LeftButtonClick()
        {
            std::vector<INPUT> inputList = InputBuilder().AddMouseButtonClick(MouseButton::LeftButton).ToArray();
            sendSimulatedInput(inputList);
            return *this;
        }
        IMouseSimulator &LeftButtonDoubleClick()
        {
            std::vector<INPUT> inputList = InputBuilder().AddMouseButtonDoubleClick(MouseButton::LeftButton).ToArray();
            sendSimulatedInput(inputList);
            return *this;
        }

        IMouseSimulator &MiddleButtonDown()
        {
            std::vector<INPUT> inputList = InputBuilder().AddMouseButtonDown(MouseButton::MiddleButton).ToArray();
            sendSimulatedInput(inputList);
            return *this;
        }

        IMouseSimulator &MiddleButtonUp()
        {
            std::vector<INPUT> inputList = InputBuilder().AddMouseButtonUp(MouseButton::MiddleButton).ToArray();
            sendSimulatedInput(inputList);
            return *this;
        }
        IMouseSimulator &MiddleButtonClick()
        {
            std::vector<INPUT> inputList = InputBuilder().AddMouseButtonClick(MouseButton::MiddleButton).ToArray();
            sendSimulatedInput(inputList);
            return *this;
        }
        IMouseSimulator &MiddleButtonDoubleClick()
        {
            std::vector<INPUT> inputList = InputBuilder().AddMouseButtonDoubleClick(MouseButton::MiddleButton).ToArray();
            sendSimulatedInput(inputList);
            return *this;
        }

        IMouseSimulator &RightButtonDown()
        {
            std::vector<INPUT> inputList = InputBuilder().AddMouseButtonDown(MouseButton::RightButton).ToArray();
            sendSimulatedInput(inputList);
            return *this;
        }
        IMouseSimulator &RightButtonUp()
        {
            std::vector<INPUT> inputList = InputBuilder().AddMouseButtonUp(MouseButton::RightButton).ToArray();
            sendSimulatedInput(inputList);
            return *this;
        }
        IMouseSimulator &RightButtonClick()
        {
            std::vector<INPUT> inputList = InputBuilder().AddMouseButtonClick(MouseButton::RightButton).ToArray();
            sendSimulatedInput(inputList);
            return *this;
        }
        IMouseSimulator &RightButtonDoubleClick()
        {
            std::vector<INPUT> inputList = InputBuilder().AddMouseButtonDoubleClick(MouseButton::RightButton).ToArray();
            sendSimulatedInput(inputList);
            return *this;
        }

        IMouseSimulator &XButtonDown(int buttonId)
        {
            std::vector<INPUT> inputList = InputBuilder().AddMouseXButtonDown(buttonId).ToArray();
            sendSimulatedInput(inputList);
            return *this;
        }
        IMouseSimulator &XButtonUp(int buttonId)
        {
            std::vector<INPUT> inputList = InputBuilder().AddMouseXButtonUp(buttonId).ToArray();
            sendSimulatedInput(inputList);
            return *this;
        }
        IMouseSimulator &XButtonClick(int buttonId)
        {
            std::vector<INPUT> inputList = InputBuilder().AddMouseXButtonClick(buttonId).ToArray();
            sendSimulatedInput(inputList);
            return *this;
        }
        IMouseSimulator &XButtonDoubleClick(int buttonId)
        {
            std::vector<INPUT> inputList = InputBuilder().AddMouseXButtonDoubleClick(buttonId).ToArray();
            sendSimulatedInput(inputList);
            return *this;
        }

        IMouseSimulator &VerticalScroll(int scrollAmountInClicks)
        {
            std::vector<INPUT> inputList = InputBuilder().AddMouseVerticalWheelScroll(scrollAmountInClicks * mouseWheelClickSize_).ToArray();
            sendSimulatedInput(inputList);
            return *this;
        }
        IMouseSimulator &HorizontalScroll(int scrollAmountInClicks)
        {
            std::vector<INPUT> inputList = InputBuilder().AddMouseHorizontalWheelScroll(scrollAmountInClicks * mouseWheelClickSize_).ToArray();
            sendSimulatedInput(inputList);
            return *this;
        }

        IMouseSimulator &Sleep(int millsecondsTimeout)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(millsecondsTimeout));
            return *this;
        }
        IMouseSimulator &Sleep(const std::chrono::duration<int, std::milli> &timeout)
        {
            std::this_thread::sleep_for(timeout);
            return *this;
        }

    private:
        void sendSimulatedInput(std::vector<INPUT> &inputList)
        {
            messageDispatcher_->DispatchInput(inputList);
        }

        IInputSimulator *inputSimulator_;
        IInputMessageDispatcher *messageDispatcher_;
        int mouseWheelClickSize_;
    };
}