#include "ObjectARXWrapper.h"

#include <string>
#include <aced.h>          // acutPrintf
#include <dbapserv.h>      // acdbHostApplicationServices
#include <dbidar.h>        // acdbOpenObject
#include <dbents.h>        // AcDbEntity (полное определение для ->close())
#include <dbsymtb.h>       // AcDbBlockTableRecord, AcDbLayerTable, AcDbLinetypeTable
#include <dbtrans.h>       // AcDbTransactionManager, acdbTransactionManagerPtr
#include <dbcolor.h>       // AcCmColor

ObjectARXWrapper::~ObjectARXWrapper() {
    if (m_transactionActive) {
        AcDbTransactionManager* pTM = acdbTransactionManagerPtr();
        if (pTM) {
            pTM->abortTransaction();
            m_transactionActive = false;
            logMessage(L"Транзакцию откатили в деструкторе", false);
        }
    }
}

AcDbObjectId ObjectARXWrapper::addEntity(AcDbEntity* pEnt) {
    if (!pEnt) {
        logMessage(L"Ошибка: передали пустой указатель", true);
        return AcDbObjectId::kNull;
    }

    AcDbDatabase* pDb = acdbHostApplicationServices()->workingDatabase();
    if (!pDb) {
        logMessage(L"Ошибка: не удалось получить текущую базу данных", true);
        delete pEnt;
        return AcDbObjectId::kNull;
    }

    AcDbObjectId spaceId = pDb->currentSpaceId();
    if (spaceId == AcDbObjectId::kNull) {
        logMessage(L"Ошибка: не удалось получить Id текущего пространства", true);
        delete pEnt;
        return AcDbObjectId::kNull;
    }

    AcDbBlockTableRecord* pRecSpace = nullptr;
    Acad::ErrorStatus es = acdbOpenObject(pRecSpace, spaceId, AcDb::kForWrite);
    if (es != Acad::eOk || !pRecSpace) {
        logMessage(L"Ошибка: не удалось открыть для записи текущее пространство", true);
        delete pEnt;
        return AcDbObjectId::kNull;
    }

    AcDbObjectId objId;
    es = pRecSpace->appendAcDbEntity(objId, pEnt);
    pRecSpace->close();

    if (es != Acad::eOk) {
        logMessage(L"Ошибка: добавления сущности", true);
        delete pEnt;
        return AcDbObjectId::kNull;
    }

    pEnt->close();
    return objId;
}

void ObjectARXWrapper::setLayer(const wchar_t* layerName, int indexColor) {
    if (!layerName || wcslen(layerName) == 0) {
        logMessage(L"Ошибка: пустое имя слоя", true);
        return;
    }

    Acad::ErrorStatus es = createLayerIfNeeded(layerName, indexColor);
    if (es != Acad::eOk) {
        logMessage(L"Ошибка: не удалось создать слой", true);
        return;
    }

    AcDbLayerTable* pLayerTable = nullptr;
    es = acdbHostApplicationServices()->workingDatabase()
            ->getLayerTable(pLayerTable, AcDb::kForRead);
    if (es != Acad::eOk || !pLayerTable) {
        logMessage(L"Ошибка: не удалось открыть таблицу слоёв для чтения", true);
        return;
    }

    AcDbObjectId layerId = AcDbObjectId::kNull;
    es = pLayerTable->getAt(layerName, layerId);
    pLayerTable->close();

    if (es != Acad::eOk || layerId == AcDbObjectId::kNull) {
        logMessage(L"Не удалось найти ID слоя", true);
        return;
    }

    es = acdbHostApplicationServices()->workingDatabase()->setClayer(layerId);
    if (es != Acad::eOk) {
        logMessage(L"Не удалось установить слой текущим", true);
    } else {
        logMessage(L"Слой установлен текущим", false);
    }
}

void ObjectARXWrapper::startTransaction() {
    logMessage(L"[ObjectARXWrapper::startTransaction] Начало", false);
    if (m_transactionActive) {
        logMessage(L"Предупреждение: транзакция уже активна", false);
        return;
    }

    AcDbTransactionManager* pTM = acdbTransactionManagerPtr();
    if (pTM) {
        pTM->startTransaction();
        m_transactionActive = true;
        logMessage(L"Транзакция начата", false);
    } else {
        logMessage(L"Ошибка: не удалось получить менеджер транзакций", true);
    }
}

void ObjectARXWrapper::commitTransaction() {
    logMessage(L"[ObjectARXWrapper::commitTransaction] Начало", false);
    if (!m_transactionActive) {
        logMessage(L"Ошибка: нет активной транзакции для коммита", true);
        return;
    }

    AcDbTransactionManager* pTM = acdbTransactionManagerPtr();
    if (pTM) {
        pTM->endTransaction();
        m_transactionActive = false;
        logMessage(L"Транзакция зафиксирована", false);
    } else {
        logMessage(L"Ошибка: не удалось получить менеджер транзакций", true);
    }
}

