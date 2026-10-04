// ConsoleUI.cpp
#include "ConsoleUI.h"

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