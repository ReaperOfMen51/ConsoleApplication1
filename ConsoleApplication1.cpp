#include <iostream>
#include <fstream>
#include <string>
#include <vector>   
#include <windows.h>
#include <winuser.h>
#include <chrono>
#include <thread>
#include <cstdlib>
#include "nvapi.h"

const std::string FILEPATH = "C:/Program Files (x86)/MSI Afterburner/Profiles/VEN_1002&DEV_1636&SUBSYS_8760103C&REV_D3&BUS_3&DEV_0&FN_0.CFG";
const std::string AFNAME = "MSI Afterburner ";
const std::string OCCTNAME = "OCCT - Stability Testing since 2003";
const std::string SUPERLAUNCHNAME = "UNIGINE Superposition v1.1";
const std::string SUPERBENCHNAME = "UNIGINE Superposition Benchmark 1.1 (DirectX) - Benchmarking...";
const char AFTERBURNERPATH[] = "C:/\"Program Files (x86)\"/\"MSI Afterburner\"/MSIAfterburner.exe";
const char OCCTPATH[] = "C:/Users/brick/Downloads/OCCT.exe";

static void instruct_user()
{
    std::cout << "Launch MSI Afterburner and OCCT. Make sure that OCCT is fully launched and in fullscreen mode \nPress any key and the enter key once both programs have fully launched." << std::endl;
    std::string confirm;
    std::cin >> confirm;
}

static std::vector<std::string> read_to_vector(std::string filepath)
{
    std::vector<std::string> lines;
    std::ifstream file(filepath);

    if (!file.is_open())
    {
        std::cerr << "Error opening file: " << filepath << std::endl;
    }

    std::string line;
    while (std::getline(file, line))
    {
        lines.push_back(line);
    }

    file.close();

    return lines;
}

static std::string read_from_vector(std::vector<std::string> file, std::string parameter)
{
    for (int i = 0; i < file.size(); i++)
    {
        bool match = true;
        for (int j = 0; j < parameter.size(); j++)
        {
            if (!(file[i][j] == parameter[j]))
                match = false;
            else if (match && j == parameter.size() - 1)
            {
                line = i;
                done = true;
                break;
            }
        }
        std::cout << "param line: " << std::to_string(line) << std::endl;
        if (done)
            break;
    }
}

static void change_vector_value(std::vector<std::string>& file, std::string parameter, std::string input, int profile)
{
    std::string profileString = "[Profile" + std::to_string(profile) + "]";
    int profileLine = 0;
    for (int i = 0; i < file.size(); i++)
    {
        if (file[i] == profileString)
            profileLine = i;
    }
    if (profileLine == 0)
    {
        file.push_back(profileString);
        file.push_back("Format=2");
        file.push_back("CoreClk=");
        file.push_back("MemClk=");
        profileLine = file.size() - 4;
    }
    int line = 0;
    bool done = false;
    for (int i = profileLine; i < file.size(); i++)
    {
        bool match = true;
        for (int j = 0; j < parameter.size(); j++)
        {
            if (!(file[i][j] == parameter[j]))
                match = false;
            else if (match && j == parameter.size() - 1)
            {
                line = i;
                done = true;
                break;
            }
        }
        std::cout << "param line: " << std::to_string(line) << std::endl;
        if (done)
            break;
    }
    file.erase(file.begin() + line);
    file.insert(file.begin() + line, parameter + input);
    
}

static void vector_to_file(std::vector<std::string> input, std::string filepath)
{
    std::ofstream output(filepath, std::ios::out);

    if (!output.is_open())
    {
        std::cerr << "Error opening file for writing: " << filepath << std::endl;
    }

    for (int i = 0; i < input.size(); i++)
    {
        output << input[i] << std::endl;
    }
    output.close();
}

static char get_gpu_clocksteps(int& clockSteps)
{
    NvPhysicalGpuHandle gpuHandle[NVAPI_MAX_PHYSICAL_GPUS];
    NvU32 gpuNum;
    NvAPI_EnumPhysicalGPUs(gpuHandle, &gpuNum);
    NV_GPU_ARCH_INFO architecture;
    NvAPI_GPU_GetArchInfo(gpuHandle[0], &architecture);
    char name[NVAPI_SHORT_STRING_MAX];
    NvAPI_GPU_GetFullName(gpuHandle[0], name);
}

