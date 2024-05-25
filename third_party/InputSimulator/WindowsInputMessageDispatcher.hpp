#pragma once
#include <Windows.h>
#include <exception>
#include <stdexcept>

#include "IInputMessageDispatcher.hpp"

namespace WindowsInput
{
    class WindowsInputMessageDispatcher : public IInputMessageDispatcher
    {
    public:
        void DispatchInput(std::vector<INPUT> inputs) override
        {
            if (inputs.empty())
                return;
            UINT count = static_cast<UINT>(inputs.size());
            UINT successful = SendInput(count, inputs.data(), sizeof(INPUT));
            if (successful != count)
            {
                throw std::runtime_error(
                    "Some simulated input commands were not sent successfully. "
                    "The most common reason for this happening are the security features of "
                    "Windows including User Interface Privacy Isolation (UIPI). "
                    "Your application can only send commands to applications of "
                    "the same or lower elevation. Similarly certain commands are restricted to "
                    "Accessibility/UIAutomation applications. "
                    "Refer to the project home page and the code samples for more information.");
            }
        }
    };
}