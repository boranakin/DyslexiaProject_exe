#include <iostream>
#include <fstream>
#include <string>
#include "windows.h"
#include <thread>
#include "tobii_gameintegration.h"
#include "tobii_gameintegration_dynamic_loader.h"

using namespace TobiiGameIntegration;

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
            gazeDataFile << "Gaze point: [" << gazePoint.X << ", " << gazePoint.Y << "]" << std::endl;
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
