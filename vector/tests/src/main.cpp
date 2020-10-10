#include <iostream>
#include <cmath>
#include <limits>

#include "../include/IVector.h"

using namespace std;



/* Global vars */

ILogger * logger = ILogger::createLogger(nullptr);
IVector::NORM const NORM = IVector::NORM::NORM_2;
size_t const DIM = 2;
double const TOLERANCE = 1e-6;
double vCoords [] = {1., 2.};
double wCoords [] = {-3., -4.};
IVector * v = IVector::createVector(DIM, vCoords, logger);
IVector * w = IVector::createVector(DIM, wCoords, logger);


/* Test function */

bool passed = true;

void test(char const * testFunctionName, bool (* testFunction)()) {
    bool res = testFunction();
    passed &= res;

    cout << testFunctionName << ": " << (res ? "OK" : "WRONG ANSWER") << "\n";
}



/* Secondary functions */

bool numbersEqual(double a, double b) {
    if(isnan(a) || isnan(b)) {
        return false;
    }

    return fabs(a - b) <= TOLERANCE;
}

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

bool testCreateVectorCoordsNaN() {
    double coords [] = {numeric_limits <double>::quiet_NaN()};
    IVector * vector = IVector::createVector(1, coords, logger);
    bool result = vector == nullptr;

    delete vector;
    vector = nullptr;

    return result;
}

bool testCreateVectorCoordsNull() {
    IVector * vector = IVector::createVector(DIM, nullptr, logger);
    bool result = vector == nullptr;

    delete vector;
    vector = nullptr;

    return result;
}

bool testCreateVector() {
    IVector * vector = IVector::createVector(DIM, vCoords, logger);
    bool result = vector != nullptr;

    delete vector;
    vector = nullptr;

    return result;
}

bool testAdd() {
    IVector * sum = IVector::add(v, w, logger);
    double correctSumCoords [] = {-2., -2.};
    IVector * correctSum = IVector::createVector(DIM, correctSumCoords, logger);
    bool result = equalVectors(sum, correctSum);

    delete correctSum;
    delete sum;

    correctSum = nullptr;
    sum = nullptr;

    return result;
}

bool testSub() {
    IVector * diff = IVector::sub(v, w, logger);
    double correctDiffCoords [] = {4., 6.};
    IVector * correctDiff = IVector::createVector(DIM, correctDiffCoords, logger);
    bool result = equalVectors(diff, correctDiff);

    delete correctDiff;
    delete diff;

    correctDiff = nullptr;
    diff = nullptr;

    return result;
}

bool testMulScalar() {
    IVector * mul = IVector::mul(v, -1, logger);
    double correctMulCoords [] = {-1., -2.};
    IVector * correctMul = IVector::createVector(DIM, correctMulCoords, logger);
    bool result = equalVectors(mul, correctMul);

    delete mul;
    delete correctMul;

    mul = nullptr;
    correctMul = nullptr;

    return result;
}

bool testMulScalarNan() {
    IVector * mul = IVector::mul(v, numeric_limits <double>::quiet_NaN(), logger);
    bool result = mul == nullptr;

    delete mul;
    mul = nullptr;

    return result;
}

bool testMul() {
    double mul = IVector::mul(v, w, logger);
    double correctMul = -11.;

    return numbersEqual(mul, correctMul);
}

bool testEquals() {
    double coords1 [] = {1., 2.};
    double coords2 [] = {1., 2.};
    IVector * v1 = IVector::createVector(DIM, coords1, logger);
    IVector * v2 = IVector::createVector(DIM, coords2, logger);
    bool result = equalVectors(v1, v2);

    delete v1;
    delete v2;

    v1 = nullptr;
    v2 = nullptr;

    return result;
}

bool testNotEquals() {
    return !equalVectors(v, w);
}

bool testClone() {
    IVector * cloned = v->clone();
    bool result = cloned != nullptr;

    delete cloned;
    cloned = nullptr;

    return result;
}

bool testGetCoord() {
    double coord = v->getCoord(0);
    double correctCoord = 1.;

    return numbersEqual(coord, correctCoord);
}

bool testGetCoordIndex() {
    return isnan(v->getCoord(DIM));
}

bool testCreateVectorDim() {
    IVector * vector = IVector::createVector(0, vCoords, logger);
    bool result = vector == nullptr;

    delete vector;
    vector = nullptr;

    return result;
}

bool testSetCoord() {
    IVector * vector = v->clone();
    RESULT_CODE resultCode = vector->setCoord(0, 0.);

    delete vector;
    vector = nullptr;

    return resultCode == RESULT_CODE::SUCCESS;
}

bool testSetCoordIndex() {
    IVector * vector = v->clone();
    RESULT_CODE resultCode = vector->setCoord(DIM, 0.);

    delete vector;
    vector = nullptr;

    return resultCode != RESULT_CODE::SUCCESS;
}

bool testSetCoordNaN() {
    IVector * vector = v->clone();
    RESULT_CODE resultCode = vector->setCoord(0, numeric_limits <double>::quiet_NaN());

    delete vector;
    vector = nullptr;

    return resultCode != RESULT_CODE::SUCCESS;
}

bool testNorm() {
    double norm1 = w->norm(IVector::NORM::NORM_1);
    double correctNorm1 = 7.;

    double norm2 = w->norm(IVector::NORM::NORM_2);
    double correctNorm2 = 5.;

    double normInf = w->norm(IVector::NORM::NORM_INF);
    double correctNormInf = 4.;

    return numbersEqual(norm1, correctNorm1) && numbersEqual(norm2, correctNorm2)
            && numbersEqual(normInf, correctNormInf);
}

bool testGetDim() {
    size_t dim = v->getDim();
    double correctDim = 2;

    return dim == correctDim;
}



int main() {
    cout << "START\n\n";

    logger->setLogFile("log.txt");

    test("testCreateVectorCoordsNaN", testCreateVectorCoordsNaN);
    test("testCreateVectorCoordsNull", testCreateVectorCoordsNull);
    test("testCreateVector", testCreateVector);
    test("testAdd", testAdd);
    test("testSub", testSub);
    test("testMulScalar", testMulScalar);
    test("testMulScalarNan", testMulScalarNan);
    test("testMul", testMul);
    test("testEquals", testEquals);
    test("testNotEquals", testNotEquals);
    test("testClone", testClone);
    test("testGetCoord", testGetCoord);
    test("testGetCoordIndex", testGetCoordIndex);
    test("testCreateVectorDim", testCreateVectorDim);
    test("testSetCoord", testSetCoord);
    test("testSetCoordIndex", testSetCoordIndex);
    test("testSetCoordNaN", testSetCoordNaN);
    test("testNorm", testNorm);
    test("testGetDim", testGetDim);

    if(passed) {
        cout << "\nAll tests PASSED\n";
    }
    else {
        cout << "\nThere was an ERROR somewhere\n";
    }

    delete v;
    delete w;

    v = nullptr;
    w = nullptr;

    logger->destroyLogger(nullptr);
    cout << "\nFINISH";

    return 0;
}
