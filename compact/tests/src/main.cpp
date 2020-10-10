#include <iostream>
#include <limits>

#include "../include/ICompact.h"
#include "../include/IVector.h"

using namespace std;



/* Global vars */

ILogger * logger = ILogger::createLogger(nullptr);
IVector::NORM const NORM = IVector::NORM::NORM_2;
double const TOLERANCE = 1e-6;
double vCoords [] = {1., 2.};
double wCoords [] = {-3., -4., -5.};
double xCoords [] = {6., 7., 8.};
double stepCoords [] = {0.1, 0.2, 0.3};
double stepReversedCoords [] = {-0.1, -0.2, -0.3};
IVector * v = IVector::createVector(2, vCoords, logger);
IVector * w = IVector::createVector(3, wCoords, logger);
IVector * x = IVector::createVector(3, xCoords, logger);
IVector * step = IVector::createVector(3, stepCoords, logger);
IVector * stepReversed = IVector::createVector(3, stepReversedCoords, logger);



/* Test function */

bool passed = true;

void test(char const * testFunctionName, bool (* testFunction)()) {
    bool res = testFunction();
    passed &= res;

    cout << testFunctionName << ": " << (res ? "OK" : "WRONG ANSWER") << "\n";
}



/* Secondary functions */

bool equalVectors(IVector * vector1, IVector * vector2) {
    bool result = false;
    RESULT_CODE resultCode = IVector::equals(vector1, vector2, NORM, TOLERANCE, &result, logger);

    return resultCode == RESULT_CODE::SUCCESS && result;
}

void printVector(IVector const * const vector) {
    if(vector == nullptr) {
        cout << "Passed vector == nullptr to printVector\n";

        return;
    }

    for(size_t i = 0; i < vector->getDim(); ++i) {
        cout << vector->getCoord(i) << " ";
    }

    cout << "\n";
}



/* Test functions */

bool testCreateCompact() {
    ICompact * compact = ICompact::createCompact(w, x, logger);
    bool result = compact != nullptr;

    delete compact;
    compact = nullptr;

    return result;
}

bool testCreateCompactNaN() {
    double coordsBeginNaN [] = {1., numeric_limits <double>::quiet_NaN(), 3.};
    double coordsEndNaN [] = {4., numeric_limits <double>::quiet_NaN(), 6.};
    IVector * begin = IVector::createVector(3, coordsBeginNaN, logger);
    IVector * end = IVector::createVector(3, coordsEndNaN, logger);
    ICompact * compact1 = ICompact::createCompact(begin, x, logger);
    ICompact * compact2 = ICompact::createCompact(w, end, logger);
    ICompact * compact3 = ICompact::createCompact(begin, end, logger);
    bool result = compact1 == nullptr && compact2 == nullptr && compact3 == nullptr;

    delete begin;
    delete end;
    delete compact1;
    delete compact2;
    delete compact3;

    begin = nullptr;
    end = nullptr;
    compact1 = nullptr;
    compact2 = nullptr;
    compact3 = nullptr;

    return result;
}

bool testCreateCompactNull() {
    ICompact * compact1 = ICompact::createCompact(w, nullptr, logger),
            * compact2 = ICompact::createCompact(nullptr, x, logger),
            * compact3 = ICompact::createCompact(nullptr, nullptr, logger);
    bool result = compact1 == nullptr && compact2 == nullptr && compact3 == nullptr;

    delete compact1;
    delete compact2;
    delete compact3;

    compact1 = nullptr;
    compact2 = nullptr;
    compact3 = nullptr;

    return result;
}

bool testCreateCompactWrongDim() {
    ICompact * compact1 = ICompact::createCompact(v, w, logger),
            * compact2 = ICompact::createCompact(v, x, logger);
    bool result = compact1 == nullptr && compact2 == nullptr;

    delete compact1;
    delete compact2;

    compact1 = nullptr;
    compact2 = nullptr;

    return result;
}

bool testCreateCompactNotLess() {
    ICompact * compact = ICompact::createCompact(x, w, logger);
    bool result = compact == nullptr;

    delete compact;
    compact = nullptr;

    return result;
}

