#include <string.h>
#include <string>
#include <cmath>

#include "../include/ICompact.h"
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



class Compact : public ICompact, private Loggable {
public:
    IVector * getBegin() const override;
    IVector * getEnd() const override;
    ICompact::iterator * end(IVector const * const step) override;
    ICompact::iterator * begin(IVector const * const step) override;
    RESULT_CODE isContains(IVector const * const vec, bool & result) const override;
    RESULT_CODE isSubSet(ICompact const * const other, bool & result) const override;
    RESULT_CODE isIntersects(ICompact const * const other, bool & result) const override;
    size_t getDim() const override;
    ICompact * clone() const override;
    ~Compact() override;

    static Compact * createCompact(IVector const * const begin, IVector const * const end, ILogger * logger);

    static double const TOLERANCE;



    class Iterator : public ICompact::iterator {
    public:
        Iterator(ICompact const * compact, IVector const * step, bool reverse, ILogger * logger);
        RESULT_CODE doStep() override;
        IVector * getPoint() const override;
        RESULT_CODE setDirection(IVector const * const dir) override;

        ~Iterator();

    protected:
        Iterator() = default;

    private:
        Iterator(const Iterator & other) = delete;
        Iterator & operator = (const Iterator & other) = delete;

        bool reverse;
        ICompact const * compact;
        IVector * current;
        IVector const * step;
        IVector * dir;
        ILogger * logger;
    };

private:
    Compact(Compact const & anotherCompact) = delete;
    Compact & operator = (Compact const & anotherCompact) = delete;
    Compact(IVector * left, IVector * right, ILogger * logger);

    bool checkStep(IVector const * const step, bool reversed, char const * during) const;

