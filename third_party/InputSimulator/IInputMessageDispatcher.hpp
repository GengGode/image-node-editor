#pragma once
#include <Windows.h>
#include <vector>

namespace WindowsInput
{
    class IInputMessageDispatcher
    {
    public:
        virtual void DispatchInput(std::vector<INPUT> inputs) = 0;
    };
}