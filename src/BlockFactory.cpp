#include "BlockFactory.h"
#include "IAcadAPI.h"

#include <dbents.h>      // AcDbCircle
#include <gevec3d.h>     // AcGeVector3d::kZAxis

AcDbObjectId BlockFactory::CreateCircle(const AcGePoint3d& center,
                                        double radius,
                                        IAcadAPI& api) {
    if (radius <= 0.0) {
        api.logMessage(L"CreateCircle: радиус должен быть положительным", true);
        return AcDbObjectId::kNull;
    }

    AcDbCircle* pCircle = new AcDbCircle(center, AcGeVector3d::kZAxis, radius);
    return api.addEntity(pCircle);
}