#include <string>
#include <cmath>
#include <mem.h>
#include <limits>

#include "../include/IVector.h"



namespace {
class Loggable {
public:
    explicit Loggable(ILogger * pLogger);
    virtual ~Loggable() = 0;
    static RESULT_CODE printLog(char const * pMsg, RESULT_CODE err, ILogger * pLogger);
    static RESULT_CODE printLogDuring(char const * pMsg, char const * during, RESULT_CODE err, ILogger * pLogger);

    static char const ErrorName[][30];
    ILogger * logger;

private:
    Loggable() = delete;

    static RESULT_CODE printFormatted(FILE * logStream, char const * pMsg, RESULT_CODE err);
};



class Vector : public IVector, private Loggable {
public:
    ~Vector() override;
    IVector * clone() const override;
    double getCoord(size_t index) const override;
    RESULT_CODE setCoord(size_t index, double value) override;
    double norm(NORM norm) const override;
    size_t getDim() const override;

    static Vector * createVector(size_t dim, double * pData, ILogger * pLogger);

private:
    Vector() = delete;
    Vector(Vector const & anotherVector) = delete;
    Vector & operator = (Vector const & anotherVector) = delete;
    Vector(size_t dim, double * pData, ILogger * pLogger);

    size_t dim;
    double * coords;
};
}



/* Loggable */

Loggable::Loggable(ILogger * pLogger) : logger(pLogger) {}

Loggable::~Loggable() = default;

RESULT_CODE Loggable::printFormatted(FILE * logStream, char const * pMsg, RESULT_CODE err) {
    fprintf(logStream, "%s: %s\n", ErrorName[(int) err], pMsg);

    return err;
}

RESULT_CODE Loggable::printLog(char const * pMsg, RESULT_CODE err, ILogger * pLogger) {
    if(pLogger != nullptr) {
        pLogger->log(pMsg, err);
    } else {
        printFormatted(stderr, pMsg, err);
    }

    return err;
}

RESULT_CODE Loggable::printLogDuring(char const * pMsg, char const * during, RESULT_CODE err, ILogger * pLogger) {
    char result[1024] = "";

    strcat(result, pMsg);
    strcat(result, " during \"");
    strcat(result, during);
    strcat(result, "\"");

    return printLog(result, err, pLogger);
}

char const Loggable::ErrorName[][30] = {
    "SUCCESS",
    "OUT_OF_MEMORY",
    "BAD_REFERENCE",
    "WRONG_DIM",
    "DIVISION_BY_ZERO",
    "NAN_VALUE",
    "FILE_ERROR",
    "OUT_OF_BOUNDS",
    "NOT_FOUND",
    "WRONG_ARGUMENT",
    "CALCULATION_ERROR",
    "MULTIPLE_DEFINITION"
};



/* Secondary functions */

bool operandsAreNullptr(void const * pOperand1, void const * pOperand2, char const * during, ILogger * pLogger) {
    if(pOperand1 == nullptr || pOperand2 == nullptr) {
        if(pOperand1 == nullptr && pOperand2 != nullptr) {
            Loggable::printLogDuring("First operand turned out to be equal to nullptr", during,
                                     RESULT_CODE::BAD_REFERENCE, pLogger);
        }

        if(pOperand2 == nullptr && pOperand1 != nullptr) {
            Loggable::printLogDuring("Second operand turned out to be equal to nullptr", during,
                                     RESULT_CODE::BAD_REFERENCE, pLogger);
        }

        if(pOperand1 == nullptr && pOperand2 == nullptr) {
            Loggable::printLogDuring("Both operands turned out to be equal to nullptr", during,
                                     RESULT_CODE::BAD_REFERENCE, pLogger);
        }

        return true;
    }

    return false;
}

bool equalDims(IVector const * pOperand1, IVector const * pOperand2, char const * during, ILogger * pLogger) {
    if(pOperand1->getDim() != pOperand2->getDim()) {
        Loggable::printLogDuring("The dimensions of the vectors are not equal", during, RESULT_CODE::WRONG_DIM, pLogger);

        return false;
    }

    return true;
}



/* IVector */

IVector::~IVector() = default;

IVector * IVector::createVector(size_t dim, double * pData, ILogger * pLogger) {
    return Vector::createVector(dim, pData, pLogger);
}

