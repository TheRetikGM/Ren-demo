#include <Ren/Engine.h>
#include <stdexcept>
#include <exception>
#include <chrono>
#define MAX_LOG_ENTRIES 1000

////////////////////////////////////////////////////
////////////////////// Logger //////////////////////
////////////////////////////////////////////////////

template<typename T, std::size_t MAX_ENTRIES>
class LoggerArray
{
    T mEntries[MAX_ENTRIES];
    uint32_t mStart = 0;
    uint32_t mEnd = 0;
public:
    inline size_t size() { return mEnd - mStart; }
    void push_back(const T& entry) 
    {
        if (mEnd >= MAX_ENTRIES)
            mStart++;

        mEntries[mEnd % MAX_ENTRIES] = entry;
        mEnd++;

        if (mStart >= MAX_ENTRIES)
        {
            mStart = 0;
            mEnd = MAX_ENTRIES;
        }
    }
    inline void clear() { mStart = mEnd = 0; }
    inline T& operator[] (const int& index) { return mEntries[(mStart + index) % MAX_ENTRIES]; }
    inline T& at(const int& index) { return mEntries.at((mStart + index) % MAX_ENTRIES); }


    class iterator
    {
        uint32_t index = 0;
        LoggerArray* pArr = nullptr;
    public:
        iterator() = default;
        iterator(uint32_t index, LoggerArray* array) : index(index), pArr(array) {}

        bool operator==(const iterator& it) { return index == it.index; }
        bool operator!=(const iterator& it) { return index != it.index; }
        iterator& operator++() { index++; return *this; }
        iterator& operator--() { index--; return *this; }
        T& operator->() { return (*pArr)[index]; }
        T& operator*() { return (*pArr)[index]; }
    };

    iterator begin() { return {0, this}; }
    iterator end() { return {(uint32_t)size(), this}; }
};

class GUILogger : public Ren::LogEntryHandler
{
    struct EntryWrapper { std::string timestamp; Ren::LogEntry entry; };
    LoggerArray<EntryWrapper, MAX_LOG_ENTRIES> mLogs;
    bool mAutoScroll = true;
    bool mScrollBottom = false;
    bool mPauseLogging = false;
public:
    void HandleEntry(const Ren::LogEntry& entry) override
    {
        if (mPauseLogging)
            return;
        mLogs.push_back({getTimestamp(), entry});
        if (mAutoScroll)
            mScrollBottom = true;
    }

    void Draw()
    {
        ImGui::Begin("Logger");
        ImVec2 a_size = ImGui::GetContentRegionMax();
        ImGuiTableFlags flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Hideable;
        if (ImGui::BeginTable("table_log", 4, flags, ImVec2(0.0f, a_size.y - ImGui::GetTextLineHeight() * 2.0f)))
        {
            if (ImGui::BeginPopupContextWindow())
            {
                ImGui::MenuItem("Autoscroll", "", &mAutoScroll);
                ImGui::MenuItem("Pause logging", "", &mPauseLogging);
                ImGui::Separator();
                if (ImGui::Selectable("Clear logs"))
                    mLogs.clear();
                ImGui::EndPopup();
            }
            ImGui::TableSetupScrollFreeze(0, 1);
            ImGui::TableSetupColumn("Timestamp");
            ImGui::TableSetupColumn("File");
            ImGui::TableSetupColumn("Type");
            ImGui::TableSetupColumn("Log");
            ImGui::TableHeadersRow();

            ImGuiListClipper clipper;
            clipper.Begin((int)mLogs.size());
            while (clipper.Step())
            {
                for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
                {
                    EntryWrapper& e = mLogs[i];

                    ImGui::TableNextRow();                    
                    ImGui::TableNextColumn();
                    ImGui::Text(e.timestamp.c_str());

                    ImGui::TableNextColumn();
                    ImGui::Text("%s:%i", e.entry.file.c_str(), e.entry.line);

                    ImGui::TableNextColumn();
                    if (e.entry.type == Ren::LogType::Warning)
                        ImGui::TextColored({1.0f, 1.0f, 0.0f, 1.0f}, "Warning");
                    else if (e.entry.type == Ren::LogType::Error)
                        ImGui::TextColored({1.0f, 0.0f, 0.0f, 1.0f}, "Error");
                    else if (e.entry.type == Ren::LogType::Info)
                        ImGui::TextColored({0.0f, 1.0f, 1.0f, 1.0f}, "Info");
                    else
                        ImGui::TextColored({1.0f, 0.0f, 1.0f, 1.0f}, "Critical");

                    ImGui::TableNextColumn();
                    ImGui::Text(e.entry.message.c_str());
                }
            }

            if (mScrollBottom && mAutoScroll)
                ImGui::SetScrollHereY(1.0f);
            mScrollBottom = false;

            ImGui::EndTable();
        }
        ImGui::End();
    }
private:

    std::string getTimestamp()
    {
        auto now = std::chrono::system_clock::now();
        auto seconds = std::chrono::time_point_cast<std::chrono::seconds>(now);
        auto fraction = now - seconds;
        std::time_t cnow = std::chrono::system_clock::to_time_t(now);
        std::tm* tm = std::localtime(&cnow);
        uint32_t millisec = std::chrono::duration_cast<std::chrono::milliseconds>(fraction).count();

        // 2022-06-15 00:00:00.000  --> 23 characters
        char buf[100] = {};
        std::sprintf(buf, "%04i-%02i-%02i %02i:%02i:%02i.%03i", 
            tm->tm_year + 1900,
            tm->tm_mon + 1,
            tm->tm_mday,
            tm->tm_hour,
            tm->tm_min,
            tm->tm_sec,
            millisec
        );
        buf[23] = '\0';

        return std::string(buf);
    }
};

/////////////////////////////////////////////////////
///////////////// Debug GUI handler /////////////////
/////////////////////////////////////////////////////
// class DebugGUI
// {
// public:

//     static void 
// };