#pragma once
#include <Windows.h>
#include <vector>
#include <string>

#include "VirtualKeyCode.hpp"
#include "MouseButton.hpp"

namespace WindowsInput
{
    class InputBuilder : public std::vector<INPUT>
    {
    public:
        static bool IsExtendedKey(VirtualKeyCode keyCode)
        {
            return keyCode == VirtualKeyCode::MENU || keyCode == VirtualKeyCode::RMENU || keyCode == VirtualKeyCode::CONTROL_ || keyCode == VirtualKeyCode::RCONTROL ||
                   keyCode == VirtualKeyCode::INSERT || keyCode == VirtualKeyCode::DELETE_ || keyCode == VirtualKeyCode::HOME || keyCode == VirtualKeyCode::END ||
                   keyCode == VirtualKeyCode::PRIOR || keyCode == VirtualKeyCode::NEXT || keyCode == VirtualKeyCode::RIGHT || keyCode == VirtualKeyCode::UP ||
                   keyCode == VirtualKeyCode::LEFT || keyCode == VirtualKeyCode::DOWN || keyCode == VirtualKeyCode::NUMLOCK || keyCode == VirtualKeyCode::CANCEL ||
                   keyCode == VirtualKeyCode::SNAPSHOT || keyCode == VirtualKeyCode::DIVIDE;
        }

        std::vector<INPUT> &ToArray()
        {
            return *this;
        }

        InputBuilder &AddKeyDown(VirtualKeyCode keyCode)
        {
            INPUT down = {};
            down.type = INPUT_KEYBOARD;
            down.ki.dwFlags = IsExtendedKey(keyCode) ? KEYEVENTF_EXTENDEDKEY : 0;
            down.ki.wVk = (WORD)keyCode;
            down.ki.wScan = (WORD)MapVirtualKey((UINT)keyCode, 0) & 0xFF;
            down.ki.time = 0;
            down.ki.dwExtraInfo = 0;
            emplace_back(down);
            return *this;
        }

        InputBuilder &AddKeyUp(VirtualKeyCode keyCode)
        {
            INPUT up = {};
            up.type = INPUT_KEYBOARD;
            up.ki.dwFlags = IsExtendedKey(keyCode) ? KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP : KEYEVENTF_KEYUP;
            up.ki.wVk = (WORD)keyCode;
            up.ki.wScan = (WORD)MapVirtualKey((UINT)keyCode, 0) & 0xFF;
            up.ki.time = 0;
            up.ki.dwExtraInfo = 0;
            emplace_back(up);
            return *this;
        }

        InputBuilder &AddKeyPress(VirtualKeyCode keyCode)
        {
            AddKeyDown(keyCode);
            AddKeyUp(keyCode);
            return *this;
        }

        InputBuilder &AddCharacter(char character)
        {
            USHORT scanCode = character;
            INPUT down = {};
            down.type = INPUT_KEYBOARD;
            down.ki.dwFlags = KEYEVENTF_UNICODE;
            down.ki.wVk = 0;
            down.ki.wScan = scanCode;
            down.ki.time = 0;
            down.ki.dwExtraInfo = 0;
            emplace_back(down);

            INPUT up = {};
            up.type = INPUT_KEYBOARD;
            up.ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
            up.ki.wVk = 0;
            up.ki.wScan = scanCode;
            up.ki.time = 0;
            up.ki.dwExtraInfo = 0;
            if ((scanCode & 0xFF00) == 0xE000)
            {
                down.ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;
                up.ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;
            }
            emplace_back(up);
            return *this;
        }

        InputBuilder &AddCharacters(const std::string &characters)
        {
            for (char c : characters)
            {
                AddCharacter(c);
            }
            return *this;
        }

        InputBuilder &AddRelativeMouseMovement(int x, int y)
        {
            INPUT movement = {};
            movement.type = INPUT_MOUSE;
            movement.mi.dwFlags = MOUSEEVENTF_MOVE;
            movement.mi.dx = x;
            movement.mi.dy = y;
            emplace_back(movement);
            return *this;
        }

        InputBuilder &AddAbsoluteMouseMovement(int absoluteX, int absoluteY)
        {
            INPUT movement = {};
            movement.type = INPUT_MOUSE;
            movement.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
            movement.mi.dx = absoluteX;
            movement.mi.dy = absoluteY;
            emplace_back(movement);
            return *this;
        }

        InputBuilder &AddAbsoluteMouseMovementOnVirtualDesktop(int absoluteX, int absoluteY)
        {
            INPUT movement = {};
            movement.type = INPUT_MOUSE;
            movement.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_VIRTUALDESK;
            movement.mi.dx = absoluteX;
            movement.mi.dy = absoluteY;
            emplace_back(movement);
            return *this;
        }

        InputBuilder &AddMouseButtonDown(MouseButton button)
        {
            INPUT buttonDown = {};
            buttonDown.type = INPUT_MOUSE;
            buttonDown.mi.dwFlags = ToMouseButtonDownFlag(button);
            emplace_back(buttonDown);
            return *this;
        }

