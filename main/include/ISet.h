#ifndef ISET_H
#define ISET_H
#include "ILogger.h"
#include "IVector.h"
class ISet {
public:
	static ISet* createSet(ILogger* pLogger);
	virtual~ISet() = 0;
	virtual RESULT_CODE insert(const IVector* pVector,IVector::NORM norm, double tolerance) = 0;
	virtual RESULT_CODE get(IVector*& pVector, size_t index)const = 0;
	virtual RESULT_CODE get(IVector*& pVector, IVector const* pSample, IVector::NORM norm, double tolerance)const = 0;
	virtual size_t getDim() const = 0; //space dimension
	virtual size_t getSize() const = 0; //num elements in set
	virtual void clear() = 0; // delete all
	virtual RESULT_CODE erase(size_t index) = 0;
	virtual RESULT_CODE erase(IVector const* pSample, IVector::NORM norm, double tolerance) = 0;
	virtual ISet* clone()const = 0;
	static ISet* add(ISet const* pOperand1, ISet const* pOperand2, IVector::NORM norm, double tolerance, ILogger* pLogger);
	static ISet* intersect(ISet const* pOperand1, ISet const* pOperand2, IVector::NORM norm, double tolerance, ILogger* pLogger);
        static ISet* sub(ISet const* pOperand1, ISet const* pOperand2, IVector::NORM norm, double tolerance, ILogger* pLogger);
	static ISet* symSub(ISet const* pOperand1, ISet const* pOperand2, IVector::NORM norm, double tolerance, ILogger* pLogger);
protected:
	ISet() = default;
private:
	ISet(ISet const& set) = delete;
	ISet& operator=(ISet const& vector) = delete;

};

#endif //ISET_H
