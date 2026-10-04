// ConsoleUI.h
#pragma once
#include <gepnt3d.h>     // полное определение AcGePoint3d

class ConsoleUI {
public:
    // Запрашивает точку в пространстве модели
    static bool GetPoint(const wchar_t* prompt, AcGePoint3d& pt);
    // Запрашивает вещественное число
    static bool GetReal(const wchar_t* prompt, double& val);
};