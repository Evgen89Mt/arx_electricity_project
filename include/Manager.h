#pragma once
#include "IAcadAPI.h"
#include "BlockFactory.h"
#include "ObjectARXWrapper.h"
#include "ConsoleUI.h"

#include <gepnt3d.h>     // AcGePoint3d — нужен по значению
#include <dbmain.h>      // AcDbObjectId — нужен для kNull

// #include <gepnt3d.h>     // полное определение AcGePoint3d

class Manager {
public:
    explicit Manager(IAcadAPI& api) : m_api(api) {}

    void Execute() {
        m_api.logMessage(L"[Manager] Запрос параметров круга", false);

        AcGePoint3d center;
        double radius;

        // Запрос центра
        if (!ConsoleUI::GetPoint(L"\nУкажите центр круга: ", center)) {
            m_api.logMessage(L"Операция отменена пользователем", true);
            return;
        }
        // Запрос радиуса
        if (!ConsoleUI::GetReal(L"\nВведите радиус: ", radius)) {
            m_api.logMessage(L"Операция отменена пользователем", true);
            return;
        }
        if (radius <= 0) {
            m_api.logMessage(L"Радиус должен быть положительным", true);
            return;
        }

        // Транзакция
        ObjectARXWrapper::TransactionGuard guard(static_cast<ObjectARXWrapper&>(m_api));

        // Создаём круг через фабрику
        AcDbObjectId id = BlockFactory::CreateCircle(center, radius, m_api);
        if (id == AcDbObjectId::kNull) {
            m_api.logMessage(L"Не удалось создать круг", true);
            return;
        }

        m_api.logMessage(L"Круг успешно создан", false);
        guard.commit();
        m_api.logMessage(L"[Manager] Завершено", false);
    }

private:
    IAcadAPI& m_api;
};
