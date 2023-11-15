// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ConsoleApplication1.h"
#include <memory>
#include "NexaFaceWrapper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;

using namespace std;

int main()
{
    int nRetCode = 0;
    cout << "starting nexaface tester ..." << endl;

    HMODULE hModule = ::GetModuleHandle(nullptr);

    if (hModule != nullptr)
    {
        // initialize MFC and print and error on failure
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            // TODO: change error code to suit your needs
            wprintf(L"Fatal Error: MFC initialization failed\n");
            nRetCode = 1;
        }
        else
        {
            // TODO: code your application's behavior here.
        }
    }
    else
    {
        // TODO: change error code to suit your needs
        wprintf(L"Fatal Error: GetModuleHandle failed\n");
        nRetCode = 1;
    }

    string templateXiao = "ChJORVhBX0ZBQ0U6MDAwMDAwMDIS8wMKBEY1MDAS6AMK5QMK4AMAAAAEAAAAAwAAAAAEAwACAQAAAMABAAABBgEB/v5DdtPmrfWqAz5MviDERgASD/ShfbBXxJW9m6N+0MpBZhti8P+VlnCHYi2EyfrBJM2BOunI+x3BusgjeNyni3IbV+bAAQzbse/LsR/HsR/HcQRHcGzBsh87MOzHAADbdhz7vgFAcCzAHhzHsQf7cGzHEQTBsQ/HMSz7AezHsS/HMezHvh0LAAxBsB9HsBzANgRLsOwHcGzbcRwBcOzBAizLfhwHACzHsixAABzBsez7PmzHvhzLcSzHMRzHEAzHsB/DMezHARzBEOzAsR/HFhzAHgzAEezbvg/AsW3HsW3Lsh3AcWz7ESzHcBzHEQTbdhzbBhzHMAzbfgDHcAzHvgDAsW3LFgD7sBzDdgTHsixbcBzDERzHsR/7sh3BAuzAMuzbdgTAAQDHsm37fhzBsRwAcBzAsW/LEizHEAzHPhzBsAxHcOzDsRzBcADHcRzHcRzBAgQ7EATHvmxHMAxAsAHHfhz7cRzDERzHcSzLsR9DAAD7ASzHvgHbsA3DcQAHcGzHsQMLACxHcOzbBizHcmzbcWzbMgzBAhzBsh/LdgADMABHcGzBNizHsW3HMRzHsAPDtgzBsR3bASz7cQzBFgzBsQAQABgA";

    unique_ptr<NexaFaceWrapper> nexaface = make_unique<NexaFaceWrapper>("version3cache", "D:/nexaface/data/version3cache", 10000, 20);
    nexaface->ShowVersion();
    nexaface->Initialize();
    // nexaface->ProcessDrop();
    // nexaface->ProcessDelete("xiao111405555");
    nexaface->ProcessAddSingleTemplate("xiao2", templateXiao);

    nexaface->ProcessIdentify(templateXiao);
    cout << "all set" << endl;
    return nRetCode;
}
