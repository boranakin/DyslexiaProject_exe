
#include <iostream>
#include "windows.h"
#include <thread>
#include "tobii_gameintegration.h"
#include "tobii_gameintegration_dynamic_loader.h"

using namespace TobiiGameIntegration;

void GazeSample()
{
    ITobiiGameIntegrationApi* api = GetApi("Gaze Sample");
    IStreamsProvider* streamsProvider = api->GetStreamsProvider();

    api->GetTrackerController()->TrackRectangle({0,0,1000,1000});

    while(!GetAsyncKeyState(VK_ESCAPE))
    {
        api->Update();

        GazePoint gazePoint;
        if (streamsProvider->GetLatestGazePoint(gazePoint))
        {
            std::cout << "Gaze point: [" << gazePoint.X << ", " << gazePoint.Y << "]" << std::endl;
        }

        Sleep(1000 / 60);
    }

    api->Shutdown();
}
