#include <aced.h>
#include <rxregsvc.h>
#include <dbmain.h>

#include "ObjectARXWrapper.h"
#include "Manager.h"

// --- Команды ---

void helloWorld() {
    acutPrintf(L"\nHello, World from ObjectARX!\n");
}

void drawCircleCmd() {
    ObjectARXWrapper api;
    Manager manager(api);
    manager.Execute();
}

// --- Init / Unload ---

void initApp() {
    acedRegCmds->addCommand(L"MY_ARX_GROUP", L"HELLO_WORLD",
                            L"HELLO_WORLD", ACRX_CMD_MODAL, helloWorld);

    acedRegCmds->addCommand(L"MY_ARX_GROUP", L"DRAW_CIRCLE",
                            L"DRAW_CIRCLE", ACRX_CMD_MODAL, drawCircleCmd);

    acutPrintf(L"\nMyArxProject loaded. Commands: HELLO_WORLD, DRAW_CIRCLE\n");
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