bool testGetBegin() {
    ICompact * compact = ICompact::createCompact(w, x, logger);
    IVector * begin = compact->getBegin();
    bool result = equalVectors(begin, w);

    delete compact;
    delete begin;

    compact = nullptr;
    begin = nullptr;

    return result;
}

bool testGetEnd() {
    ICompact * compact = ICompact::createCompact(w, x, logger);
    IVector * end = compact->getEnd();
    bool result = equalVectors(end, x);

    delete compact;
    delete end;

    compact = nullptr;
    end = nullptr;

    return result;
}

bool testBegin() {
    ICompact * compact = ICompact::createCompact(w, x, logger);
    ICompact::iterator * it = compact->begin(step);
    bool result = it != nullptr;

    delete compact;
    delete it;

    compact = nullptr;
    it = nullptr;

    return result;
}

bool testBeginNull() {
    ICompact * compact = ICompact::createCompact(w, x, logger);
    ICompact::iterator * it = compact->begin(nullptr);
    bool result = it == nullptr;

    delete compact;
    delete it;

    compact = nullptr;
    it = nullptr;

    return result;
}

bool testEnd() {
    ICompact * compact = ICompact::createCompact(w, x, logger);
    ICompact::iterator * it = compact->end(stepReversed);
    bool result = it != nullptr;

    delete compact;
    delete it;

    compact = nullptr;
    it = nullptr;

    return result;
}

bool testEndNull() {
    ICompact * compact = ICompact::createCompact(w, x, logger);
    ICompact::iterator * it = compact->end(nullptr);
    bool result = it == nullptr;

    delete compact;
    delete it;

    compact = nullptr;
    it = nullptr;

    return result;
}

bool testIsContains() {
    ICompact * compact = ICompact::createCompact(w, x, logger);
    double coords [] = {0., 0., 0.};
    IVector * vector = IVector::createVector(3, coords, logger);
    bool isContainsResult = false;
    RESULT_CODE isContainsResultCode = compact->isContains(vector, isContainsResult);
    bool result = isContainsResultCode == RESULT_CODE::SUCCESS && isContainsResult;

    delete compact;
    delete vector;

    compact = nullptr;
    vector = nullptr;

    return result;
}

bool testIsNotContains() {
    ICompact * compact = ICompact::createCompact(w, x, logger);
    double coords [] = {123., 456., 789.};
    IVector * vector = IVector::createVector(3, coords, logger);
    bool isContainsResult = false;
    RESULT_CODE isContainsResultCode = compact->isContains(vector, isContainsResult);
    bool result = isContainsResultCode == RESULT_CODE::SUCCESS && !isContainsResult;

    delete compact;
    delete vector;

    compact = nullptr;
    vector = nullptr;

    return result;
}

bool testIsContainsNull() {
    ICompact * compact = ICompact::createCompact(w, x, logger);
    bool isContainsResult = false;
    RESULT_CODE isContainsResultCode = compact->isContains(nullptr, isContainsResult);
    bool result = isContainsResultCode != RESULT_CODE::SUCCESS && !isContainsResult;

    delete compact;
    compact = nullptr;

    return result;
}

bool testIsContainsWrongDim() {
    ICompact * compact = ICompact::createCompact(w, x, logger);
    bool isContainsResult = false;
    RESULT_CODE isContainsResultCode = compact->isContains(v, isContainsResult);
    bool result = isContainsResultCode != RESULT_CODE::SUCCESS && !isContainsResult;

    delete compact;
    compact = nullptr;

    return result;
}

bool testIsSubSet() {
    double otherBeginCoords [] = {-2., -3., -4.};
    double otherEndCoords [] = {5., 6., 7.};
    IVector * otherBegin = IVector::createVector(3, otherBeginCoords, logger),
            * otherEnd = IVector::createVector(3, otherEndCoords, logger);
    ICompact * compact = ICompact::createCompact(w, x, logger),
            * other = ICompact::createCompact(otherBegin, otherEnd, logger);
    bool isSubSetResult = false;
    RESULT_CODE isSubSetResultCode = compact->isSubSet(other, isSubSetResult);
    bool result = isSubSetResultCode == RESULT_CODE::SUCCESS && isSubSetResult;

    delete compact;
    delete other;
    delete otherBegin;
    delete otherEnd;

    compact = nullptr;
    other = nullptr;
    otherBegin = nullptr;
    otherEnd = nullptr;

    return result;
}

