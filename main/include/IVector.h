#ifndef IVECTOR_H
#define IVECTOR_H


#include<stddef.h>
#include "ILogger.h"






class IVector {
public:
    enum class NORM {
        NORM_1,
        NORM_2,
        NORM_INF
    };
    static IVector* createVector(size_t dim, double* pData, ILogger* pLogger);
    virtual ~IVector() = 0;
    virtual IVector* clone() const = 0;
    static IVector* add(IVector const* pOperand1, IVector const* pOperand2, ILogger* pLogger);
    static IVector* sub(IVector const* pOperand1, IVector const* pOperand2, ILogger* pLogger);
    static IVector* mul(IVector const* pOperand1, double scaleParam, ILogger* pLogger);
    static double mul(IVector const* pOperand1, IVector const* pOperand2, ILogger* pLogger);
    static RESULT_CODE equals(IVector const* pOperand1, IVector const* pOperand2, NORM norm, double tolerance, bool* result, ILogger* pLogger);
    virtual double getCoord(size_t index)const = 0;
    virtual RESULT_CODE setCoord(size_t index, double value) = 0;
    virtual double norm(NORM norm) const= 0;
    virtual size_t getDim() const = 0;
protected:
    IVector() = default;
private:
    IVector(IVector const& vector) = delete;
    IVector& operator=(IVector const& vector) = delete;
};


#endif // IVECTOR_H