    IVector * vecBegin;
    IVector * vecEnd;
    size_t dim;
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

bool operandsAreNullptr(void const * left, void const * right, char const * during, ILogger * pLogger) {
    if(left == nullptr || right == nullptr) {
        if(left == nullptr && right != nullptr) {
            Loggable::printLogDuring("Left operand turned out to be equal to nullptr", during,
                                     RESULT_CODE::BAD_REFERENCE, pLogger);
        }

        if(right == nullptr && left != nullptr) {
            Loggable::printLogDuring("Right operand turned out to be equal to nullptr", during,
                                     RESULT_CODE::BAD_REFERENCE, pLogger);
        }

        if(left == nullptr && right == nullptr) {
            Loggable::printLogDuring("Both operands turned out to be equal to nullptr", during,
                                     RESULT_CODE::BAD_REFERENCE, pLogger);
        }

        return true;
    }

    return false;
}

bool equalDims(IVector const * left, IVector const * right, char const * during, ILogger * pLogger) {
    if(operandsAreNullptr(left, right, during, pLogger)) {
        return false;
    }

    if(left->getDim() != right->getDim()) {
        Loggable::printLogDuring("The dimensions of the vectors are not equal", during, RESULT_CODE::WRONG_DIM, pLogger);

        return false;
    }

    return true;
}

bool equalDims(ICompact const * left, ICompact const * right, char const * during, ILogger * pLogger) {
    if(operandsAreNullptr(left, right, during, pLogger)) {
        return false;
    }

    if(left->getDim() != right->getDim()) {
        Loggable::printLogDuring("The dimensions of the compacts are not equal", during, RESULT_CODE::WRONG_DIM, pLogger);

        return false;
    }

    return true;
}

bool checkLess(IVector const * left, IVector const * right, const char * during, ILogger * pLogger) {
    if(operandsAreNullptr(left, right, during, pLogger) || !equalDims(left, right, during, pLogger)) {
        return false;
    }

    for(size_t i = 0; i < left->getDim(); ++i) {
        if(left->getCoord(i) > right->getCoord(i)) {
            return false;
        }
    }

    return true;
}

IVector const * min(IVector const * left, IVector const * right, const char * during, ILogger * pLogger) {
    if(checkLess(left, right, during, pLogger)) {
        return left;
    }

    return right;
}

IVector const * max(IVector const * left, IVector const * right, const char * during, ILogger * pLogger) {
    if(!checkLess(left, right, during, pLogger)) {
        return left;
    }

    return right;
}

bool equals(double a, double b) {
    return std::fabs(a - b) <= Compact::TOLERANCE;
}

bool checkParallel(IVector const * const vector, int & axisIndex) {
    int nonZeroCount = 0;
    double norm = vector->norm(IVector::NORM::NORM_INF);

    for(size_t i = 0; i < vector->getDim() && nonZeroCount <= 1; ++i) {
        if(!equals(vector->getCoord(i) / norm, 0)) {
            axisIndex = i;
            ++nonZeroCount;
        }
    }

    return nonZeroCount <= 1;
}

bool compactInsideCompact(ICompact const * const left, ICompact const * const right, char const * during,
                          ILogger * logger) {
    return checkLess(left->getBegin(), right->getBegin(), during, logger) &&
            checkLess(left->getEnd(), right->getEnd(), during, logger);
}

bool checkUnique(IVector const * const vector, size_t idx) {
    size_t dim = vector->getDim();
    double coord = vector->getCoord(idx);

    if(coord < 0 || coord > dim - 1) {
        return false;
    }

    for(size_t i = 0; i < dim; ++i) {
        if(i == idx) {
            continue;
        }

        if(equals(vector->getCoord(i), coord)) {
            return false;
        }
    }

    return true;
}

bool hasNaNCoord(IVector * vector, char const * during, ILogger * pLogger) {
    for(size_t i = 0; i < vector->getDim(); ++i) {
        if(std::isnan(vector->getCoord(i))) {
            Loggable::printLogDuring("Passed vector with NaN coordinate", during, RESULT_CODE::NAN_VALUE, pLogger);

            return true;
        }
    }

    return false;
}



/* ICompact */

ICompact::~ICompact() = default;

ICompact * ICompact::createCompact(IVector const * const begin, IVector const * const end, ILogger * logger) {
    return Compact::createCompact(begin, end, logger);
}



/* Compact */

double const Compact::TOLERANCE = 1e-6;

Compact::Compact(IVector * left, IVector * right, ILogger * logger) : ICompact(), Loggable(logger), vecBegin(left),
    vecEnd(right), dim(left->getDim()) {}

Compact * Compact::createCompact(IVector const * const begin, IVector const * const end, ILogger * logger) {
    char const * during = "ICompact::createCompact";

    if(operandsAreNullptr(begin, end, during, logger) || !equalDims(begin, end, during, logger)) {
        return nullptr;
    }

    if(!checkLess(begin, end, during, logger)) {
        return nullptr;
    }

    auto clonedBegin = const_cast <IVector *> (begin)->clone();

    if(hasNaNCoord(clonedBegin, during, logger)) {
        delete clonedBegin;
        clonedBegin = nullptr;

        return nullptr;
    }

    if(clonedBegin == nullptr) {
        printLogDuring("Failed to clone vector", during, RESULT_CODE::OUT_OF_MEMORY, logger);

        return nullptr;
    }

    auto clonedEnd = const_cast <IVector *> (end)->clone();

    if(hasNaNCoord(clonedEnd, during, logger)) {
        delete clonedBegin;
        delete clonedEnd;

        clonedBegin = nullptr;
        clonedEnd = nullptr;

        return nullptr;
    }

    if(clonedEnd == nullptr) {
        printLogDuring("Failed to clone vector", during, RESULT_CODE::OUT_OF_MEMORY, logger);

        return nullptr;
    }

    Compact * compact = new Compact(clonedBegin, clonedEnd, logger);

    if(compact == nullptr) {
        printLogDuring("Not enough memory to create the compact", during, RESULT_CODE::OUT_OF_MEMORY, logger);

        delete clonedBegin;
        delete clonedEnd;

        clonedBegin = nullptr;
        clonedEnd = nullptr;
    }

    return compact;
}

Compact::~Compact() {
    delete vecBegin;
    delete vecEnd;

    vecBegin = nullptr;
    vecEnd = nullptr;
}

IVector * Compact::getBegin() const {
    char const * during = "ICompact::getBegin";
    IVector * cloned = vecBegin->clone();

    if(cloned == nullptr) {
        printLogDuring("Failed to clone vector", during, RESULT_CODE::OUT_OF_MEMORY, logger);
    }

    return cloned;
}

IVector * Compact::getEnd() const {
    char const * during = "ICompact::getEnd";
    IVector * cloned = vecEnd->clone();

    if(cloned == nullptr) {
        printLogDuring("Failed to clone vector", during, RESULT_CODE::OUT_OF_MEMORY, logger);
    }

    return cloned;
}

ICompact::iterator * Compact::begin(IVector const * const step = nullptr) {
    char const * during = "ICompact::begin";

    if(!checkStep(step, false, during)) {
        return nullptr;
    }

    Compact::Iterator * it = new Compact::Iterator(this, step, false, logger);

    if(it == nullptr) {
        printLogDuring("Failed to create iterator instance", during, RESULT_CODE::OUT_OF_MEMORY, logger);
    }

    return it;
}

ICompact::iterator * Compact::end(IVector const * const step = nullptr) {
    char const * during = "ICompact::end";

    if(!checkStep(step, true, during)) {
        return nullptr;
    }

    Compact::Iterator * it = new Compact::Iterator(this, step, true, logger);

    if(it == nullptr) {
        printLogDuring("Failed to create iterator instance", during, RESULT_CODE::OUT_OF_MEMORY, logger);
    }

    return it;
}

RESULT_CODE Compact::isContains(IVector const * const vec, bool & result) const {
    char const * during = "ICompact::isContains";

    if(vec == nullptr) {
        return printLogDuring("Passed a vector with a null pointer", during, RESULT_CODE::BAD_REFERENCE, logger);
    }

    if(vec->getDim() != getDim()) {
        return printLogDuring("Vector and compact dimensions are not equal", during, RESULT_CODE::WRONG_DIM, logger);
    }

    result = checkLess(vecBegin, vec, during, logger) && checkLess(vec, vecEnd, during, logger);

    return RESULT_CODE::SUCCESS;
}

RESULT_CODE Compact::isSubSet(ICompact const * const other, bool & result) const {
    char const * during = "ICompact::isSubSet";

    if(other == nullptr) {
        return printLogDuring("Passed a compact with a null pointer", during, RESULT_CODE::BAD_REFERENCE, logger);
    }

    if(other->getDim() != getDim()) {
        return printLogDuring("The dimensions of the current and passed compact are not equal", during,
                              RESULT_CODE::WRONG_DIM, logger);
    }

    bool iscontains;
    RESULT_CODE containsResult = isContains(other->getBegin(), iscontains);

    if(containsResult != RESULT_CODE::SUCCESS) {
        return printLogDuring("Failed to check begin vector containing in current compact", during, containsResult,
                              logger);
    }

    if(iscontains) {
        containsResult = isContains(other->getEnd(), iscontains);

        if(containsResult != RESULT_CODE::SUCCESS) {
            return printLogDuring("Failed to check end vector containing in current compact", during, containsResult,
                                  logger);
        }

        result = iscontains;
    }

    return RESULT_CODE::SUCCESS;
}

RESULT_CODE Compact::isIntersects(ICompact const * const other, bool & result) const {
    char const * during = "ICompact::isContains";

    if(other == nullptr) {
        return printLogDuring("Passed a compact with a null pointer", during, RESULT_CODE::BAD_REFERENCE, logger);
    }

    if(other->getDim() != getDim()) {
        return printLogDuring("Current and passed compact dimensions are not equal", during, RESULT_CODE::WRONG_DIM,
                              logger);
    }

    IVector const * begin = max(vecBegin, other->getBegin(), during, logger);
    IVector const * end = min(vecEnd, other->getEnd(), during, logger);

    result = checkLess(begin, end, during, logger);

    return RESULT_CODE::SUCCESS;
}

size_t Compact::getDim() const {
    return dim;
}

ICompact * Compact::clone() const {
    return ICompact::createCompact(vecBegin, vecEnd, logger);
}

bool Compact::checkStep(IVector const * const step, bool reversed, char const * during) const {
    if(step == nullptr) {
        printLogDuring("Passed a vector with a null pointer", during, RESULT_CODE::BAD_REFERENCE, logger);

        return false;
    }

    if(step->getDim() != getDim()) {
        printLogDuring("Vector and compact dimensions are not equal", during, RESULT_CODE::WRONG_DIM, logger);

        return false;
    }

    for(size_t i = 0; i < dim; ++i) {
        double coord = step->getCoord(i);

        if(std::isnan(coord)) {
            printLogDuring("Passed a vector with NaN coord", during, RESULT_CODE::NAN_VALUE, logger);

            return false;
        }

        if((coord < 0 && !reversed) || (coord > 0 && reversed) || equals(coord, 0)) {
            return false;
        }
    }

    return true;
}

ICompact * ICompact::intersection(ICompact const * const left, ICompact const * const right, ILogger * logger) {
    char const * during = "ICompact::intersection";

    if(operandsAreNullptr(left, right, during, logger) || !equalDims(left, right, during, logger)) {
        return nullptr;
    }

    size_t dim = left->getDim();
    bool intersects;
    RESULT_CODE intersectsResult = left->isIntersects(right, intersects);

    if(intersectsResult != RESULT_CODE::SUCCESS) {
        Loggable::printLogDuring("Failed to check intersection", during, RESULT_CODE::BAD_REFERENCE, logger);

        return nullptr;
    }

    if(intersects) {
        double * data = new double[dim] {0};

        if(data == nullptr) {
            Loggable::printLogDuring("Failed to create array of doubles", during, RESULT_CODE::OUT_OF_MEMORY, logger);

            return nullptr;
        }

        IVector * l = IVector::createVector(dim, data, logger);

        if(l == nullptr) {
            Loggable::printLogDuring("Failed to create vector", during, RESULT_CODE::OUT_OF_MEMORY, logger);

            delete [] data;
            data = nullptr;

            return nullptr;
        }

        IVector * r = IVector::createVector(dim, data, logger);

        if(r == nullptr) {
            Loggable::printLogDuring("Failed to create vector", during, RESULT_CODE::OUT_OF_MEMORY, logger);

            delete [] data;
            delete l;

            data = nullptr;
            l = nullptr;

            return nullptr;
        }

        IVector * lBegin = left->getBegin();
        IVector * rBegin = right->getBegin();
        IVector * lEnd = left->getEnd();
        IVector * rEnd = right->getEnd();

        for(size_t i = 0; i < dim; i++) {
            l->setCoord(i, std::max(lBegin->getCoord(i), rBegin->getCoord(i)));
            r->setCoord(i, std::min(lEnd->getCoord(i), rEnd->getCoord(i)));
        }

        delete lBegin;
        delete rBegin;
        delete lEnd;
        delete rEnd;

        lBegin = nullptr;
        rBegin = nullptr;
        lEnd = nullptr;
        rEnd = nullptr;

        ICompact * compact = createCompact(l, r, logger);

        delete l;
        delete r;

        l = nullptr;
        r = nullptr;

        if(compact == nullptr) {
            Loggable::printLogDuring("Failed to create compact", during, RESULT_CODE::OUT_OF_MEMORY, logger);
        }

        return compact;
    }

    return nullptr;
}

ICompact * ICompact::add(ICompact const * const left, ICompact const * const right, ILogger * logger) {
    char const * during = "ICompact::add";

    if(operandsAreNullptr(left, right, during, logger) || !equalDims(left, right, during, logger)) {
        return nullptr;
    }

    if(compactInsideCompact(left, right, during, logger)) {
        return left->clone();
    }

    if(compactInsideCompact(right, left, during, logger)) {
        return right->clone();
    }

    IVector * lBegin = left->getBegin();
    IVector * rBegin = right->getBegin();
    IVector * lEnd = left->getEnd();
    IVector * rEnd = right->getEnd();

    if(checkLess(lEnd, rBegin, during, logger) || checkLess(rEnd, lBegin, during, logger)) {
        delete lBegin;
        delete rBegin;
        delete lEnd;
        delete rEnd;

        lBegin = nullptr;
        rBegin = nullptr;
        lEnd = nullptr;
        rEnd = nullptr;

        return nullptr;
    }

    IVector * diffBegin = IVector::sub(lBegin, rBegin, logger);

    if(diffBegin == nullptr) {
        Loggable::printLogDuring("Failed to sub vectors", during, RESULT_CODE::WRONG_ARGUMENT, logger);

        delete lBegin;
        delete rBegin;
        delete lEnd;
        delete rEnd;

        lBegin = nullptr;
        rBegin = nullptr;
        lEnd = nullptr;
        rEnd = nullptr;

        return nullptr;
    }

    int axisIndexBegin, axisIndexEnd;

    if(checkParallel(diffBegin, axisIndexBegin)) {
        IVector * diffEnd = IVector::sub(lEnd, rEnd, logger);

        if(diffEnd == nullptr) {
            Loggable::printLogDuring("Failed to sub vectors", during, RESULT_CODE::WRONG_ARGUMENT, logger);

            delete lBegin;
            delete rBegin;
            delete lEnd;
            delete rEnd;

            lBegin = nullptr;
            rBegin = nullptr;
            lEnd = nullptr;
            rEnd = nullptr;

            return nullptr;
        }

        if(checkParallel(diffEnd, axisIndexEnd)) {
            if(axisIndexBegin == axisIndexEnd) {
                ICompact * compact = createCompact(min(lBegin, rBegin, during, logger), max(lEnd, rEnd, during, logger),
                                                   logger);

                if(compact == nullptr) {
                    Loggable::printLogDuring("Failed to create compact", during, RESULT_CODE::OUT_OF_MEMORY, logger);
                }

                delete diffBegin;
                delete diffEnd;
                delete lBegin;
                delete rBegin;
                delete lEnd;
                delete rEnd;

                diffBegin = nullptr;
                diffEnd = nullptr;
                lBegin = nullptr;
                rBegin = nullptr;
                lEnd = nullptr;
                rEnd = nullptr;

                return compact;
            }
        }

        delete diffEnd;
    }

    delete diffBegin;
    delete lBegin;
    delete rBegin;
    delete lEnd;
    delete rEnd;

    diffBegin = nullptr;
    lBegin = nullptr;
    rBegin = nullptr;
    lEnd = nullptr;
    rEnd = nullptr;

    return nullptr;
}

ICompact * ICompact::makeConvex(ICompact const * const left, ICompact const * const right, ILogger * logger) {
    char const * during = "ICompact::makeConvex";

    if(operandsAreNullptr(left, right, during, logger) || !equalDims(left, right, during, logger)) {
        return nullptr;
    }

    IVector * lBegin = left->getBegin();
    IVector * rBegin = right->getBegin();
    IVector * lEnd = left->getEnd();
    IVector * rEnd = right->getEnd();

    ICompact * compact = createCompact(min(lBegin, rBegin, during, logger), max(lEnd, rEnd, during, logger), logger);

    delete lBegin;
    delete rBegin;
    delete lEnd;
    delete rEnd;

    lBegin = nullptr;
    rBegin = nullptr;
    lEnd = nullptr;
    rEnd = nullptr;

    if(compact == nullptr) {
        Loggable::printLogDuring("Failed to create compact", during, RESULT_CODE::OUT_OF_MEMORY, logger);
    }

    return compact;
}



/* Iterator */

Compact::Iterator::Iterator(ICompact const * compact, IVector const * step, bool reverse, ILogger * logger) :
    ICompact::iterator(), reverse(reverse), compact(compact->clone()), step(step->clone()), logger(logger) {
    if(!reverse) {
        current = compact->getBegin()->clone();
    }
    else {
        current = compact->getEnd()->clone();
    }

    size_t dim = compact->getDim();
    double * coords = new double[dim];

    for(size_t i = 0; i < dim; ++i) {
        coords[i] = i;
    }

    dir = IVector::createVector(dim, coords, logger);

    delete [] coords;
    coords = nullptr;
}

RESULT_CODE Compact::Iterator::doStep() {
    char const * during = "ICompact::iterator::doStep";
    IVector * begin = compact->getBegin();
    IVector * end = compact->getEnd();
    bool equalsBool;
    RESULT_CODE equalsResult;

    if(!reverse) {
        equalsResult = IVector::equals(end, current, IVector::NORM::NORM_2, TOLERANCE, &equalsBool, logger);
    }
    else {
        equalsResult = IVector::equals(begin, current, IVector::NORM::NORM_2, TOLERANCE, &equalsBool, logger);
    }

    if(equalsBool || equalsResult != RESULT_CODE::SUCCESS) {
        delete begin;
        delete end;

        begin = nullptr;
        end = nullptr;

        return RESULT_CODE::OUT_OF_BOUNDS;
    }

    IVector * currentCloned = current->clone();

    if(currentCloned == nullptr) {
        delete begin;
        delete end;

        begin = nullptr;
        end = nullptr;

        return printLogDuring("Failed to clone vector", during, RESULT_CODE::OUT_OF_MEMORY, logger);
    }

    size_t dim = compact->getDim();
    bool done = false;

    for(size_t i = 0; i < dim && !done; ++i) {
        size_t idx = (size_t) std::round(dir->getCoord(i));

        if(!reverse) {
            if(equals(currentCloned->getCoord(idx), end->getCoord(idx))) {
                currentCloned->setCoord(idx, begin->getCoord(idx));

                continue;
            }
        }
        else {
            if(equals(currentCloned->getCoord(idx), begin->getCoord(idx))) {
                currentCloned->setCoord(idx, end->getCoord(idx));

                continue;
            }
        }

        currentCloned->setCoord(idx, currentCloned->getCoord(idx) + step->getCoord(idx));

        bool contains;
        RESULT_CODE containsResult = compact->isContains(currentCloned, contains);

        if(containsResult != RESULT_CODE::SUCCESS) {
            delete currentCloned;
            currentCloned = nullptr;

            return containsResult;
        }

        if(!contains) {
            if(!reverse) {
                currentCloned->setCoord(idx, end->getCoord(idx));
            }
            else {
                currentCloned->setCoord(idx, begin->getCoord(idx));
            }
        }

        done = true;
    }

    if(done) {
        for(size_t i = 0; i < dim; ++i) {
            current->setCoord(i, currentCloned->getCoord(i));
        }
    }

    delete currentCloned;
    currentCloned = nullptr;

    return RESULT_CODE::SUCCESS;
}

IVector * Compact::Iterator::getPoint() const {
    char const * during = "ICompact::iterator::getPoint";
    IVector * cloned = current->clone();

    if(cloned == nullptr) {
        printLogDuring("Failed to clone vector", during, RESULT_CODE::OUT_OF_MEMORY, logger);
    }

    return cloned;
}

RESULT_CODE Compact::Iterator::setDirection(IVector const * const dir) {
    char const * during = "ICompact::iterator::setDirection";

    if(dir == nullptr) {
        return printLogDuring("Passed a vector with a null pointer", during, RESULT_CODE::BAD_REFERENCE, logger);
    }

    if(dir->getDim() != compact->getDim()) {
        return printLogDuring("Compact and vector dimensions are not equal", during, RESULT_CODE::WRONG_DIM, logger);
    }

    size_t dim = dir->getDim();

    for(size_t i = 0; i < dim; ++i) {
        if(!checkUnique(dir, i)) {
            return printLogDuring("Direction vector has repeated coordinates", during, RESULT_CODE::WRONG_ARGUMENT,
                                  logger);
        }
    }

    for(size_t i = 0; i < dim; ++i) {
        double coord = dir->getCoord(i);

        if(!equals(coord, std::round(coord))) {
            return printLogDuring("Non integer direction vector coordinate is passed", during,
                                  RESULT_CODE::WRONG_ARGUMENT, logger);
        }
    }

    delete this->dir;
    delete current;

    this->dir = dir->clone();

    if(!reverse) {
        current = compact->getBegin();
    }
    else {
        compact->getEnd();
    }

    return RESULT_CODE::SUCCESS;
}

Compact::Iterator::~Iterator() {
    delete dir;
    delete step;
    delete current;
    delete compact;

    dir = nullptr;
    step = nullptr;
    current = nullptr;
    compact = nullptr;
}