bool testIsNotSubSet() {
    double otherBeginCoords [] = {-2., -3., -4.};
    double otherEndCoords [] = {5., 6., 7.};
    IVector * otherBegin = IVector::createVector(3, otherBeginCoords, logger),
            * otherEnd = IVector::createVector(3, otherEndCoords, logger);
    ICompact * compact = ICompact::createCompact(w, x, logger),
            * other = ICompact::createCompact(otherBegin, otherEnd, logger);
    bool isSubSetResult = false;
    RESULT_CODE isSubSetResultCode = compact->isSubSet(other, isSubSetResult);
    bool result = isSubSetResultCode == RESULT_CODE::SUCCESS && isSubSetResult;

    delete compact;
    delete other;
    delete otherBegin;
    delete otherEnd;

    compact = nullptr;
    other = nullptr;
    otherBegin = nullptr;
    otherEnd = nullptr;

    return result;
}

bool testIsSubSetNull() {
    ICompact * compact = ICompact::createCompact(w, x, logger);
    bool isSubSetResult = false;
    RESULT_CODE isSubSetResultCode = compact->isSubSet(nullptr, isSubSetResult);
    bool result = isSubSetResultCode != RESULT_CODE::SUCCESS && !isSubSetResult;

    delete compact;
    compact = nullptr;

    return result;
}

bool testIsSubSetWrongDim() {
    double otherBeginCoords [] = {-2., -3.};
    double otherEndCoords [] = {5., 6.};
    IVector * otherBegin = IVector::createVector(2, otherBeginCoords, logger),
            * otherEnd = IVector::createVector(2, otherEndCoords, logger);
    ICompact * compact = ICompact::createCompact(w, x, logger),
            * other = ICompact::createCompact(otherBegin, otherEnd, logger);
    bool isSubSetResult = false;
    RESULT_CODE isSubSetResultCode = compact->isSubSet(other, isSubSetResult);
    bool result = isSubSetResultCode != RESULT_CODE::SUCCESS && !isSubSetResult;

    delete compact;
    delete other;
    delete otherBegin;
    delete otherEnd;

    compact = nullptr;
    other = nullptr;
    otherBegin = nullptr;
    otherEnd = nullptr;

    return result;
}

bool testIsIntersects() {
    double otherBeginCoords [] = {-2., -3., -4.};
    double otherEndCoords [] = {5., 6., 7.};
    IVector * otherBegin = IVector::createVector(3, otherBeginCoords, logger),
            * otherEnd = IVector::createVector(3, otherEndCoords, logger);
    ICompact * compact = ICompact::createCompact(w, x, logger),
            * other = ICompact::createCompact(otherBegin, otherEnd, logger);
    bool isIntersectsResult = false;
    RESULT_CODE isIntersectsResultCode = compact->isIntersects(other, isIntersectsResult);
    bool result = isIntersectsResultCode == RESULT_CODE::SUCCESS && isIntersectsResult;

    delete compact;
    delete other;
    delete otherBegin;
    delete otherEnd;

    compact = nullptr;
    other = nullptr;
    otherBegin = nullptr;
    otherEnd = nullptr;

    return result;
}

bool testIsNotIntersects() {
    double otherBeginCoords [] = {123., 456., 789.};
    double otherEndCoords [] = {124., 457., 790.};
    IVector * otherBegin = IVector::createVector(3, otherBeginCoords, logger),
            * otherEnd = IVector::createVector(3, otherEndCoords, logger);
    ICompact * compact = ICompact::createCompact(w, x, logger),
            * other = ICompact::createCompact(otherBegin, otherEnd, logger);
    bool isIntersectsResult = false;
    RESULT_CODE isIntersectsResultCode = compact->isIntersects(other, isIntersectsResult);
    bool result = isIntersectsResultCode == RESULT_CODE::SUCCESS && !isIntersectsResult;

    delete compact;
    delete other;
    delete otherBegin;
    delete otherEnd;

    compact = nullptr;
    other = nullptr;
    otherBegin = nullptr;
    otherEnd = nullptr;

    return result;
}

