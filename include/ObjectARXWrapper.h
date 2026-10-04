#pragma once

#include "IAcadAPI.h"

/*
    ObjectARXWrapper.h
    Реализация IAcadAPI через прямые вызовы ObjectARX.
    Включает вложенный RAII-класс TransactionGuard.
*/

class ObjectARXWrapper : public IAcadAPI {
public:
    ObjectARXWrapper()
        : m_transactionActive(false)
    {}

    virtual ~ObjectARXWrapper();

    // Методы интерфейса
    AcDbObjectId addEntity(AcDbEntity* pEnt)                        override;
    void setLayer(const wchar_t* layerName, int indexColor = 256)   override;
    void startTransaction()                                         override;
    void commitTransaction()                                        override;
    void abortTransaction()                                         override;
    void ensureLinetype(const wchar_t* linetypeName)                override;
    void logMessage(const wchar_t* msg, bool isError = false)       override;

    // RAII-обёртка для транзакции: старт в конструкторе, откат в деструкторе,
    // если не был вызван commit().
    class TransactionGuard {
    public:
        explicit TransactionGuard(ObjectARXWrapper& wrapper);
        ~TransactionGuard();
        void commit();
        void abort();

    private:
        ObjectARXWrapper& m_wrapper;
        bool m_committed;
    };

private:
    bool m_transactionActive;

    // Вспомогательные методы
    Acad::ErrorStatus createLayerIfNeeded(const wchar_t* name, int color);
    void errorView(const wchar_t* nameFunc);
};