#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <string>
#include "windows.h"
#include <thread>
#include "tobii_gameintegration.h"
#include "tobii_gameintegration_dynamic_loader.h"

using namespace TobiiGameIntegration;
using namespace std::chrono;

void GazeSample(HWND windowHandle) {
    ITobiiGameIntegrationApi* api = GetApi("Tobii_api_test1");
    if (api == nullptr) {
        std::cerr << "Failed to initialize the Tobii API." << std::endl;
        return;
    }

    IStreamsProvider* streamsProvider = api->GetStreamsProvider();
    if (streamsProvider == nullptr) {
        std::cerr << "Failed to get Streams Provider." << std::endl;
        api->Shutdown();
        return;
    }

    std::ofstream gazeDataFile("gazeData.txt", std::ios::out);
    if (!gazeDataFile.is_open()) {
        std::cerr << "Failed to open gazeData.txt for writing." << std::endl;
        api->Shutdown();
        return;
    }

    if (!api->GetTrackerController()->TrackWindow(windowHandle)) {
        std::cerr << "Failed to track window." << std::endl;
        api->Shutdown();
        return;
    }

    while (!GetAsyncKeyState(VK_ESCAPE)) {
        api->Update();

        GazePoint gazePoint;
        if (streamsProvider->GetLatestGazePoint(gazePoint)) {
            // Get the current time as a time_point
            system_clock::time_point now = system_clock::now();
            // Convert to a time_t for compatibility with std::put_time
            std::time_t now_c = system_clock::to_time_t(now);
            // Convert to tm struct for formatting
            std::tm now_tm;
            localtime_s(&now_tm, &now_c);

            // Write the time and gaze point to the file
            gazeDataFile << std::put_time(&now_tm, "[%Y-%m-%d %H:%M:%S] ")
                << "Gaze point: [" << gazePoint.X << ", " << gazePoint.Y << "]" << std::endl;
        }

        Sleep(1000 / 60); // Approximately 60 updates per second
    }

    api->Shutdown();
    gazeDataFile.close();
}

int main(int argc, char* argv[]) {
    std::cout << "Starting Gaze Recording...\n";

    if (argc > 1) {
        HWND windowHandle = reinterpret_cast<HWND>(std::stoull(argv[1], nullptr, 0));
        GazeSample(windowHandle);
    }
    else {
        std::cerr << "Window handle not provided." << std::endl;
        return -1; // Return an error code
    }

    return 0;
}
