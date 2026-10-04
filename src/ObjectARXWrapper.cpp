#include "ObjectARXWrapper.h"

#include <string>
#include <aced.h>          // acutPrintf
#include <dbapserv.h>      // acdbCurDwg, acdbHostApplicationServices
#include <migrtion.h>      // Определяет макрос acdbCurDwg()
#include <dbidar.h>        // acdbOpenObject, AcDbObjectIdArray
#include <dbents.h>        // AcDbEntity (полное определение для ->close())
#include <dbsymtb.h>       // AcDbBlockTableRecord
#include <dbtrans.h>       // AcDbTransactionManager, acdbTransactionManagerPtr
#include <dbsymtb.h>       // AcDbLayerTable, AcDbLinetypeTable, AcDbBlockTable
#include <dbcolor.h>       // AcCmColor


ObjectARXWrapper::~ObjectARXWrapper() {
	if (m_transactionActive) {
		// откатываем не завёршенную тарнзакцию
		AcDbTransactionManager* pTM = acdbTransactionManagerPtr();
		if (pTM) {
			pTM->abortTransaction(); // откат
			m_transactionActive = false;
			logMessage(L"Транзакцию откатили в деструкторе", false);
		}
	}
}

AcDbObjectId ObjectARXWrapper::addEntity(AcDbEntity* pEnt) {
	if (!pEnt) {
		logMessage(L"Ошибка, передали пустой указатель", true);
		return AcDbObjectId::kNull;
	}

	// Получаем текущие пространство
	// AcDbDatabase* pDb = acdbCurDwg();
    AcDbDatabase* pDb = acdbHostApplicationServices()->workingDatabase();
	if (!pDb) {
		logMessage(L"Ошибка: не удалось получить текущую базу данных", true);
		delete pEnt;
		return AcDbObjectId::kNull;
	}

	// Получаем id пространства
	AcDbObjectId spaceId = pDb->currentSpaceId();
	if (spaceId == AcDbObjectId::kNull) {
		logMessage(L"Ошибка: не удалось получить Id текущего пространства", true);
		delete pEnt;
		return AcDbObjectId::kNull;
	}

	// Открываем блок для записи
	AcDbBlockTableRecord* pRecSpace = nullptr;
	Acad::ErrorStatus es = acdbOpenObject(pRecSpace, spaceId, AcDb::kForWrite);
	if (es != Acad::eOk || !pRecSpace) {
		logMessage(L"Ошибка: не удалось открыть для записи текущее пространство", true);
		delete pEnt;
		return AcDbObjectId::kNull;
	}

	// add Entity to DB
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
		logMessage(L"Ошибка: пустое имя.", true);
		return;
	}

	Acad::ErrorStatus es = createLayerIfNeeded(layerName, indexColor);
	if (es != Acad::eOk) {
		logMessage(L"Ошибка: Не удалось создать слой.", true);
		return;
	}

	// Получаем id таблицы слоёв
	AcDbLayerTable* pLayerTable = nullptr;
    // AcDbDatabase* pDb = acdbHostApplicationServices()->workingDatabase();
	es = acdbCurDwg()->getLayerTable(pLayerTable, AcDb::kForRead);
	if (es != Acad::eOk || !pLayerTable) {
		logMessage(L"Ошибка: Не удалось открыть таблицу слоёв для чтения", true);
		return;
	}

	// Получаем id слоя
	AcDbObjectId layerId = AcDbObjectId::kNull;
	es = pLayerTable->getAt(layerName, layerId);

	if (es != Acad::eOk || layerId == AcDbObjectId::kNull) {
		logMessage(L"Не удалось найти ID слоя.", true);
		return;
	}

	// Устанавливаем слой текущим.
	es = acdbCurDwg()->setClayer(layerId);
	if (es != Acad::eOk) {
		logMessage(L"Не удалось установить слой текущим.", true);
	}
	else {
		logMessage(L"Слой установлен текущем.", false);
	}
}

void ObjectARXWrapper::startTransaction() {
	logMessage(L"[ObjectARXWrapper::commitTransaction] Начало", false);
	if (m_transactionActive) {
		logMessage(L"Предупреждение: тарнзакция уже активна.", false);
		return;
	}

	AcDbTransactionManager* pTM = acdbTransactionManagerPtr();
	if (pTM) {
		pTM->startTransaction();
		m_transactionActive = true;
		logMessage(L"Транзакция начата.", false);
	}
	else {
		logMessage(L"Ошбка: не удалось получить менеджер тарнзакций.", true);
	}
}

