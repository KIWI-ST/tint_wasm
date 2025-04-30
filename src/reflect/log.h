#pragma once

void PrintLog(const char format [], ...);
void PrintError(const char format [], ...);

#define LOGI(...) gems::core::PrintLog(__VA_ARGS__);
#define LOGE(...) gems::core::PrintError(__VA_ARGS__);
