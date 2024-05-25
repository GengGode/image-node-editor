#pragma once
#include <chrono>
#include <string>
#include <vector>
#include <exception>
#include <thread>
#include <algorithm>

#include "IKeyboardSimulator.hpp"
#include "IInputSimulator.hpp"
#include "IInputMessageDispatcher.hpp"
#include "WindowsInputMessageDispatcher.hpp"
#include "InputBuilder.hpp"

namespace WindowsInput
{
    class KeyboardSimulator : public IKeyboardSimulator
    {
    private:
        IInputSimulator *inputSimulator_;
        IInputMessageDispatcher *messageDispatcher_;

    public:
        KeyboardSimulator(IInputSimulator *inputSimulator)
            : inputSimulator_(inputSimulator)
        {
            messageDispatcher_ = new WindowsInputMessageDispatcher();
        }

        KeyboardSimulator(IInputSimulator *inputSimulator, IInputMessageDispatcher *messageDispatcher)
            : inputSimulator_(inputSimulator), messageDispatcher_(messageDispatcher)
        {
            if (!messageDispatcher)
            {
                throw std::invalid_argument("Message dispatcher cannot be null");
            }
        }

        ~KeyboardSimulator()
        {
            delete messageDispatcher_;
        }

        IMouseSimulator *GetMouseSimulator() override
        {
            return &inputSimulator_->GetMouseSimulator();
        }

    private:
        static void ModifiersDown(InputBuilder &builder, const std::vector<VirtualKeyCode> &modifierKeyCodes)
        {
            for (auto keyCode : modifierKeyCodes)
            {
                builder.AddKeyDown(keyCode);
            }
        }

        static void ModifiersUp(InputBuilder &builder, const std::vector<VirtualKeyCode> &modifierKeyCodes)
        {
            // for (auto keyCode : modifierKeyCodes | std::views::reverse) {
            //     builder.AddKeyUp(keyCode);
            // }

            // c++17
            std::vector<VirtualKeyCode> reversedModifierKeyCodes = modifierKeyCodes;
            std::reverse(reversedModifierKeyCodes.begin(), reversedModifierKeyCodes.end());

            for (auto keyCode : reversedModifierKeyCodes)
            {
                builder.AddKeyUp(keyCode);
            }
        }

        static void KeysPress(InputBuilder &builder, const std::vector<VirtualKeyCode> &keyCodes)
        {
            for (auto keyCode : keyCodes)
            {
                builder.AddKeyPress(keyCode);
            }
        }

        void SendSimulatedInput(std::vector<INPUT> &inputList)
        {
            messageDispatcher_->DispatchInput(inputList);
        }

    public:
        IKeyboardSimulator &KeyDown(VirtualKeyCode keyCode)
        {
            std::vector<INPUT> inputList = InputBuilder().AddKeyDown(keyCode).ToArray();
            SendSimulatedInput(inputList);
            return *this;
        }

        IKeyboardSimulator &KeyDown(const std::vector<VirtualKeyCode> &keyCodes)
        {
            InputBuilder builder;
            for (auto keyCode : keyCodes)
            {
                builder.AddKeyDown(keyCode);
            }
            SendSimulatedInput(builder.ToArray());
            return *this;
        }

        IKeyboardSimulator &KeyUp(VirtualKeyCode keyCode)
        {
            std::vector<INPUT> inputList = InputBuilder().AddKeyUp(keyCode).ToArray();
            SendSimulatedInput(inputList);
            return *this;
        }

        IKeyboardSimulator &KeyUp(const std::vector<VirtualKeyCode> &keyCodes)
        {
            InputBuilder builder;

            // for (auto keyCode : keyCodes | std::views::reverse) {
            //     builder.AddKeyUp(keyCode);
            // }

            // c++17
            std::vector<VirtualKeyCode> reversedKeyCodes = keyCodes;
            std::reverse(reversedKeyCodes.begin(), reversedKeyCodes.end());

            for (auto keyCode : reversedKeyCodes)
            {
                builder.AddKeyUp(keyCode);
            }

            SendSimulatedInput(builder.ToArray());
            return *this;
        }

        IKeyboardSimulator &KeyPress(VirtualKeyCode keyCode)
        {
            std::vector<INPUT> inputList = InputBuilder().AddKeyPress(keyCode).ToArray();
            SendSimulatedInput(inputList);
            return *this;
        }

        IKeyboardSimulator &KeyPress(const std::vector<VirtualKeyCode> &keyCodes)
        {
            InputBuilder builder;
            KeysPress(builder, keyCodes);
            SendSimulatedInput(builder.ToArray());
            return *this;
        }

        IKeyboardSimulator &ModifiedKeyStroke(VirtualKeyCode modifierKeyCode, VirtualKeyCode keyCode)
        {
            return ModifiedKeyStroke(std::vector<VirtualKeyCode>{modifierKeyCode}, std::vector<VirtualKeyCode>{keyCode});
        }

        IKeyboardSimulator &ModifiedKeyStroke(const std::vector<VirtualKeyCode> &modifierKeyCodes, VirtualKeyCode keyCode)
        {
            return ModifiedKeyStroke(modifierKeyCodes, std::vector<VirtualKeyCode>{keyCode});
        }

        IKeyboardSimulator &ModifiedKeyStroke(VirtualKeyCode modifierKey, const std::vector<VirtualKeyCode> &keyCodes)
        {
            return ModifiedKeyStroke(std::vector<VirtualKeyCode>{modifierKey}, keyCodes);
        }

        IKeyboardSimulator &ModifiedKeyStroke(const std::vector<VirtualKeyCode> &modifierKeyCodes, const std::vector<VirtualKeyCode> &keyCodes)
        {
            InputBuilder builder;
            ModifiersDown(builder, modifierKeyCodes);
            KeysPress(builder, keyCodes);
            ModifiersUp(builder, modifierKeyCodes);
            SendSimulatedInput(builder.ToArray());
            return *this;
        }

        IKeyboardSimulator &TextEntry(const std::string &text)
        {
            InputBuilder builder;
            builder.AddCharacters(text);
            SendSimulatedInput(builder.ToArray());
            return *this;
        }

        IKeyboardSimulator &TextEntry(char character)
        {
            InputBuilder builder;
            builder.AddCharacter(character);
            SendSimulatedInput(builder.ToArray());
            return *this;
        }

        IKeyboardSimulator &Sleep(int millsecondsTimeout)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(millsecondsTimeout));
            return *this;
        }

        IKeyboardSimulator &Sleep(const std::chrono::duration<int, std::milli> &timeout)
        {
            std::this_thread::sleep_for(timeout);
            return *this;
        }
    };
}