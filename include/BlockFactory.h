#pragma once
#include "IAcadAPI.h"
#include <gepnt3d.h>     // AcGePoint3d

class BlockFactory {
public:
    // Создаёт круг в текущем пространстве. Возвращает ObjectId или kNull.
    static AcDbObjectId CreateCircle(const AcGePoint3d& center,
                                     double radius,
                                     IAcadAPI& api);
};