bool testIsIntersectsNull() {
    ICompact * compact = ICompact::createCompact(w, x, logger);
    bool isIntersectsResult = false;
    RESULT_CODE isIntersectsResultCode = compact->isIntersects(nullptr, isIntersectsResult);
    bool result = isIntersectsResultCode != RESULT_CODE::SUCCESS && !isIntersectsResult;

    delete compact;
    compact = nullptr;

    return result;
}

bool testIsIntersectsWrongDim() {
    double otherBeginCoords [] = {-2., -3.};
    double otherEndCoords [] = {5., 6.};
    IVector * otherBegin = IVector::createVector(2, otherBeginCoords, logger),
            * otherEnd = IVector::createVector(2, otherEndCoords, logger);
    ICompact * compact = ICompact::createCompact(w, x, logger),
            * other = ICompact::createCompact(otherBegin, otherEnd, logger);
    bool isIntersectsResult = false;
    RESULT_CODE isIntersectsResultCode = compact->isIntersects(other, isIntersectsResult);
    bool result = isIntersectsResultCode != RESULT_CODE::SUCCESS && !isIntersectsResult;

    delete compact;
    delete other;
    delete otherBegin;
    delete otherEnd;

    compact = nullptr;
    other = nullptr;
    otherBegin = nullptr;
    otherEnd = nullptr;

    return result;
}

bool testGetDim() {
    ICompact * compact = ICompact::createCompact(w, x, logger);
    bool result = compact->getDim() == 3;

    delete compact;
    compact = nullptr;

    return result;
}

bool testClone() {
    ICompact * compact = ICompact::createCompact(w, x, logger),
            * cloned = compact->clone();
    bool result = cloned != nullptr;

    delete compact;
    delete cloned;

    compact = nullptr;
    cloned = nullptr;

    return result;
}



int main() {
    cout << "START\n\n";

    logger->setLogFile("log.txt");

    test("testCreateCompact", testCreateCompact);
    test("testCreateCompactNaN", testCreateCompactNaN);
    test("testCreateCompactNull", testCreateCompactNull);
    test("testCreateCompactWrongDim", testCreateCompactWrongDim);
    test("testCreateCompactNotLess", testCreateCompactNotLess);
    test("testGetBegin", testGetBegin);
    test("testGetEnd", testGetEnd);
    test("testBegin", testBegin);
    test("testBeginNull", testBeginNull);
    test("testEnd", testEnd);
    test("testEndNull", testEndNull);
    test("testIsContains", testIsContains);
    test("testIsNotContains", testIsNotContains);
    test("testIsContainsNull", testIsContainsNull);
    test("testIsContainsWrongDim", testIsContainsWrongDim);
    test("testIsSubSet", testIsSubSet);
    test("testIsNotSubSet", testIsNotSubSet);
    test("testIsSubSetNull", testIsSubSetNull);
    test("testIsSubSetWrongDim", testIsSubSetWrongDim);
    test("testIsIntersects", testIsIntersects);
    test("testIsNotIntersects", testIsNotIntersects);
    test("testIsIntersectsNull", testIsIntersectsNull);
    test("testIsIntersectsWrongDim", testIsIntersectsWrongDim);
    test("testGetDim", testGetDim);
    test("testClone", testClone);

    if(passed) {
        cout << "\nAll tests PASSED\n";
    }
    else {
        cout << "\nThere was an ERROR somewhere\n";
    }

    delete v;
    delete w;
    delete x;
    delete step;
    delete stepReversed;

    v = nullptr;
    w = nullptr;
    x = nullptr;
    step = nullptr;
    stepReversed = nullptr;

    logger->destroyLogger(nullptr);
    cout << "\nFINISH";

    return 0;
}