static void click_screen(int xPos = 0, int yPos = 0)
{
    INPUT vm;
    vm.type = INPUT_MOUSE;
    vm.mi.time = 0;
    vm.mi.dwExtraInfo = 0;
    vm.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
    vm.mi.dx = xPos * 65536 / GetSystemMetrics(SM_CXSCREEN);
    vm.mi.dy = yPos * 65536 / GetSystemMetrics(SM_CYSCREEN);
    SendInput(1, &vm, sizeof(INPUT));
    vm.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    SendInput(1, &vm, sizeof(INPUT));
    vm.mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(1, &vm, sizeof(INPUT));
}

static void stroke_key(char key = NULL, int times = 1)
{
    for (int i = 0; i < times; i++)
    {
        INPUT vk;
        vk.type = INPUT_KEYBOARD;
        vk.ki.wScan = 0;
        vk.ki.time = 0;
        vk.ki.dwExtraInfo = 0;
        vk.ki.wVk = VkKeyScan(key);
        vk.ki.dwFlags = 0;
        SendInput(1, &vk, sizeof(INPUT));
        vk.ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(1, &vk, sizeof(INPUT));
    }
}

static void apply_afterburner_profile(char profile = '1')
{
    std::string name = AFNAME;
    std::wstring wideName = std::wstring(name.begin(), name.end());
    const wchar_t* wideNameCstr = wideName.c_str();
    HWND afterburnerWindow = FindWindow(NULL, wideNameCstr);
    if (afterburnerWindow == NULL)
    {
        std::cerr << "Could not find running instance of MSI Afterburner" << std::endl;
        return;
    }
    if (!SetForegroundWindow(afterburnerWindow))
    {
        std::cerr << "Could not set MSI Afterburner to the foreground" << std::endl;
        return;
    }
    stroke_key(profile);
    POINT cursorPos;
    //GetCursorPos(&cursorPos);
    //ScreenToClient(afterburnerWindow, &cursorPos);
    cursorPos.x = 440;
    cursorPos.y = 510;
    ClientToScreen(afterburnerWindow, &cursorPos);
    click_screen(cursorPos.x, cursorPos.y);
}

static void start_occt_test(int min = 10, int max = 100, int step = 4, int interval = 30)
{
    int minDigs[] = { min / 100, min % 100 / 10, min % 10 };
    int maxDigs[] = { max / 100, max % 100 / 10, max % 10 };
    int stepDigs[] = { step / 100, step % 100 / 10, step % 10 };
    int minutes = interval % 60;
    int minuteDigs[] = { minutes / 100, minutes % 100 / 10, minutes % 10 };
    interval = interval - minutes * 60;
    int intervalDigs[] = { interval % 100 / 10, interval % 10 };
    std::string name = OCCTNAME;
    std::wstring wideName = std::wstring(name.begin(), name.end());
    const wchar_t* wideNameCstr = wideName.c_str();
    HWND occtWindow = FindWindow(NULL, wideNameCstr);
    if (occtWindow == NULL)
    {
        std::cerr << "Could not find running instance of OCCT" << std::endl;
        return;
    }
    if (!SetForegroundWindow(occtWindow))
    {
        std::cerr << "Could not set OCCT to the foreground" << std::endl;
        return;
    }
    click_screen(80, 175);//clicks stability test
    click_screen(250, 400);//clicks 3d adaptive
    click_screen(500, 230);//clicks variable
    click_screen(473, 325);//clicks minimum intensity
    stroke_key(VK_BACK, 3);//clears minimum
    for (int i = 0; i < sizeof(minDigs) - 1; i++)//types individual digits
        stroke_key('0' + minDigs[i]);
    click_screen(583, 325);
    stroke_key(VK_BACK, 3);
    for (int i = 0; i < sizeof(maxDigs) - 1; i++)
        stroke_key('0' + maxDigs[i]);
    click_screen(805, 325);
    stroke_key(VK_BACK, 3);
    for (int i = 0; i < sizeof(stepDigs) - 1; i++)
        stroke_key('0' + stepDigs[i]);
    click_screen(945, 325);
    stroke_key(VK_BACK, 3);
    for (int i = 0; i < sizeof(minuteDigs) - 1; i++)
        stroke_key('0' + stepDigs[i]);
    click_screen(1082, 325);
    stroke_key(VK_BACK, 3);
    for (int i = 0; i < sizeof(intervalDigs) - 1; i++)
        stroke_key('0' + intervalDigs[i]);
    click_screen(250, 1000);
    std::this_thread::sleep_for(std::chrono::seconds(11));
    click_screen(1065, 685);
}

