#pragma once

/*
	IAcadAPI.h
	Абстрактный интерфейс, инкапсулирующий все взаимодействия с AutoCAD
	Позволяет тестировать бизнес-логику и тестировать реализацию.
*/

class AcDbEntity;
class AcDbObjectId;

class IAcadAPI {
public:
	virtual ~IAcadAPI() = default;

	// Добавить сущность в пространство модели (или текущие пространство)
	// Возращает ObjectId ссылки из базы пространства или 0 - error
	virtual AcDbObjectId addEntity(AcDbEntity* pEnt) = 0;

	// Установить текущим слой, при необходимости создать его
	// colorIndex - цвет слоя (по умолчанию ByLayer)
	virtual void setLayer(const wchar_t* layerName, int indexColor = 256) = 0;

	// Группа операций для атомарного выполнения/отката
	virtual void startTransaction() = 0;
	virtual void commitTransaction() = 0;
	virtual void abortTransaction() = 0;

	// Убедимся в загрузки типа линии в БД пространства
	virtual void ensureLinetype(const wchar_t* linetypeName) = 0;

	// Логирование сообщений (в командную строку Autocad или можно в файл)
	// isError - маркер ошибки (для выделения)
	virtual void logMessage(const wchar_t* msg, bool isError = false) = 0;
};

