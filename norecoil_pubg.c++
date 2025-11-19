#include <windows.h>
#include <thread>
#include <atomic>
#include <chrono>
#include <iostream>
#include <cmath>
#include <map>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

using namespace std;

// ====== Struct lưu setting súng ======
struct GunSetting {
    int baseStrength;
    double maxRadius;
    double returnSpeed;
};

// ====== Danh sách preset súng ======
std::map<int, GunSetting> gunPresets = {
    { 1,  {7, 0.1, 0.1}},  
    { 2,  {8, 0.1, 0.1}},  
    { 3,  {7, 0.1, 0.1}},  
    { 4,  {12, 0.1, 0.1}},  
    { 5,  {12, 0.1, 0.1}},  
    { 6,  {8, 0.1, 0.1}}, 
    { 7,  {10, 0.1, 0.1}}, 
    { 8,  {8, 0.1, 0.1}},
    { 9,  {8, 0.1, 0.1}},
    { 10, {10, 0.1, 0.1}},
    { 11, {14, 0.1, 0.1}},
    { 12, {0, 0.1, 0.1}},
    { 13, {9, 0.1, 0.1}},
    { 14, {0, 0.1, 0.1}},
    { 15, {10, 0.1, 0.1}},
    { 16, {0, 0.1, 0.1}},
    { 17, {0, 0.1, 0.1}},
    { 18, {12, 0.1, 0.1}},
    { 19, {0, 0.1, 0.1}},
    { 20, {11, 0.1, 0.1}},
    { 21, {0, 0.1, 0.1}},
    { 22, {0, 0.1, 0.1}},
    { 23, {0, 0.1, 0.1}},
    { 24, {0, 0.1, 0.1}},
    { 25, {0, 0.1, 0.1}},
    { 26, {0, 0.1, 0.1}},
    { 27, {0, 0.1, 0.1}},
    { 28, {0, 0.1, 0.1}},
    { 29, {45, 0.1, 0.1}},
    { 30, {48, 0.1, 0.1}},
    { 31, {0, 0.1, 0.1}},
    { 32, {0, 0.1, 0.1}},
    { 33, {0, 0.1, 0.1}},
    { 34, {0, 0.1, 0.1}},
    { 35, {0, 0.1, 0.1}},
    { 36, {0, 0.1, 0.1}},
    { 37, {0, 0.1, 0.1}},
    { 38, {0, 0.1, 0.1}},
    { 39, {0, 0.1, 0.1}},
};

std::map<int, std::string> gunNames = {
    {1, "PP-19 Bizon - 7"},
    {2, "Micro UZI - 8"},
    {3, "UMP45 - 7"},
    {4, "Tommy - 12" },
    {5, "Vector - 12"},
    {6, "MP5K - 8"},
    {7, "P90 - 10" },
    {8, "JS9 - 8"},
    {9, "MP9 - 8"},
    {10, "AKM - 10"},
    {11, "Beryl - 14"},
    {12, ""},
    {13, "M416 - 9"},
    {14, ""},
    {15, "SCAR-L - 10"},
    {16, ""},
    {17, ""},
    {18, "AUG - 12"},
    {19, ""},
    {20, "ACE - 11"},
    {21, ""},
    {22, ""},
    {23, ""},
    {24, ""},
    {25, ""},
    {26, ""},
    {27, ""},
    {28, ""},
    {29, "SLR - 45"},
    {30, "Mini14 - 48"},
    {31, ""},
    {32, ""},
    {33, ""},
    {34, ""},
    {35, ""},
	{36, ""},
    {37, ""},
    {38, ""},
    {39, ""}
};

// ====== Biến toàn cục ======
std::atomic<bool> macroOn(false);
std::atomic<bool> running(true);
std::atomic<int> baseStrength(1);
double maxRadius = 0.15;
double returnSpeed = 0.01;
double currentX = 0.0;
int direction = 1;
bool movingBack = false;

// ====== Hàm update ngang ======
void updateHorizontal() {
    if (!movingBack) {
        currentX = maxRadius * direction;
        movingBack = true;
    }
    else {
        if (direction > 0) currentX -= returnSpeed;
        else currentX += returnSpeed;

        if ((direction > 0 && currentX <= 0) || (direction < 0 && currentX >= 0)) {
            currentX = 0;
            movingBack = false;
            direction *= -1;
        }
    }
}

// ====== Hàm tính độ giật dọc ======
int getVerticalStrength(int elapsedMs) {
    int elapsedSec = elapsedMs / 1000;
    double multiplier = 1.0;

    switch (elapsedSec) {
    case 0: multiplier = 1.00; break;
    case 1: multiplier = 1.10; break;
    case 2: multiplier = 1.15; break;
    case 3: multiplier = 1.05; break;
    case 4: multiplier = 1.10; break;
    default: multiplier = 1.00; break;
    }
    return static_cast<int>(baseStrength * multiplier);
}

