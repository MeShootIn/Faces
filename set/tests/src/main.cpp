#include <iostream>
#include <limits>

#include "../include/ISet.h"

using namespace std;



/* Global vars */

ILogger * logger = ILogger::createLogger(nullptr);
IVector::NORM const NORM = IVector::NORM::NORM_2;
double const TOLERANCE = 1e-6;
double vCoords [] = {1., 2.};
double wCoords [] = {-3., -4., -5.};
double xCoords [] = {-6., -7., -8.};
IVector * v = IVector::createVector(2, vCoords, logger);
IVector * w = IVector::createVector(3, wCoords, logger);
IVector * x = IVector::createVector(3, xCoords, logger);



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

bool equalSets(ISet * s1, ISet * s2) {
    if(s1 == nullptr || s2 == nullptr || s1->getDim() != s2->getDim() || s1->getSize() != s2->getSize()) {
        return false;
    }

    for(size_t i = 0; i < s1->getSize(); ++i) {
        IVector * vector = nullptr;
        s1->get(vector, i);
        IVector * founded = nullptr;
        RESULT_CODE resultCode = s2->get(founded, vector, NORM, TOLERANCE);
        bool result = resultCode == RESULT_CODE::SUCCESS && equalVectors(vector, founded);

        delete vector;
        delete founded;

        vector = nullptr;
        founded = nullptr;

        if(!result) {
            return false;
        }
    }

    return true;
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

bool testCreateSet() {
    ISet * set = ISet::createSet(logger);
    bool result = set != nullptr;

    delete set;
    set = nullptr;

    return result;
}

bool testInsert() {
    ISet * set = ISet::createSet(logger);
    RESULT_CODE resultCode = set->insert(v, NORM, TOLERANCE);
    bool result = resultCode == RESULT_CODE::SUCCESS;

    delete set;
    set = nullptr;

    return result;
}

bool testInsertNull() {
    ISet * set = ISet::createSet(logger);
    RESULT_CODE resultCode = set->insert(nullptr, NORM, TOLERANCE);
    bool result = resultCode != RESULT_CODE::SUCCESS;

    delete set;
    set = nullptr;

    return result;
}

bool testInsertNaN() {
    ISet * set = ISet::createSet(logger);
    RESULT_CODE resultCode = set->insert(v, NORM, numeric_limits <double>::quiet_NaN());
    bool result = resultCode != RESULT_CODE::SUCCESS;

    delete set;
    set = nullptr;

    return result;
}

bool testInsertNegative() {
    ISet * set = ISet::createSet(logger);
    RESULT_CODE resultCode = set->insert(v, NORM, -TOLERANCE);
    bool result = resultCode != RESULT_CODE::SUCCESS;

    delete set;
    set = nullptr;

    return result;
}

bool testInsertWrongDim() {
    ISet * set = ISet::createSet(logger);
    set->insert(v, NORM, TOLERANCE);
    RESULT_CODE resultCode = set->insert(w, NORM, TOLERANCE);
    bool result = resultCode != RESULT_CODE::SUCCESS;

    delete set;
    set = nullptr;

    return result;
}

bool testInsertMultiple() {
    ISet * set = ISet::createSet(logger);
    set->insert(v, NORM, TOLERANCE);
    RESULT_CODE resultCode = set->insert(v, NORM, TOLERANCE);
    bool result = resultCode != RESULT_CODE::SUCCESS;

    delete set;
    set = nullptr;

    return result;
}

bool testIndexedGet() {
    ISet * set = ISet::createSet(logger);
    set->insert(v, NORM, TOLERANCE);
    IVector * vector = nullptr;
    RESULT_CODE resultCode = set->get(vector, 0);
    bool result = resultCode == RESULT_CODE::SUCCESS && vector != nullptr;

    delete set;
    delete vector;

    set = nullptr;
    vector = nullptr;

    return result;
}

bool testIndexedGetWrongIndex() {
    ISet * set = ISet::createSet(logger);
    set->insert(v, NORM, TOLERANCE);
    IVector * vector = nullptr;
    RESULT_CODE resultCode = set->get(vector, set->getSize());
    bool result = resultCode != RESULT_CODE::SUCCESS && vector == nullptr;

    delete set;
    delete vector;

    set = nullptr;
    vector = nullptr;

    return result;
}

bool testGet() {
    ISet * set = ISet::createSet(logger);
    set->insert(v, NORM, TOLERANCE);
    IVector * vector = nullptr;
    RESULT_CODE resultCode = set->get(vector, v, NORM, TOLERANCE);
    bool result = resultCode == RESULT_CODE::SUCCESS && vector != nullptr;

    delete set;
    delete vector;

    set = nullptr;
    vector = nullptr;

    return result;
}

bool testGetNull() {
    ISet * set = ISet::createSet(logger);
    set->insert(v, NORM, TOLERANCE);
    IVector * vector = nullptr;
    RESULT_CODE resultCode = set->get(vector, nullptr, NORM, TOLERANCE);
    bool result = resultCode != RESULT_CODE::SUCCESS && vector == nullptr;

    delete set;
    delete vector;

    set = nullptr;
    vector = nullptr;

    return result;
}

bool testGetNaN() {
    ISet * set = ISet::createSet(logger);
    set->insert(v, NORM, TOLERANCE);
    IVector * vector = nullptr;
    RESULT_CODE resultCode = set->get(vector, v, NORM, numeric_limits <double>::quiet_NaN());
    bool result = resultCode != RESULT_CODE::SUCCESS && vector == nullptr;

    delete set;
    delete vector;

    set = nullptr;
    vector = nullptr;

    return result;
}

bool testGetNegative() {
    ISet * set = ISet::createSet(logger);
    set->insert(v, NORM, TOLERANCE);
    IVector * vector = nullptr;
    RESULT_CODE resultCode = set->get(vector, v, NORM, -TOLERANCE);
    bool result = resultCode != RESULT_CODE::SUCCESS && vector == nullptr;

    delete set;
    delete vector;

    set = nullptr;
    vector = nullptr;

    return result;
}

bool testGetWrongDim() {
    ISet * set = ISet::createSet(logger);
    set->insert(v, NORM, TOLERANCE);
    IVector * vector = nullptr;
    RESULT_CODE resultCode = set->get(vector, w, NORM, TOLERANCE);
    bool result = resultCode != RESULT_CODE::SUCCESS && vector == nullptr;

    delete set;
    delete vector;

    set = nullptr;
    vector = nullptr;

    return result;
}

bool testGetDim() {
    ISet * set = ISet::createSet(logger);
    set->insert(v, NORM, TOLERANCE);
    bool result = set->getDim() == v->getDim();

    delete set;
    set = nullptr;

    return result;
}

bool testGetSize() {
    ISet * set = ISet::createSet(logger);
    set->insert(v, NORM, TOLERANCE);
    bool result = set->getSize() == 1;

    delete set;
    set = nullptr;

    return result;
}

bool testIndexedErase() {
    ISet * set = ISet::createSet(logger);
    set->insert(v, NORM, TOLERANCE);
    RESULT_CODE resultCode = set->erase(0);
    bool result = resultCode == RESULT_CODE::SUCCESS;

    delete set;
    set = nullptr;

    return result;
}

bool testIndexedEraseWrongIndex() {
    ISet * set = ISet::createSet(logger);
    set->insert(v, NORM, TOLERANCE);
    RESULT_CODE resultCode = set->erase(1);
    bool result = resultCode != RESULT_CODE::SUCCESS;

    delete set;
    set = nullptr;

    return result;
}

bool testErase() {
    ISet * set = ISet::createSet(logger);
    set->insert(v, NORM, TOLERANCE);
    RESULT_CODE resultCode = set->erase(v, NORM, TOLERANCE);
    bool result = resultCode == RESULT_CODE::SUCCESS;

    delete set;
    set = nullptr;

    return result;
}

bool testEraseNull() {
    ISet * set = ISet::createSet(logger);
    set->insert(v, NORM, TOLERANCE);
    RESULT_CODE resultCode = set->erase(nullptr, NORM, TOLERANCE);
    bool result = resultCode != RESULT_CODE::SUCCESS;

    delete set;
    set = nullptr;

    return result;
}

bool testEraseNaN() {
    ISet * set = ISet::createSet(logger);
    set->insert(v, NORM, TOLERANCE);
    RESULT_CODE resultCode = set->erase(v, NORM, numeric_limits <double>::quiet_NaN());
    bool result = resultCode != RESULT_CODE::SUCCESS;

    delete set;
    set = nullptr;

    return result;
}

bool testEraseNegative() {
    ISet * set = ISet::createSet(logger);
    set->insert(v, NORM, TOLERANCE);
    RESULT_CODE resultCode = set->erase(v, NORM, -TOLERANCE);
    bool result = resultCode != RESULT_CODE::SUCCESS;

    delete set;
    set = nullptr;

    return result;
}

bool testEraseWrongDim() {
    ISet * set = ISet::createSet(logger);
    set->insert(v, NORM, TOLERANCE);
    RESULT_CODE resultCode = set->erase(w, NORM, TOLERANCE);
    bool result = resultCode != RESULT_CODE::SUCCESS;

    delete set;
    set = nullptr;

    return result;
}

bool testEraseNotFounded() {
    ISet * set = ISet::createSet(logger);
    set->insert(w, NORM, TOLERANCE);
    RESULT_CODE resultCode = set->erase(x, NORM, TOLERANCE);
    bool result = resultCode != RESULT_CODE::SUCCESS;

    delete set;
    set = nullptr;

    return result;
}

bool testClone() {
    ISet * set = ISet::createSet(logger);
    ISet * cloned = set->clone();
    bool result = cloned != nullptr;

    delete set;
    delete cloned;

    set = nullptr;
    cloned = nullptr;

    return result;
}

bool testAdd() {
    ISet * s1 = ISet::createSet(logger);
    ISet * s2 = ISet::createSet(logger);
    s1->insert(w, NORM, TOLERANCE);
    s2->insert(x, NORM, TOLERANCE);
    ISet * sum = ISet::add(s1, s2, NORM, TOLERANCE, logger);
    ISet * correctSum = ISet::createSet(logger);
    correctSum->insert(w, NORM, TOLERANCE);
    correctSum->insert(x, NORM, TOLERANCE);
    bool result = equalSets(sum, correctSum);

    delete s1;
    delete s2;
    delete sum;
    delete correctSum;

    s1 = nullptr;
    s2 = nullptr;
    sum = nullptr;
    correctSum = nullptr;

    return result;
}

bool testAddWrongDim() {
    ISet * s1 = ISet::createSet(logger);
    ISet * s2 = ISet::createSet(logger);
    s1->insert(v, NORM, TOLERANCE);
    s2->insert(w, NORM, TOLERANCE);
    ISet * sum = ISet::add(s1, s2, NORM, TOLERANCE, logger);
    bool result = sum == nullptr;

    delete s1;
    delete s2;
    delete sum;

    s1 = nullptr;
    s2 = nullptr;
    sum = nullptr;

    return result;
}

bool testAddNaN() {
    ISet * s1 = ISet::createSet(logger);
    ISet * s2 = ISet::createSet(logger);
    s1->insert(w, NORM, TOLERANCE);
    s2->insert(x, NORM, TOLERANCE);
    ISet * sum = ISet::add(s1, s2, NORM, numeric_limits <double>::quiet_NaN(), logger);
    bool result = sum == nullptr;

    delete s1;
    delete s2;
    delete sum;

    s1 = nullptr;
    s2 = nullptr;
    sum = nullptr;

    return result;
}

bool testAddNegative() {
    ISet * s1 = ISet::createSet(logger);
    ISet * s2 = ISet::createSet(logger);
    s1->insert(w, NORM, TOLERANCE);
    s2->insert(x, NORM, TOLERANCE);
    ISet * sum = ISet::add(s1, s2, NORM, -TOLERANCE, logger);
    bool result = sum == nullptr;

    delete s1;
    delete s2;
    delete sum;

    s1 = nullptr;
    s2 = nullptr;
    sum = nullptr;

    return result;
}

bool testIntersect() {
    ISet * s1 = ISet::createSet(logger);
    ISet * s2 = ISet::createSet(logger);
    s1->insert(v, NORM, TOLERANCE);
    s2->insert(v, NORM, TOLERANCE);
    ISet * intersection = ISet::intersect(s1, s2, NORM, TOLERANCE, logger);
    ISet * correctIntersection = ISet::createSet(logger);
    correctIntersection->insert(v, NORM, TOLERANCE);
    bool result = equalSets(intersection, correctIntersection);

    delete s1;
    delete s2;
    delete intersection;
    delete correctIntersection;

    s1 = nullptr;
    s2 = nullptr;
    intersection = nullptr;
    correctIntersection = nullptr;

    return result;
}

bool testEmptyIntersect() {
    ISet * s1 = ISet::createSet(logger);
    ISet * s2 = ISet::createSet(logger);
    s1->insert(w, NORM, TOLERANCE);
    s2->insert(x, NORM, TOLERANCE);
    ISet * intersection = ISet::intersect(s1, s2, NORM, TOLERANCE, logger);
    ISet * correctIntersection = ISet::createSet(logger);
    bool result = equalSets(intersection, correctIntersection);

    delete s1;
    delete s2;
    delete intersection;
    delete correctIntersection;

    s1 = nullptr;
    s2 = nullptr;
    intersection = nullptr;
    correctIntersection = nullptr;

    return result;
}

bool testIntersectWrongDim() {
    ISet * s1 = ISet::createSet(logger);
    ISet * s2 = ISet::createSet(logger);
    s1->insert(v, NORM, TOLERANCE);
    s2->insert(w, NORM, TOLERANCE);
    ISet * intersection = ISet::intersect(s1, s2, NORM, TOLERANCE, logger);
    bool result = intersection == nullptr;

    delete s1;
    delete s2;
    delete intersection;

    s1 = nullptr;
    s2 = nullptr;
    intersection = nullptr;

    return result;
}

bool testIntersectNaN() {
    ISet * s1 = ISet::createSet(logger);
    ISet * s2 = ISet::createSet(logger);
    s1->insert(w, NORM, TOLERANCE);
    s2->insert(x, NORM, TOLERANCE);
    ISet * intersection = ISet::intersect(s1, s2, NORM, numeric_limits <double>::quiet_NaN(), logger);
    bool result = intersection == nullptr;

    delete s1;
    delete s2;
    delete intersection;

    s1 = nullptr;
    s2 = nullptr;
    intersection = nullptr;

    return result;
}

bool testIntersectNegative() {
    ISet * s1 = ISet::createSet(logger);
    ISet * s2 = ISet::createSet(logger);
    s1->insert(w, NORM, TOLERANCE);
    s2->insert(x, NORM, TOLERANCE);
    ISet * intersection = ISet::intersect(s1, s2, NORM, -TOLERANCE, logger);
    bool result = intersection == nullptr;

    delete s1;
    delete s2;
    delete intersection;

    s1 = nullptr;
    s2 = nullptr;
    intersection = nullptr;

    return result;
}

bool testSub() {
    ISet * s1 = ISet::createSet(logger);
    ISet * s2 = ISet::createSet(logger);
    s1->insert(w, NORM, TOLERANCE);
    s2->insert(x, NORM, TOLERANCE);
    ISet * diff = ISet::sub(s1, s2, NORM, TOLERANCE, logger);
    ISet * correctDiff = ISet::createSet(logger);
    correctDiff->insert(w, NORM, TOLERANCE);
    bool result = equalSets(diff, correctDiff);

    delete s1;
    delete s2;
    delete diff;
    delete correctDiff;

    s1 = nullptr;
    s2 = nullptr;
    diff = nullptr;
    correctDiff = nullptr;

    return result;
}

bool testEmptySub() {
    ISet * s1 = ISet::createSet(logger);
    ISet * s2 = ISet::createSet(logger);
    ISet * diff = ISet::sub(s1, s2, NORM, TOLERANCE, logger);
    ISet * correctDiff = ISet::createSet(logger);
    bool result = equalSets(diff, correctDiff);

    delete s1;
    delete s2;
    delete diff;
    delete correctDiff;

    s1 = nullptr;
    s2 = nullptr;
    diff = nullptr;
    correctDiff = nullptr;

    return result;
}

bool testSubWrongDim() {
    ISet * s1 = ISet::createSet(logger);
    ISet * s2 = ISet::createSet(logger);
    s1->insert(v, NORM, TOLERANCE);
    s2->insert(w, NORM, TOLERANCE);
    ISet * diff = ISet::sub(s1, s2, NORM, TOLERANCE, logger);
    bool result = diff == nullptr;

    delete s1;
    delete s2;
    delete diff;

    s1 = nullptr;
    s2 = nullptr;
    diff = nullptr;

    return result;
}

bool testSubNaN() {
    ISet * s1 = ISet::createSet(logger);
    ISet * s2 = ISet::createSet(logger);
    s1->insert(w, NORM, TOLERANCE);
    s2->insert(x, NORM, TOLERANCE);
    ISet * diff = ISet::sub(s1, s2, NORM, numeric_limits <double>::quiet_NaN(), logger);
    bool result = diff == nullptr;

    delete s1;
    delete s2;
    delete diff;

    s1 = nullptr;
    s2 = nullptr;
    diff = nullptr;

    return result;
}

bool testSubNegative() {
    ISet * s1 = ISet::createSet(logger);
    ISet * s2 = ISet::createSet(logger);
    s1->insert(w, NORM, TOLERANCE);
    s2->insert(x, NORM, TOLERANCE);
    ISet * diff = ISet::sub(s1, s2, NORM, -TOLERANCE, logger);
    bool result = diff == nullptr;

    delete s1;
    delete s2;
    delete diff;

    s1 = nullptr;
    s2 = nullptr;
    diff = nullptr;

    return result;
}

bool testSymSub() {
    ISet * s1 = ISet::createSet(logger);
    ISet * s2 = ISet::createSet(logger);
    s1->insert(w, NORM, TOLERANCE);
    s2->insert(x, NORM, TOLERANCE);
    s2->insert(w, NORM, TOLERANCE);
    ISet * symDiff = ISet::symSub(s1, s2, NORM, TOLERANCE, logger);
    ISet * correctSymDiff = ISet::createSet(logger);
    correctSymDiff->insert(x, NORM, TOLERANCE);
    bool result = equalSets(symDiff, correctSymDiff);

    delete s1;
    delete s2;
    delete symDiff;
    delete correctSymDiff;

    s1 = nullptr;
    s2 = nullptr;
    symDiff = nullptr;
    correctSymDiff = nullptr;

    return result;
}

bool testEmptySymSub() {
    ISet * s1 = ISet::createSet(logger);
    ISet * s2 = ISet::createSet(logger);
    s1->insert(w, NORM, TOLERANCE);
    s2->insert(w, NORM, TOLERANCE);
    ISet * symDiff = ISet::symSub(s1, s2, NORM, TOLERANCE, logger);
    ISet * correctSymDiff = ISet::createSet(logger);
    bool result = equalSets(symDiff, correctSymDiff);

    delete s1;
    delete s2;
    delete symDiff;
    delete correctSymDiff;

    s1 = nullptr;
    s2 = nullptr;
    symDiff = nullptr;
    correctSymDiff = nullptr;

    return result;
}

bool testSymSubWrongDim() {
    ISet * s1 = ISet::createSet(logger);
    ISet * s2 = ISet::createSet(logger);
    s1->insert(v, NORM, TOLERANCE);
    s2->insert(w, NORM, TOLERANCE);
    ISet * symDiff = ISet::symSub(s1, s2, NORM, TOLERANCE, logger);
    bool result = symDiff == nullptr;

    delete s1;
    delete s2;
    delete symDiff;

    s1 = nullptr;
    s2 = nullptr;
    symDiff = nullptr;

    return result;
}

bool testSymSubNaN() {
    ISet * s1 = ISet::createSet(logger);
    ISet * s2 = ISet::createSet(logger);
    s1->insert(w, NORM, TOLERANCE);
    s2->insert(x, NORM, TOLERANCE);
    ISet * symDiff = ISet::symSub(s1, s2, NORM, numeric_limits <double>::quiet_NaN(), logger);
    bool result = symDiff == nullptr;

    delete s1;
    delete s2;
    delete symDiff;

    s1 = nullptr;
    s2 = nullptr;
    symDiff = nullptr;

    return result;
}

bool testSymSubNegative() {
    ISet * s1 = ISet::createSet(logger);
    ISet * s2 = ISet::createSet(logger);
    s1->insert(w, NORM, TOLERANCE);
    s2->insert(x, NORM, TOLERANCE);
    ISet * symDiff = ISet::symSub(s1, s2, NORM, -TOLERANCE, logger);
    bool result = symDiff == nullptr;

    delete s1;
    delete s2;
    delete symDiff;

    s1 = nullptr;
    s2 = nullptr;
    symDiff = nullptr;

    return result;
}





int main() {
    cout << "START\n\n";

    logger->setLogFile("log.txt");

    test("testCreateSet", testCreateSet);
    test("testInsert", testInsert);
    test("testInsertNull", testInsertNull);
    test("testInsertNaN", testInsertNaN);
    test("testInsertNegative", testInsertNegative);
    test("testInsertWrongDim", testInsertWrongDim);
    test("testInsertMultiple", testInsertMultiple);
    test("testIndexedGet", testIndexedGet);
    test("testIndexedGetWrongIndex", testIndexedGetWrongIndex);
    test("testGet", testGet);
    test("testGetNull", testGetNull);
    test("testGetNaN", testGetNaN);
    test("testGetNegative", testGetNegative);
    test("testGetWrongDim", testGetWrongDim);
    test("testGetDim", testGetDim);
    test("testGetSize", testGetSize);
    test("testIndexedErase", testIndexedErase);
    test("testIndexedEraseWrongIndex", testIndexedEraseWrongIndex);
    test("testErase", testErase);
    test("testEraseNull", testEraseNull);
    test("testEraseNaN", testEraseNaN);
    test("testEraseNegative", testEraseNegative);
    test("testEraseWrongDim", testEraseWrongDim);
    test("testEraseNotFounded", testEraseNotFounded);
    test("testClone", testClone);
    test("testAdd", testAdd);
    test("testAddWrongDim", testAddWrongDim);
    test("testAddNaN", testAddNaN);
    test("testAddNegative", testAddNegative);
    test("testIntersect", testIntersect);
    test("testEmptyIntersect", testEmptyIntersect);
    test("testIntersectWrongDim", testIntersectWrongDim);
    test("testIntersectNaN", testIntersectNaN);
    test("testIntersectNegative", testIntersectNegative);
    test("testSub", testSub);
    test("testEmptySub", testEmptySub);
    test("testSubWrongDim", testSubWrongDim);
    test("testSubNaN", testSubNaN);
    test("testSubNegative", testSubNegative);
    test("testSymSub", testSymSub);
    test("testEmptySymSub", testEmptySymSub);
    test("testSymSubWrongDim", testSymSubWrongDim);
    test("testSymSubNaN", testSymSubNaN);
    test("testSymSubNegative", testSymSubNegative);

    if(passed) {
        cout << "\nAll tests PASSED\n";
    }
    else {
        cout << "\nThere was an ERROR somewhere\n";
    }

    delete v;
    delete w;
    delete x;

    v = nullptr;
    w = nullptr;
    x = nullptr;

    logger->destroyLogger(nullptr);
    cout << "\nFINISH";

    return 0;
}
