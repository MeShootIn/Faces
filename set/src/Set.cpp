#include <vector>
#include <math.h>
#include <string.h>
#include <string>
#include <algorithm>

#include "../include/ISet.h"



namespace {
typedef std::vector <IVector const *>::iterator setIterator;

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



class Set : public ISet, private Loggable {
public:
    ~Set() override;
    RESULT_CODE insert(const IVector * pVector, IVector::NORM norm, double tolerance) override;
    RESULT_CODE get(IVector * & pVector, size_t index) const override;
    RESULT_CODE get(IVector * & pVector, IVector const * pSample, IVector::NORM norm, double tolerance) const override;
    size_t getDim() const override;
    size_t getSize() const override;
    void clear() override;
    RESULT_CODE erase(size_t index) override;
    RESULT_CODE erase(IVector const * pSample, IVector::NORM norm, double tolerance) override;
    ISet * clone() const override;

    static Set * createSet(ILogger * pLogger);

private:
    explicit Set(ILogger * logger);
    Set(Set const & anotherSet) = delete;
    Set & operator = (Set const & anotherSet) = delete;

    setIterator findFirstClosest(IVector const * pSample, IVector::NORM norm, double tolerance) const;
    void insert(const IVector * pVector);

    mutable std::vector <IVector const *> set;
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

bool equalDims(ISet const * pOperand1, ISet const * pOperand2, char const * during, ILogger * pLogger) {
    if(pOperand1->getDim() != pOperand2->getDim()) {
        Loggable::printLogDuring("The dimensions of the vectors are not equal", during, RESULT_CODE::WRONG_DIM, pLogger);

        return false;
    }

    return true;
}



/* ISet */

ISet::~ISet() = default;

ISet * ISet::createSet(ILogger * pLogger) {
    return Set::createSet(pLogger);
}



/* Set */

Set::Set(ILogger * logger) : ISet(), Loggable(logger) {}

Set::~Set() {
    clear();
}

Set * Set::createSet(ILogger * pLogger) {
    char const * during = "ISet::createSet";
    Set * set = new Set(pLogger);

    if(set == nullptr) {
        printLogDuring("Not enough memory to create the set", during, RESULT_CODE::OUT_OF_MEMORY, pLogger);
    }

    return set;
}

void Set::insert(const IVector * pVector) {
    set.push_back(pVector);
}

RESULT_CODE Set::insert(IVector const * pVector, IVector::NORM norm, double tolerance) {
    char const * during = "ISet::insert";

    if(pVector == nullptr) {
        return printLogDuring("Passed a vector with a null pointer", during, RESULT_CODE::BAD_REFERENCE, logger);
    }

    if(std::isnan(tolerance)) {
        return printLogDuring("Tolerance equal to NaN is passed", during, RESULT_CODE::NAN_VALUE, logger);
    }

    if(tolerance < 0) {
        return printLogDuring("Tolerance less than zero is passed", during, RESULT_CODE::WRONG_ARGUMENT, logger);
    }

    if(getSize() != 0 && pVector->getDim() != getDim()) {
        return printLogDuring("Vector and set dimensions are not equal", during, RESULT_CODE::WRONG_DIM, logger);
    }

    setIterator it = findFirstClosest(pVector, norm, tolerance);

    if(it != set.end()) {
        return RESULT_CODE::MULTIPLE_DEFINITION;
    }

    auto cloned = pVector->clone();

    if(cloned == nullptr) {
        return printLogDuring("Failed to clone vector", during, RESULT_CODE::OUT_OF_MEMORY, logger);
    }

    insert(cloned);

    return RESULT_CODE::SUCCESS;
}

RESULT_CODE Set::get(IVector * & pVector, size_t index) const {
    char const * during = "ISet::get";

    if(index >= getSize()) {
        return printLogDuring("Index of vector in set out of bounds", during, RESULT_CODE::OUT_OF_BOUNDS, logger);
    }

    auto cloned = set[index]->clone();

    if(cloned == nullptr) {
        return printLogDuring("Failed to clone vector", during, RESULT_CODE::OUT_OF_MEMORY, logger);
    }

    pVector = cloned;

    return RESULT_CODE::SUCCESS;
}

RESULT_CODE Set::get(IVector * & pVector, IVector const * pSample, IVector::NORM norm, double tolerance) const {
    char const * during = "ISet::get";

    if(pSample == nullptr) {
        return printLogDuring("Passed a vector with a null pointer", during, RESULT_CODE::BAD_REFERENCE, logger);
    }

    if(std::isnan(tolerance)) {
        return printLogDuring("Tolerance equal to NaN is passed", during, RESULT_CODE::NAN_VALUE, logger);
    }

    if(tolerance < 0) {
        return printLogDuring("Tolerance less than zero is passed", during, RESULT_CODE::WRONG_ARGUMENT, logger);
    }

    if(pSample->getDim() != getDim()) {
        return printLogDuring("Vector and set dimensions are not equal", during, RESULT_CODE::WRONG_DIM, logger);
    }

    setIterator it = findFirstClosest(pSample, norm, tolerance);

    if(it == set.end()) {
        return RESULT_CODE::NOT_FOUND;
    }

    auto cloned = (*it)->clone();

    if(cloned == nullptr) {
        return printLogDuring("Failed to clone vector", during, RESULT_CODE::OUT_OF_MEMORY, logger);
    }

    pVector = cloned;

    return RESULT_CODE::SUCCESS;
}

size_t Set::getDim() const {
    if(set.empty()) {
        return 0;
    }

    return set[0]->getDim();
}

size_t Set::getSize() const {
    return set.size();
}

void Set::clear() {
    for(auto & vector : set) {
        delete vector;
        vector = nullptr;
    }

    set.clear();
}

RESULT_CODE Set::erase(size_t index) {
    char const * during = "ISet::erase";

    if(index >= getSize()) {
        return printLogDuring("Index of vector in set out of bounds", during, RESULT_CODE::OUT_OF_BOUNDS, logger);
    }

    setIterator it = set.begin() + index;

    delete *it;
    *it = nullptr;
    set.erase(it);

    return RESULT_CODE::SUCCESS;
}

RESULT_CODE Set::erase(IVector const * pSample, IVector::NORM norm, double tolerance) {
    char const * during = "ISet::erase";

    if(pSample == nullptr) {
        return printLogDuring("Passed a vector with a null pointer", during, RESULT_CODE::BAD_REFERENCE, logger);
    }

    if(std::isnan(tolerance)) {
        return printLogDuring("Tolerance equal to NaN is passed", during, RESULT_CODE::NAN_VALUE, logger);
    }

    if(tolerance < 0) {
        return printLogDuring("Tolerance less than zero is passed", during, RESULT_CODE::WRONG_ARGUMENT, logger);
    }

    if(pSample->getDim() != getDim()) {
        return printLogDuring("Vector and set dimensions are not equal", during, RESULT_CODE::WRONG_DIM, logger);
    }

    setIterator it = findFirstClosest(pSample, norm, tolerance);

    if(it == set.end()) {
        return RESULT_CODE::NOT_FOUND;
    }

    delete *it;
    *it = nullptr;
    set.erase(it);

    return RESULT_CODE::SUCCESS;
}

ISet * Set::clone() const {
    char const * during = "ISet::clone";
    Set * copy = Set::createSet(logger);

    if(copy == nullptr) {
        printLogDuring("Failed to clone set", during, RESULT_CODE::OUT_OF_MEMORY, logger);

        return nullptr;
    }

    for(auto const & vector : set) {
        IVector * cloned = vector->clone();

        if(cloned == nullptr) {
            printLogDuring("Failed to clone vector", during, RESULT_CODE::OUT_OF_MEMORY, logger);

            delete copy;
            return copy = nullptr;
        }

        copy->insert(cloned);
    }

    return copy;
}

setIterator Set::findFirstClosest(IVector const * pSample, IVector::NORM norm, double tolerance) const {
    setIterator const ERROR = set.end();

    if(pSample == nullptr || std::isnan(tolerance) || tolerance < 0 || pSample->getDim() != getDim()) {
        return ERROR;
    }

    for(setIterator it = set.begin(); it != set.end(); ++it) {
        bool result = false;
        RESULT_CODE equalsResult = IVector::equals(pSample, *it, norm, tolerance, &result, logger);

        if(equalsResult == RESULT_CODE::SUCCESS && result) {
            return it;
        }
    }

    return ERROR;
}

ISet * ISet::add(
        ISet const * pOperand1, ISet const * pOperand2, IVector::NORM norm, double tolerance, ILogger * pLogger
        ) {
    char const * during = "ISet::add";

    if(operandsAreNullptr(pOperand1, pOperand2, during, pLogger) || !equalDims(pOperand1, pOperand2, during, pLogger)) {
        return nullptr;
    }

    if(std::isnan(tolerance)) {
        Loggable::printLogDuring("Tolerance equal to NaN is passed", during, RESULT_CODE::NAN_VALUE, pLogger);

        return nullptr;
    }

    if(tolerance < 0) {
        Loggable::printLogDuring("Tolerance less than zero is passed", during, RESULT_CODE::WRONG_ARGUMENT, pLogger);

        return nullptr;
    }

    ISet * sum = pOperand1->clone();

    if(sum == nullptr) {
        Loggable::printLogDuring("Failed to clone set", during, RESULT_CODE::OUT_OF_MEMORY, pLogger);

        return nullptr;
    }

    IVector * vector = nullptr;

    for(size_t i = 0; i < pOperand2->getSize(); ++i) {
        pOperand2->get(vector, i);
        sum->insert(vector, norm, tolerance);

        delete vector;
        vector = nullptr;
    }

    return sum;
}

ISet * ISet::intersect(
        ISet const * pOperand1, ISet const * pOperand2, IVector::NORM norm, double tolerance, ILogger * pLogger
        ) {
    char const * during = "ISet::intersect";

    if(operandsAreNullptr(pOperand1, pOperand2, during, pLogger) || !equalDims(pOperand1, pOperand2, during, pLogger)) {
        return nullptr;
    }

    if(std::isnan(tolerance)) {
        Loggable::printLogDuring("Tolerance equal to NaN is passed", during, RESULT_CODE::NAN_VALUE, pLogger);

        return nullptr;
    }

    if(tolerance < 0) {
        Loggable::printLogDuring("Tolerance less than zero is passed", during, RESULT_CODE::WRONG_ARGUMENT, pLogger);

        return nullptr;
    }

    ISet * intersection = ISet::createSet(pLogger);

    if(intersection == nullptr) {
        Loggable::printLogDuring("Failed to create intersection set", during, RESULT_CODE::OUT_OF_MEMORY, pLogger);

        return nullptr;
    }

    IVector * vector = nullptr,
            * vectorFounded = nullptr;

    for(size_t i = 0; i < pOperand1->getSize(); ++i) {
        pOperand1->get(vector, i);

        RESULT_CODE result = pOperand2->get(vectorFounded, vector, norm, tolerance);

        if(result == RESULT_CODE::SUCCESS) {
            intersection->insert(vector, norm, tolerance);
        }

        delete vectorFounded;
        delete vector;

        vectorFounded = nullptr;
        vector = nullptr;
    }

    return intersection;
}

ISet * ISet::sub(
        ISet const * pOperand1, ISet const * pOperand2, IVector::NORM norm, double tolerance, ILogger * pLogger
        ) {
    char const * during = "ISet::sub";

    if(operandsAreNullptr(pOperand1, pOperand2, during, pLogger) || !equalDims(pOperand1, pOperand2, during, pLogger)) {
        return nullptr;
    }

    if(std::isnan(tolerance)) {
        Loggable::printLogDuring("Tolerance equal to NaN is passed", during, RESULT_CODE::NAN_VALUE, pLogger);

        return nullptr;
    }

    if(tolerance < 0) {
        Loggable::printLogDuring("Tolerance less than zero is passed", during, RESULT_CODE::WRONG_ARGUMENT, pLogger);

        return nullptr;
    }

    ISet * diff = ISet::createSet(pLogger);

    if(diff == nullptr) {
        Loggable::printLogDuring("Failed to create difference set", during, RESULT_CODE::OUT_OF_MEMORY, pLogger);

        return nullptr;
    }

    IVector * vector = nullptr,
            * vectorFound = nullptr;

    for(size_t i = 0; i < pOperand1->getSize(); ++i) {
        pOperand1->get(vector, i);

        RESULT_CODE result = pOperand2->get(vectorFound, vector, norm, tolerance);

        if(result == RESULT_CODE::NOT_FOUND) {
            diff->insert(vector, norm, tolerance);
        }

        delete vectorFound;
        delete vector;

        vectorFound = nullptr;
        vector = nullptr;
    }

    return diff;
}

ISet * ISet::symSub(
        ISet const * pOperand1, ISet const * pOperand2, IVector::NORM norm, double tolerance, ILogger * pLogger
        ) {
    char const * during = "ISet::symSub";

    if(operandsAreNullptr(pOperand1, pOperand2, during, pLogger) || !equalDims(pOperand1, pOperand2, during, pLogger)) {
        return nullptr;
    }

    if(std::isnan(tolerance)) {
        Loggable::printLogDuring("Tolerance equal to NaN is passed", during, RESULT_CODE::NAN_VALUE, pLogger);

        return nullptr;
    }

    if(tolerance < 0) {
        Loggable::printLogDuring("Tolerance less than zero is passed", during, RESULT_CODE::WRONG_ARGUMENT, pLogger);

        return nullptr;
    }

    ISet * sum = ISet::add(pOperand1, pOperand2, norm, tolerance, pLogger);
    ISet * intersection = ISet::intersect(pOperand1, pOperand2, norm, tolerance, pLogger);
    ISet * symsub = ISet::sub(sum, intersection, norm, tolerance, pLogger);

    delete sum;
    delete intersection;

    intersection = nullptr;
    sum = nullptr;

    return symsub;
}