void ObjectARXWrapper::abortTransaction() {
    logMessage(L"[ObjectARXWrapper::abortTransaction] Начало", false);
    if (!m_transactionActive) {
        logMessage(L"Ошибка: нет активной транзакции", true);
        return;
    }

    AcDbTransactionManager* pTM = acdbTransactionManagerPtr();
    if (pTM) {
        pTM->abortTransaction();
        m_transactionActive = false;
        logMessage(L"Транзакция откачена", false);
    } else {
        logMessage(L"Ошибка: не удалось получить менеджер транзакций", true);
    }
}

void ObjectARXWrapper::ensureLinetype(const wchar_t* linetypeName) {
    if (!linetypeName || wcslen(linetypeName) == 0) {
        logMessage(L"[ensureLinetype] Ошибка: пустое имя типа линии", true);
        return;
    }

    AcDbLinetypeTable* pLinetypeTable = nullptr;
    Acad::ErrorStatus es = acdbHostApplicationServices()->workingDatabase()
        ->getLinetypeTable(pLinetypeTable, AcDb::kForRead);
    if (es != Acad::eOk || !pLinetypeTable) {
        logMessage(L"[ensureLinetype] Ошибка: не удалось открыть таблицу типов линий", true);
        return;
    }

    bool exists = pLinetypeTable->has(linetypeName);
    pLinetypeTable->close();

    if (exists) {
        logMessage(L"[ensureLinetype] Тип линий есть в БД чертежа", false);
    } else {
        logMessage(L"[ensureLinetype] Тип линий не найден, используйте Continuous", false);
    }
}

void ObjectARXWrapper::logMessage(const wchar_t* msg, bool isError) {
    if (!msg) {
        acutPrintf(L"\n[logMessage] Ошибка: указатель msg пуст");
        return;
    }

    if (isError) {
        acutPrintf(L"\n[Ошибка] %s", msg);
    } else {
        acutPrintf(L"\n%s", msg);
    }
}

// --- TransactionGuard ---

ObjectARXWrapper::TransactionGuard::TransactionGuard(ObjectARXWrapper& wrapper)
    : m_wrapper(wrapper)
    , m_committed(false)
{
    m_wrapper.logMessage(L"[TransactionGuard] Конструктор, старт транзакции", false);
    m_wrapper.startTransaction();
}

ObjectARXWrapper::TransactionGuard::~TransactionGuard() {
    m_wrapper.logMessage(L"[TransactionGuard] Деструктор", false);
    if (!m_committed && m_wrapper.m_transactionActive) {
        m_wrapper.abortTransaction();
    }
}

void ObjectARXWrapper::TransactionGuard::commit() {
    m_wrapper.logMessage(L"[TransactionGuard::commit] Вызов", false);
    if (!m_committed && m_wrapper.m_transactionActive) {
        m_wrapper.commitTransaction();
        m_committed = true;
    }
}

void ObjectARXWrapper::TransactionGuard::abort() {
    m_wrapper.logMessage(L"[TransactionGuard::abort] Вызов", false);
    if (!m_committed && m_wrapper.m_transactionActive) {
        m_wrapper.abortTransaction();
        m_committed = true;
    }
}

// --- private helpers ---

Acad::ErrorStatus ObjectARXWrapper::createLayerIfNeeded(const wchar_t* name, int color) {
    AcDbLayerTable* pLayerTable = nullptr;
    Acad::ErrorStatus es = acdbHostApplicationServices()->workingDatabase()
        ->getLayerTable(pLayerTable, AcDb::kForRead);
    if (es != Acad::eOk || !pLayerTable) return es;

    if (pLayerTable->has(name)) {
        pLayerTable->close();
        return Acad::eOk;
    }
    pLayerTable->close();

    es = acdbHostApplicationServices()->workingDatabase()
        ->getLayerTable(pLayerTable, AcDb::kForWrite);
    if (es != Acad::eOk || !pLayerTable) return es;

    AcDbLayerTableRecord* pLayer = new AcDbLayerTableRecord();
    pLayer->setName(name);

    AcCmColor am_color;
    am_color.setColorIndex(color);
    pLayer->setColor(am_color);

    es = pLayerTable->add(pLayer);
    pLayer->close();
    pLayerTable->close();
    return es;
}

void ObjectARXWrapper::errorView(const wchar_t* nameFunc) {
    if (!nameFunc) return;
    std::wstring msg = L"[ObjectARXWrapper::errorView] ";
    msg += nameFunc;
    logMessage(msg.c_str(), true);
}