IVector * IVector::add(IVector const * pOperand1, IVector const * pOperand2, ILogger * pLogger) {
    char const * during = "IVector::add";

    if(operandsAreNullptr(pOperand1, pOperand2, during, pLogger) || !equalDims(pOperand1, pOperand2, during, pLogger)) {
        return nullptr;
    }

    IVector * result = pOperand1->clone();

    if(result == nullptr) {
        Loggable::printLogDuring("Failed to clone first operand", during, RESULT_CODE::OUT_OF_MEMORY, pLogger);

        return nullptr;
    }

    for(size_t i = 0; i < pOperand1->getDim(); ++i) {
        if(result->setCoord(i, pOperand1->getCoord(i) + pOperand2->getCoord(i)) != RESULT_CODE::SUCCESS) {
            Loggable::printLogDuring("Failed to add coordinates", during, RESULT_CODE::CALCULATION_ERROR, pLogger);

            delete result;
            result = nullptr;

            return nullptr;
        }
    }

    return result;
}

IVector * IVector::sub(IVector const * pOperand1, IVector const * pOperand2, ILogger * pLogger) {
    char const * during = "IVector::sub";

    if(operandsAreNullptr(pOperand1, pOperand2, during, pLogger) || !equalDims(pOperand1, pOperand2, during, pLogger)) {
        return nullptr;
    }

    IVector * pOperand2Inversed = IVector::mul(pOperand2, -1, pLogger);

    if(pOperand2Inversed == nullptr) {
        Loggable::printLogDuring("Failed to multiply first operand by -1", during, RESULT_CODE::CALCULATION_ERROR,
                                 pLogger);

        return nullptr;
    }

    IVector * result = IVector::add(pOperand1, pOperand2Inversed, pLogger);

    delete pOperand2Inversed;
    pOperand2Inversed = nullptr;

    if(result == nullptr) {
        Loggable::printLogDuring("Failed to add vectors", during, RESULT_CODE::CALCULATION_ERROR, pLogger);

        return nullptr;
    }

    return result;
}

IVector * IVector::mul(IVector const * pOperand1, double scaleParam, ILogger * pLogger) {
    char const * during = "IVector::mul";

    if(pOperand1 == nullptr) {
        Loggable::printLogDuring("First operand (vector) turned out to be equal to nullptr", during,
                                 RESULT_CODE::BAD_REFERENCE, pLogger);

        return nullptr;
    }

    if(std::isnan(scaleParam)) {
        Loggable::printLogDuring("Second operand (scalar) turned out to be equal to NaN", during,
                                 RESULT_CODE::NAN_VALUE, pLogger);

        return nullptr;
    }

    IVector * result = pOperand1->clone();

    if(result == nullptr) {
        Loggable::printLogDuring("Failed to clone first operand (vector)", during, RESULT_CODE::OUT_OF_MEMORY, pLogger);

        return nullptr;
    }

    for(size_t i = 0; i < pOperand1->getDim(); ++i) {
        if(result->setCoord(i, pOperand1->getCoord(i) * scaleParam) != RESULT_CODE::SUCCESS) {
            Loggable::printLogDuring("Failed to multiply coordinate", during, RESULT_CODE::CALCULATION_ERROR, pLogger);

            delete result;
            result = nullptr;

            return nullptr;
        }
    }

    return result;
}

double IVector::mul(IVector const * pOperand1, IVector const * pOperand2, ILogger * pLogger) {
    char const * during = "IVector::mul";

    if(operandsAreNullptr(pOperand1, pOperand2, during, pLogger) || !equalDims(pOperand1, pOperand2, during, pLogger)) {
        return std::numeric_limits <double>::quiet_NaN();
    }

    double result = 0.;

    for(size_t i = 0; i < pOperand1->getDim(); ++i) {
        result += pOperand1->getCoord(i) * pOperand2->getCoord(i);
    }

    return result;
}

