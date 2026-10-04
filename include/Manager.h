#pragma once
#include "IAcadAPI.h"

class Manager {
public:
    explicit Manager(IAcadAPI& api);

    void Execute();

private:
    IAcadAPI& m_api;
};