// ====== Macro thread ======
void macroThread() {
    while (running) {
        if (macroOn && (GetAsyncKeyState(VK_LBUTTON) & 0x8000)) {
            auto start = std::chrono::steady_clock::now();

            while ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) && macroOn) {
                auto now = std::chrono::steady_clock::now();
                int elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();

                int vertical = getVerticalStrength(elapsedMs);
                updateHorizontal();

                mouse_event(MOUSEEVENTF_MOVE, static_cast<int>(currentX), vertical, 0, 0);
                std::this_thread::sleep_for(std::chrono::milliseconds(16));
            }

            currentX = 0;
            movingBack = false;
            direction = 1;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

// ====== Hotkey thread ======
void hotkeyThread() {
    while (running) {
        if (GetAsyncKeyState(VK_F1) & 1) {
            macroOn = !macroOn;
            std::cout << "[Macro] " << (macroOn ? "ON: Enable" : "OFF: Disable") << std::endl;
        }
        if (GetAsyncKeyState(VK_F4) & 1) {
            baseStrength++;
            std::cout << "[Macro]: +" << baseStrength << std::endl;
        }
        if (GetAsyncKeyState(VK_F3) & 1) {
            if (baseStrength > 1) baseStrength--;
            std::cout << "[Macro]: -" << baseStrength << std::endl;
        }
        if (GetAsyncKeyState(VK_F5) & 1) {
            /*std::cout << "\n=== Chon sung ===" << std::endl;*/
            /*for (auto& p : gunNames) {
                std::cout << p.first << ". " << p.second << std::endl;
            }*/
			//Bam F5 de chon sung
            int choice;
			// Hiển thị menu chọn súng
            cout<<"\n";
            std::cout << "|-----------------------------------------------------------------------------------------------------------------|\n";
            std::cout << "|-------------------------------------------Moi ban chon Macro loai sung------------------------------------------|\n";
            std::cout << "|-----------------------------------------------------------------------------------------------------------------|\n";
			std::cout << "|        SMG	      |         AR           |         SR                 |         DMR         |         LMG       |\n";
            std::cout << "|-----------------------------------------------------------------------------------------------------------------|\n";
            std::cout << "|   *   1.Bizon     |   *   10.AKM         |   *  23.Kar98k             |   *  29.SLR         |    *  37.DP-28    |\n";
            std::cout << "|   *   2.Tommy     |   *   11.Beryl M762  |   *  24.M24                |   *  30.Mini14      |    *  38.M249     |\n";
            std::cout << "|   *   3.UMP       |       12.G36C        |   *  25.AWM                |   *  31.SKS         |    *  39.MG3      |\n";
            std::cout << "|   *   4.UZI       |   *   13.M416        |   *  26.Lynx AMR           |      32.VSS         |                   |\n";
            std::cout << "|   *   5.Vector    |       14.M16A4       |      27.Win94              |   *  33.QBU         |                   |\n";
            std::cout << "|   *   6.MP5K      |   *   15.SCAR-L      |   *  28.Sung Truong Mosin  |   *  34.Mk14        |                   |\n";
            std::cout << "|       7.P90       |       16.Mk47 Mutant |                            |   *  35.Mk12        |                   |\n";
            std::cout << "|       8.JS9       |   *   17.QBZ         |                            |   *  36.Dragunov    |                   |\n";
            std::cout << "|       9.MP9       |   *   18.AUG         |                            |                     |                   |\n";
            std::cout << "|                   |   *   19.Groza       |                            |                     |                   |\n";
            std::cout << "|                   |   *   20.ACE32       |                            |                     |                   |\n";
            std::cout << "|                   |       21.K2          |                            |                     |                   |\n";
            std::cout << "|                   |       22.FAMAS       |                            |                     |                   |\n";
            std::cout << "|-----------------------------------------------------------------------------------------------------------------|\n";
            cout << "Note: Nhung sung co danh dau '*' la nhung sung thuong hay su dung.\n";
            std::cout << "\nNhap so tuong ung: ";
            cin >> choice;
            if (gunPresets.count(choice)) {
                baseStrength = gunPresets[choice].baseStrength;
                maxRadius = gunPresets[choice].maxRadius;
                returnSpeed = gunPresets[choice].returnSpeed;
                std::cout << "[Macro] sung: " << gunNames[choice] << " da chon !" << std::endl;
            }
            else {
                std::cout << "Sung ban chon khong co trong danh sach, vui long chon lai!" << std::endl;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

//void SetAlwaysOnTop(HWND hwnd) {
//    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0,
//        SWP_NOMOVE | SWP_NOSIZE);
//}

// ====== Main ======
int main() {
    //SetAlwaysOnTop(); // Gọi hàm này khi mở app
    std::cout << "F1: On: Enable/Off: Disable Macro\tF4: Tang do giat doc\tF3: Giam do giat doc\tF5: Chon sung\n";
    std::thread t1(macroThread);
    std::thread t2(hotkeyThread);

    t1.join();
    t2.join();
    return 0;
}