        InputBuilder &AddMouseXButtonDown(int xButtonId)
        {
            INPUT buttonDown = {};
            buttonDown.type = INPUT_MOUSE;
            buttonDown.mi.dwFlags = MOUSEEVENTF_XDOWN;
            buttonDown.mi.mouseData = (DWORD)xButtonId;
            emplace_back(buttonDown);
            return *this;
        }

        InputBuilder &AddMouseButtonUp(MouseButton button)
        {
            INPUT buttonUp = {};
            buttonUp.type = INPUT_MOUSE;
            buttonUp.mi.dwFlags = ToMouseButtonUpFlag(button);
            emplace_back(buttonUp);
            return *this;
        }

        InputBuilder &AddMouseXButtonUp(int xButtonId)
        {
            INPUT buttonUp = {};
            buttonUp.type = INPUT_MOUSE;
            buttonUp.mi.dwFlags = MOUSEEVENTF_XUP;
            buttonUp.mi.mouseData = (DWORD)xButtonId;
            emplace_back(buttonUp);
            return *this;
        }

        InputBuilder &AddMouseButtonClick(MouseButton button)
        {
            return AddMouseButtonDown(button).AddMouseButtonUp(button);
        }

        InputBuilder &AddMouseXButtonClick(int xButtonId)
        {
            return AddMouseXButtonDown(xButtonId).AddMouseXButtonUp(xButtonId);
        }

        InputBuilder &AddMouseButtonDoubleClick(MouseButton button)
        {
            return AddMouseButtonClick(button).AddMouseButtonClick(button);
        }

        InputBuilder &AddMouseXButtonDoubleClick(int xButtonId)
        {
            return AddMouseXButtonClick(xButtonId).AddMouseXButtonClick(xButtonId);
        }

        InputBuilder &AddMouseVerticalWheelScroll(int scrollAmount)
        {
            INPUT scroll = {};
            scroll.type = INPUT_MOUSE;
            scroll.mi.dwFlags = MOUSEEVENTF_WHEEL;
            scroll.mi.mouseData = (DWORD)scrollAmount;
            emplace_back(scroll);
            return *this;
        }

        InputBuilder &AddMouseHorizontalWheelScroll(int scrollAmount)
        {
            INPUT scroll = {};
            scroll.type = INPUT_MOUSE;
            scroll.mi.dwFlags = MOUSEEVENTF_HWHEEL;
            scroll.mi.mouseData = (DWORD)scrollAmount;
            emplace_back(scroll);
            return *this;
        }

    private:
        //  enum class MOUSE_EVENT_FLAGS : unsigned int
        // {
        // 	MOUSEEVENTF_ABSOLUTE = 0x00008000,
        // 	MOUSEEVENTF_LEFTDOWN = 0x00000002,
        // 	MOUSEEVENTF_LEFTUP = 0x00000004,
        // 	MOUSEEVENTF_MIDDLEDOWN = 0x00000020,
        // 	MOUSEEVENTF_MIDDLEUP = 0x00000040,
        // 	MOUSEEVENTF_MOVE = 0x00000001,
        // 	MOUSEEVENTF_RIGHTDOWN = 0x00000008,
        // 	MOUSEEVENTF_RIGHTUP = 0x00000010,
        // 	MOUSEEVENTF_WHEEL = 0x00000800,
        // 	MOUSEEVENTF_XDOWN = 0x00000080,
        // 	MOUSEEVENTF_XUP = 0x00000100,
        // 	MOUSEEVENTF_HWHEEL = 0x00001000,
        // 	MOUSEEVENTF_MOVE_NOCOALESCE = 0x00002000,
        // 	MOUSEEVENTF_VIRTUALDESK = 0x00004000,
        // };
        static unsigned int /*MOUSE_EVENT_FLAGS*/ ToMouseButtonDownFlag(MouseButton button)
        {
            switch (button)
            {
            case MouseButton::LeftButton:
                return MOUSEEVENTF_LEFTDOWN;
            case MouseButton::MiddleButton:
                return MOUSEEVENTF_MIDDLEDOWN;
            case MouseButton::RightButton:
                return MOUSEEVENTF_RIGHTDOWN;
            default:
                return MOUSEEVENTF_LEFTDOWN;
            }
        }
        static unsigned int /*MOUSE_EVENT_FLAGS*/ ToMouseButtonUpFlag(MouseButton button)
        {
            switch (button)
            {
            case MouseButton::LeftButton:
                return MOUSEEVENTF_LEFTUP;
            case MouseButton::MiddleButton:
                return MOUSEEVENTF_MIDDLEUP;
            case MouseButton::RightButton:
                return MOUSEEVENTF_RIGHTUP;
            default:
                return MOUSEEVENTF_LEFTUP;
            }
        }
    };

}