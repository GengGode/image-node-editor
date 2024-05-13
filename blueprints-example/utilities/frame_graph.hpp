#pragma once
#include <imgui.h>
#include <imgui_widget_flamegraph.h>

#include <array>
#include <chrono>


class Profiler
{
public:
    enum Stage
    {
        SdlInput,
        Plot,
        NewFrame,
        DemoWindow,
        SampleWindow,
        AnotherWindow,
        ProfilerWindow,
        ImGuiRender,
        OpenGL,
        ImGuiRenderOpenGL,
        SwapWindow,
        Rendering,

        _StageCount,
    };

    struct Scope
    {
        ImU8 _level;
        std::chrono::system_clock::time_point _start;
        std::chrono::system_clock::time_point _end;
        bool _finalized = false;
    };

    struct Entry
    {
        std::chrono::system_clock::time_point _frameStart;
        std::chrono::system_clock::time_point _frameEnd;
        std::array<Scope, _StageCount> _stages;
    };

    void Frame()
    {
        auto &prevEntry = _entries[_currentEntry];
        _currentEntry = (_currentEntry + 1) % _bufferSize;
        prevEntry._frameEnd = _entries[_currentEntry]._frameStart = std::chrono::system_clock::now();
    }

    void Begin(Stage stage)
    {
        assert(_currentLevel < 255);
        auto &entry = _entries[_currentEntry]._stages[stage];
        entry._level = _currentLevel;
        _currentLevel++;
        entry._start = std::chrono::system_clock::now();
        entry._finalized = false;
    }

    void End(Stage stage)
    {
        assert(_currentLevel > 0);
        auto &entry = _entries[_currentEntry]._stages[stage];
        assert(!entry._finalized);
        _currentLevel--;
        assert(entry._level == _currentLevel);
        entry._end = std::chrono::system_clock::now();
        entry._finalized = true;
    }

    ImU8 GetCurrentEntryIndex() const
    {
        return (_currentEntry + _bufferSize - 1) % _bufferSize;
    }

    static const ImU8 _bufferSize = 100;
    std::array<Entry, _bufferSize> _entries;

private:
    ImU8 _currentEntry = _bufferSize - 1;
    ImU8 _currentLevel = 0;
};

static const std::array<const char *, Profiler::_StageCount> stageNames = {
    "SDL Input",
    "Plot",
    "New Frame",
    "Demo Window",
    "Sample Window",
    "Another Window",
    "Profiler Window",
    "ImGui::Render",
    "OpenGL",
    "ImGui_ImplOpenGL3_RenderDrawData",
    "SDL_GL_SwapWindow",
    "Rendering",
};

static void ProfilerValueGetter(float *startTimestamp, float *endTimestamp, ImU8 *level, const char **caption, const void *data, int idx)
{
    auto entry = reinterpret_cast<const Profiler::Entry *>(data);
    auto &stage = entry->_stages[idx];
    if (startTimestamp)
    {
        std::chrono::duration<float, std::milli> fltStart = stage._start - entry->_frameStart;
        *startTimestamp = fltStart.count();
    }
    if (endTimestamp)
    {
        *endTimestamp = stage._end.time_since_epoch().count() / 1000000.0f;

        std::chrono::duration<float, std::milli> fltEnd = stage._end - entry->_frameStart;
        *endTimestamp = fltEnd.count();
    }
    if (level)
    {
        *level = stage._level;
    }
    if (caption)
    {
        *caption = stageNames[idx];
    }
}