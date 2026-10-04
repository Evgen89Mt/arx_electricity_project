#pragma once

#include "IAcadAPI.h"
// #include <atlstr.h>
/*
	ObjectARXWrapper.h
	–еализаци¤ интерфейса IAcadAPI с использованием пр¤мых вызовов ObjectARX
	¬ключает внутрений класс TransactionGuard дл¤ автоматического управлени¤ транзакци¤ми
*/


class ObjectARXWrapper : public IAcadAPI {
public:
	ObjectARXWrapper()
		: m_transactionActive(false)
	{}

	virtual ~ObjectARXWrapper();

	// ћетоды интерфейса
	AcDbObjectId addEntity(AcDbEntity* pEnt)						override;
	void setLayer(const wchar_t* layerName, int indexColor = 256)	override;
	void startTransaction()											override;
	void commitTransaction()										override;
	void abortTransaction()											override;
	void ensureLinetype(const wchar_t* linetypeName)				override;
	void logMessage(const wchar_t* msg, bool isError = false)		override;

	// RAII- обЄртка дл¤ транзакций: автоматически начинает транзакцию при создании,
	// и откатывает в деструкторе, если не был вызван commit();
	// »спользование: TransactionGuard guard(api); ... guard.commit();

	class TransactionGuard {
	public:
		explicit TransactionGuard(ObjectARXWrapper& wrapper);
		~TransactionGuard();
		void commit();					// ‘иксирует
		void abort();					// ќткат

	private:
		ObjectARXWrapper& m_wrapper;
		bool m_committed;
	};

private:
	//AcTransaction* m_pTrans;			// текущий объект транзакции
	bool m_transactionActive;			// флаг активной транзакции

	// вспомогательные методы
	Acad::ErrorStatus createLayerIfNeeded(const wchar_t* name, int color);
	void errorView(const wchar_t* nameFunc);	//вывод ошибки с имением функции
};

