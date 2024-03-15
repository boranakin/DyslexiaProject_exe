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
            auto now = std::chrono::system_clock::now();
            auto nowAsTimeT = std::chrono::system_clock::to_time_t(now);
            auto nowMs = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

            // Convert to tm struct for formatting
            std::tm nowTm;
            localtime_s(&nowTm, &nowAsTimeT);

            // Write the time up to seconds
            gazeDataFile << std::put_time(&nowTm, "[%Y-%m-%d %H:%M:%S");
            // Write milliseconds and gaze point to the file
            gazeDataFile << '.' << std::setfill('0') << std::setw(3) << nowMs.count() << "] "
                << "Gaze point: [" << gazePoint.X << ", " << gazePoint.Y << "]" << std::endl;
        }

        Sleep(1000 / 100); // Approximately 100 updates per second
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