RESULT_CODE IVector::equals(IVector const * pOperand1, IVector const * pOperand2, NORM norm, double tolerance,
                            bool * result, ILogger * pLogger) {
    char const * during = "IVector::equals";

    if(operandsAreNullptr(pOperand1, pOperand2, during, pLogger)) {
        return RESULT_CODE::BAD_REFERENCE;
    }

    if(!equalDims(pOperand1, pOperand2, during, pLogger)) {
        return RESULT_CODE::WRONG_DIM;
    }

    if(std::isnan(tolerance)) {
        return Loggable::printLogDuring("Tolerance equal to NaN is passed", during, RESULT_CODE::NAN_VALUE, pLogger);
    }

    if(tolerance < 0) {
        return Loggable::printLogDuring("Tolerance less than zero is passed", during, RESULT_CODE::WRONG_ARGUMENT,
                                        pLogger);
    }

    IVector * diff = IVector::sub(pOperand1, pOperand2, pLogger);

    if(diff == nullptr) {
        return Loggable::printLogDuring("The difference between the operands turned out to be equal to nullptr", during,
                                        RESULT_CODE::CALCULATION_ERROR, pLogger);
    }

    double normDiff = diff->norm(norm);

    delete diff;
    diff = nullptr;

    if(std::isnan(normDiff)) {
        return Loggable::printLogDuring("The norm of the difference between the operands turned out to be equal to NaN",
                                        during, RESULT_CODE::NAN_VALUE, pLogger);
    }

    *result = normDiff <= tolerance;

    return RESULT_CODE::SUCCESS;
}



/* Vector */

Vector::Vector(size_t dim, double * pData, ILogger * pLogger) : IVector(), Loggable(pLogger), dim(dim), coords(pData) {}

Vector::~Vector() {
    delete [] coords;
    coords = nullptr;
}

IVector * Vector::clone() const {
    return IVector::createVector(dim, coords, logger);
}

double Vector::getCoord(size_t index) const {
    char const * during = "IVector::getCoord";

    if(index >= dim) {
        printLogDuring("Index of vector in set out of bounds", during, RESULT_CODE::OUT_OF_BOUNDS, logger);

        return std::numeric_limits <double>::quiet_NaN();
    }

    return coords[index];
}

RESULT_CODE Vector::setCoord(size_t index, double value) {
    char const * during = "IVector::setCoord";

    if(index >= dim) {
        return printLogDuring("Error in setting coord index", during, RESULT_CODE::OUT_OF_BOUNDS, logger);
    }

    if(std::isnan(value)) {
        return printLogDuring("Coord value is equal to NaN", during, RESULT_CODE::NAN_VALUE, logger);
    }

    coords[index] = value;

    return RESULT_CODE::SUCCESS;
}

double Vector::norm(NORM norm) const {
    char const * during = "IVector::norm";
    double normResult = 0.;

    switch(norm) {
    case NORM::NORM_1: {
        for(size_t i = 0; i < dim; ++i) {
            normResult += std::fabs(coords[i]);
        }

        break;
    }

    case NORM::NORM_2: {
        for(size_t i = 0; i < dim; ++i) {
            normResult += coords[i] * coords[i];
        }

        normResult = sqrt(normResult);

        break;
    }

    case NORM::NORM_INF: {
        for(size_t i = 0; i < dim; ++i) {
            double coordAbs = std::fabs(coords[i]);

            if(coordAbs > normResult) {
                normResult = coordAbs;
            }
        }

        break;
    }

    default: {
        printLogDuring("Invalid value of norm", during, RESULT_CODE::WRONG_ARGUMENT, logger);
        normResult = std::numeric_limits <double>::quiet_NaN();

        break;
    }
    }

    return normResult;
}

size_t Vector::getDim() const {
    return dim;
}

Vector * Vector::createVector(size_t dim, double * pData, ILogger * pLogger) {
    char const * during = "IVector::createVector";

    if(dim == 0) {
        printLogDuring("Trying to create a zero-dimensional vector", during, RESULT_CODE::WRONG_DIM, pLogger);

        return nullptr;
    }

    if(pData == nullptr) {
        printLogDuring("Trying to create a vector with nullptr coordinates array", during, RESULT_CODE::BAD_REFERENCE,
                       pLogger);

        return nullptr;
    }

    for(size_t i = 0; i < dim; ++i) {
        if(std::isnan(pData[i])) {
            printLogDuring("NaN vector component was found", during, RESULT_CODE::NAN_VALUE, pLogger);

            return nullptr;
        }
    }

    double * coords = new double[dim];

    if(coords == nullptr) {
        printLogDuring("Not enough memory to create the coordinates array", during, RESULT_CODE::OUT_OF_MEMORY, pLogger);

        return nullptr;
    }

    memcpy(coords, pData, dim * sizeof(double));
    Vector * vec = new Vector(dim, coords, pLogger);

    if(vec == nullptr) {
        printLogDuring("Not enough memory to create the vector", during, RESULT_CODE::OUT_OF_MEMORY, pLogger);
    }

    return vec;
}
