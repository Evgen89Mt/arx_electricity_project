#include <aced.h>
#include <rxregsvc.h>
#include <dbmain.h>

// Class Project
#include "ObjectARXWrapper.h"


// --- Команды ---

void helloWorld() {
    acutPrintf(L"\nHello, World from ObjectARX!\n");
}

// void drawCircleCmd() {
//     ArCircle circle(AcGePoint3d(0.0, 0.0, 0.0), 50.0);
//     if (circle.addToModelSpace() != AcDbObjectId::kNull)
//         acutPrintf(L"\nCircle created at (0,0) with radius 50.\n");
//     else
//         acutPrintf(L"\nFailed to create circle.\n");
// }

// void drawTextCmd() {
//     ArText text(AcGePoint3d(0.0, 0.0, 0.0), L"Hello, AutoCAD!", 5.0);
//     if (text.addToModelSpace() != AcDbObjectId::kNull)
//         acutPrintf(L"\nText created at (0,0).\n");
//     else
//         acutPrintf(L"\nFailed to create text.\n");
// }

// void drawSceneCmd() {
//     const AcGePoint3d circleCenter(0.0, 0.0, 0.0);
//     const double      radius = 25.0;

//     ArCircle circle(circleCenter, radius);
//     if (circle.addToModelSpace() == AcDbObjectId::kNull) {
//         acutPrintf(L"\nFailed to create circle.\n");
//         return;
//     }

//     ArText label(
//         AcGePoint3d(circleCenter.x - radius * 0.4,
//                     circleCenter.y + radius + 5.0,
//                     0.0),
//         L"Hello, AutoCAD!",
//         5.0
//     );
//     if (label.addToModelSpace() == AcDbObjectId::kNull) {
//         acutPrintf(L"\nFailed to create text.\n");
//         return;
//     }

//     acutPrintf(L"\nScene created: circle r=%.1f + text label.\n", radius);
// }

// --- Init / Unload ---

void initApp() {
    acedRegCmds->addCommand(L"MY_ARX_GROUP", L"HELLO_WORLD", L"HELLO_WORLD", ACRX_CMD_MODAL, helloWorld);
    // acedRegCmds->addCommand(L"MY_ARX_GROUP", L"DRAW_CIRCLE", L"DRAW_CIRCLE", ACRX_CMD_MODAL, drawCircleCmd);
    // acedRegCmds->addCommand(L"MY_ARX_GROUP", L"DRAW_TEXT",   L"DRAW_TEXT",   ACRX_CMD_MODAL, drawTextCmd);
    // acedRegCmds->addCommand(L"MY_ARX_GROUP", L"DRAW_SCENE",  L"DRAW_SCENE",  ACRX_CMD_MODAL, drawSceneCmd);

    acutPrintf(L"\nMyArxProject loaded. Commands: HELLO_WORLD, DRAW_CIRCLE, DRAW_TEXT, DRAW_SCENE\n");
}

void unloadApp() {
    acedRegCmds->removeGroup(L"MY_ARX_GROUP");
    acutPrintf(L"\nMyArxProject unloaded.\n");
}

extern "C" AcRx::AppRetCode acrxEntryPoint(AcRx::AppMsgCode msg, void* pkt) {
    switch (msg) {
    case AcRx::kInitAppMsg:
        acrxDynamicLinker->unlockApplication(pkt);
        acrxRegisterAppMDIAware(pkt);
        initApp();
        break;

    case AcRx::kUnloadAppMsg:
        unloadApp();
        break;

    default:
        break;
    }
    return AcRx::kRetOK;
}