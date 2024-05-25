#pragma once
#include <Windows.h>

#include "IInputMessageDispatcher.hpp"

namespace WindowsInput
{
    class WindowsInputPostMessageDispatcher : public IInputMessageDispatcher
    {
    public:
        HWND targetWindow;
        void DispatchInput(std::vector<INPUT> inputs) override
        {
            if (inputs.empty())
                return;
            PostMessageW(targetWindow, WM_ACTIVATE, 1, 0);
            for (auto input : inputs)
            {
                // PostMessageW(targetWindow, WM_INPUT, 0, (LPARAM)&input);kccc
                if (input.type == INPUT_KEYBOARD)
                {
                    if (input.ki.dwFlags & KEYEVENTF_KEYUP)
                    {
                        PostMessageW(targetWindow, WM_KEYUP, input.ki.wVk, (int)0x1e0001);
                    }
                    else
                    {
                        PostMessageW(targetWindow, WM_KEYDOWN, input.ki.wVk, (int)0x1e0001);
                    }
                }
                else if (input.type == INPUT_MOUSE)
                {
                    if (input.mi.dwFlags & MOUSEEVENTF_ABSOLUTE)
                    {
                        PostMessageW(targetWindow, WM_MOUSEMOVE, 0, MAKELPARAM(input.mi.dx, input.mi.dy));
                    }
                    else
                    {
                        if (input.mi.dwFlags & MOUSEEVENTF_MOVE)
                        {
                            PostMessageW(targetWindow, WM_MOUSEMOVE, 0, MAKELPARAM(input.mi.dx, input.mi.dy));
                        }
                        if (input.mi.dwFlags & MOUSEEVENTF_LEFTDOWN)
                        {
                            PostMessageW(targetWindow, WM_LBUTTONDOWN, 0, MAKELPARAM(input.mi.dx, input.mi.dy));
                        }
                        if (input.mi.dwFlags & MOUSEEVENTF_LEFTUP)
                        {
                            PostMessageW(targetWindow, WM_LBUTTONUP, 0, MAKELPARAM(input.mi.dx, input.mi.dy));
                        }
                        if (input.mi.dwFlags & MOUSEEVENTF_RIGHTDOWN)
                        {
                            PostMessageW(targetWindow, WM_RBUTTONDOWN, 0, MAKELPARAM(input.mi.dx, input.mi.dy));
                        }
                        if (input.mi.dwFlags & MOUSEEVENTF_RIGHTUP)
                        {
                            PostMessageW(targetWindow, WM_RBUTTONUP, 0, MAKELPARAM(input.mi.dx, input.mi.dy));
                        }
                        if (input.mi.dwFlags & MOUSEEVENTF_MIDDLEDOWN)
                        {
                            PostMessageW(targetWindow, WM_MBUTTONDOWN, 0, MAKELPARAM(input.mi.dx, input.mi.dy));
                        }
                        if (input.mi.dwFlags & MOUSEEVENTF_MIDDLEUP)
                        {
                            PostMessageW(targetWindow, WM_MBUTTONUP, 0, MAKELPARAM(input.mi.dx, input.mi.dy));
                        }
                        if (input.mi.dwFlags & MOUSEEVENTF_XDOWN)
                        {
                            PostMessageW(targetWindow, WM_XBUTTONDOWN, input.mi.mouseData, MAKELPARAM(input.mi.dx, input.mi.dy));
                        }
                        if (input.mi.dwFlags & MOUSEEVENTF_XUP)
                        {
                            PostMessageW(targetWindow, WM_XBUTTONUP, input.mi.mouseData, MAKELPARAM(input.mi.dx, input.mi.dy));
                        }
                        if (input.mi.dwFlags & MOUSEEVENTF_WHEEL)
                        {
                            PostMessageW(targetWindow, WM_MOUSEWHEEL, input.mi.mouseData, MAKELPARAM(input.mi.dx, input.mi.dy));
                        }
                        if (input.mi.dwFlags & MOUSEEVENTF_HWHEEL)
                        {
                            PostMessageW(targetWindow, WM_MOUSEHWHEEL, input.mi.mouseData, MAKELPARAM(input.mi.dx, input.mi.dy));
                        }
                    }
                }
            }
        }
    };
}