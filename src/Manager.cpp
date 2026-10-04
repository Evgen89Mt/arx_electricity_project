#include "Manager.h"
#include "BlockFactory.h"
#include "ObjectARXWrapper.h"
#include "ConsoleUI.h"

#include <gepnt3d.h>
#include <dbmain.h>

Manager::Manager(IAcadAPI& api) : m_api(api) {}

void Manager::Execute() {
    m_api.logMessage(L"[Manager] Запрос параметров круга", false);

    AcGePoint3d center;
    double radius = 0.0;

    if (!ConsoleUI::GetPoint(L"\nУкажите центр круга: ", center)) {
        m_api.logMessage(L"Операция отменена пользователем", true);
        return;
    }
    if (!ConsoleUI::GetReal(L"\nВведите радиус: ", radius)) {
        m_api.logMessage(L"Операция отменена пользователем", true);
        return;
    }
    if (radius <= 0) {
        m_api.logMessage(L"Радиус должен быть положительным", true);
        return;
    }

    ObjectARXWrapper::TransactionGuard guard(
        static_cast<ObjectARXWrapper&>(m_api));

    AcDbObjectId id = BlockFactory::CreateCircle(center, radius, m_api);
    if (id == AcDbObjectId::kNull) {
        m_api.logMessage(L"Не удалось создать круг", true);
        return;
    }

    m_api.logMessage(L"Круг успешно создан", false);
    guard.commit();
    m_api.logMessage(L"[Manager] Завершено", false);
}