#include "ConsoleUI.h"

#include <adscodes.h>   // RTNORM
#include <aced.h>       // acedGetPoint, acedGetReal
#include <geassign.h>   // asPnt3d
#include <dbmain.h>     // ads_point

bool ConsoleUI::GetPoint(const wchar_t* prompt, AcGePoint3d& pt) {
    ads_point adsPt;
    if (acedGetPoint(NULL, prompt, adsPt) == RTNORM) {
        pt = asPnt3d(adsPt);
        return true;
    }
    return false;
}

bool ConsoleUI::GetReal(const wchar_t* prompt, double& val) {
    if (acedGetReal(prompt, &val) == RTNORM)
        return true;
    return false;
}