void ObjectARXWrapper::commitTransaction() {
	logMessage(L"[ObjectARXWrapper::commitTransaction] Начало", false);
	if (!m_transactionActive) {
		logMessage(L"[ObjectARXWrapper::commitTransaction] Ошибки: нет активной транзакции для коммита.", true);
		return;
	}
	AcDbTransactionManager* pTM = acdbTransactionManagerPtr();
	if (pTM) {
		pTM->endTransaction();
		m_transactionActive = false;
		logMessage(L"[ObjectARXWrapper::commitTransaction] Транзакция зафиксирована.", false);
	}
	else {
		logMessage(L"[ObjectARXWrapper::commitTransaction] Ошибка: не удалось получить менеджер транзакции.", true);
	}

}

void ObjectARXWrapper::abortTransaction() {
	logMessage(L"[ObjectARXWrapper::abortTransaction] Начало", false);
	if (!m_transactionActive) {
		logMessage(L"[ObjectARXWrapper::abortTransaction] Ошибка: нет активной транзакции.", true);
		return;
	}

	AcDbTransactionManager* pTM = acdbTransactionManagerPtr();
	if (pTM) {
		pTM->abortTransaction();
		m_transactionActive = false;
		logMessage(L"[ObjectARXWrapper::abortTransaction] Транзакция отключена.", false);
	}
	else {
		logMessage(L"[ObjectARXWrapper::abortTransaction] Ошибка: не удалось получить менеджер транзакций.", true);
	}
}

void ObjectARXWrapper::ensureLinetype(const wchar_t* linetypeName) {
	// logMessage(L"[ObjectARXWrapper::ensureLinetype] Начало тип: " + std::wstring(linetypeName), false);
	if (!linetypeName || wcslen(linetypeName) == 0) {
		logMessage(L"[ObjectARXWrapper::ensureLinetype] Ошибка: пустое имя типа линии", true);
		return;
	}

	AcDbLinetypeTable* pLinetypeTable = nullptr;
	Acad::ErrorStatus es = acdbCurDwg()->getLinetypeTable(pLinetypeTable, AcDb::kForRead);
	if (es != Acad::eOk || !pLinetypeTable) {
		logMessage(L"[ObjectARXWrapper::ensureLinetype] Ошибка: не удалось открыть таблицу типов линий", true);
		return;
	}

	// проверяем наличие типа линий
	bool exists = pLinetypeTable->has(linetypeName);
	pLinetypeTable->close();

	if (exists) {
		logMessage(L"[ObjectARXWrapper::ensureLinetype] Тип линий есть базе данных чертежа", false);
	}
	else {
		logMessage(L"[ObjectARXWrapper::ensureLinetype] Тип линий не найден, используйте Continuous", false);
	}
}

void ObjectARXWrapper::logMessage(const wchar_t* msg, bool isError) {
	if (!msg) {
		acutPrintf(L"[ObjectARXWrapper::logMessage] Ошибка: указатель msg is empty");
		return;
	}

	if (isError) {
		acutPrintf(L"\n[Ошибка] %s", msg);
	}
	else {
		acutPrintf(L"\n%s", msg);
	}
}

ObjectARXWrapper::TransactionGuard::TransactionGuard(ObjectARXWrapper& wrapper)
	:m_wrapper(wrapper)
	, m_committed(false)
{
	m_wrapper.logMessage(L"[TransactionGuard] Конструктор, страрт транзакции", false);
	m_wrapper.startTransaction();
}

ObjectARXWrapper::TransactionGuard::~TransactionGuard() {
	m_wrapper.logMessage(L"[TransactionGuard] Деструктор ", false);
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

Acad::ErrorStatus ObjectARXWrapper::createLayerIfNeeded(const wchar_t* name, int color) {
	AcDbLayerTable* pLayerTable = nullptr;
	Acad::ErrorStatus es = acdbCurDwg()->getLayerTable(pLayerTable, AcDb::kForRead);
	if (es != Acad::eOk || !pLayerTable) return es;
	if (pLayerTable->has(name)) {
		pLayerTable->close();
		return Acad::eOk;
	}
	pLayerTable->close();
	es = acdbCurDwg()->getLayerTable(pLayerTable, AcDb::kForWrite);
	if (es != Acad::eOk || !pLayerTable) return es;
	AcDbLayerTableRecord* pLayer = new AcDbLayerTableRecord();
	pLayer->setName(name);
	//pLayer->setColorIndex(color);						// not found methods API autocad
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