static void end_occt_test()
{
    click_screen(260, 1000);
    click_screen(260, 1000);
}

static bool detect_occt_error()
{
    return false;
}

static bool look_at_pixel(int pixelX, int pixelY, int(&rgbMin)[3], int(&rgbMax)[3])
{
    return false;
}

static void first_pass_core(int& core, int clockSteps, int profile)
{

}

static void second_pass_core(int& core, int clocksteps, int profile)
{

}

static void third_pass_core(int& core, int clocksteps, int profile)
{

}

static void clockstretching_test(int& core, int clocksteps, int profile)
{

}

static int log_benchmark_frames()
{

}

static void superposition_benchmark(int preset = 0)
{
    std::string name = SUPERLAUNCHNAME;
    std::wstring wideName = std::wstring(name.begin(), name.end());
    const wchar_t* wideNameCstr = wideName.c_str();
    HWND superLaunchWindow = FindWindow(NULL, wideNameCstr);
    if (superLaunchWindow == NULL)
    {
        std::cerr << "Could not find running instance of UNIGINE Superposition" << std::endl;
        return;
    }
    if (!SetForegroundWindow(superLaunchWindow))
    {
        std::cerr << "Could not set UNIGINE Superposition to the foreground" << std::endl;
        return;
    }
    POINT cursorPos;

    //click benchmark
    cursorPos.x = 400;
    cursorPos.y = 200;
    ClientToScreen(superLaunchWindow, &cursorPos);
    click_screen(cursorPos.x, cursorPos.y);

    //click performance
    cursorPos.x = 600;
    cursorPos.y = 130;
    ClientToScreen(superLaunchWindow, &cursorPos);
    click_screen(cursorPos.x, cursorPos.y);
  
    //click api
    cursorPos.x = 900;
    cursorPos.y = 190;
    ClientToScreen(superLaunchWindow, &cursorPos);
    click_screen(cursorPos.x, cursorPos.y);

    //click opengl
    cursorPos.x = 900;
    cursorPos.y = 230;
    ClientToScreen(superLaunchWindow, &cursorPos);
    click_screen(cursorPos.x, cursorPos.y);

    //click preset
    cursorPos.x = 900;
    cursorPos.y = 130;
    ClientToScreen(superLaunchWindow, &cursorPos);
    click_screen(cursorPos.x, cursorPos.y);

    //click the preset
    cursorPos.x = 900;
    cursorPos.y = 150 + preset * 20;
    ClientToScreen(superLaunchWindow, &cursorPos);
    click_screen(cursorPos.x, cursorPos.y);

    //click run
    cursorPos.x = 900;
    cursorPos.y = 570;
    ClientToScreen(superLaunchWindow, &cursorPos);
    click_screen(cursorPos.x, cursorPos.y);

    //click run anyway
    cursorPos.x = 600;
    cursorPos.y = 450;
    ClientToScreen(superLaunchWindow, &cursorPos);
    click_screen(cursorPos.x, cursorPos.y);
}

static void get_mouse_coords(int& mouseX, int& mouseY)
{
    std::cout << "Press a key and then enter for the current mouse coordinates." << std::endl;
    std::string input;
    std::cin >> input;
    POINT p;
    GetCursorPos(&p);
}

int main()
{
    int clockSteps = 5000;
    int profile = 1;
    int core = 1500000;
    int memory = 1333000;
    //std::vector<std::string> afterburner = read_to_vector(FILEPATH);
    //change_vector_value(afterburner, "CoreClk=", std::to_string(core), profile);
    //vector_to_file(afterburner, FILEPATH);
    //std::this_thread::sleep_for(std::chrono::seconds(2));
    //apply_afterburner_profile(profile + '0');
    //std::this_thread::sleep_for(std::chrono::seconds(2));
    //start_occt_test();
    //end_occt_test();
    //superposition_benchmark(1);
}

