#pragma once

/*
    IAcadAPI.h
    Абстрактный интерфейс, инкапсулирующий взаимодействие с AutoCAD.
    Позволяет тестировать бизнес-логику и подменять реализацию.
*/

#include <dbmain.h>     // AcDbObjectId определён здесь

class AcDbEntity;       // forward — используется только по указателю

class IAcadAPI {
public:
    virtual ~IAcadAPI() = default;

    // Добавить сущность в текущее пространство.
    // Возвращает ObjectId или kNull при ошибке.
    virtual AcDbObjectId addEntity(AcDbEntity* pEnt) = 0;

    // Установить текущий слой, при необходимости создать его.
    // colorIndex — индекс цвета слоя (256 = ByLayer).
    virtual void setLayer(const wchar_t* layerName, int indexColor = 256) = 0;

    // Транзакции
    virtual void startTransaction() = 0;
    virtual void commitTransaction() = 0;
    virtual void abortTransaction() = 0;

    // Проверить наличие типа линии в БД.
    virtual void ensureLinetype(const wchar_t* linetypeName) = 0;

    // Логирование (в командную строку AutoCAD).
    virtual void logMessage(const wchar_t* msg, bool isError = false) = 0;
};