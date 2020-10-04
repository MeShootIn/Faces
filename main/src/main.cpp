#include <QCoreApplication>
#include <iostream>

#include "include/ISet.h"
#include "include/ICompact.h"

using namespace std;



void printVector(IVector const * const vector) {
    for(size_t i = 0; i < vector->getDim(); ++i) {
        cout << vector->getCoord(i) << " ";
    }

    cout << "\n";
}

int main(int argc, char * argv[]) {
    cout << "Start\n\n";
    QCoreApplication a(argc, argv);

    IVector::NORM const NORM = IVector::NORM::NORM_2;
    double const TOLERANCE = 1e-6;
//    ILogger * logger = ILogger::createLogger(argv[0]);
    ILogger * logger = ILogger::createLogger(nullptr);

    logger->setLogFile("log.txt");
    logger->log("Start", RESULT_CODE::SUCCESS);

//    if(argc != 2) {
//        logger->log("There must be only 1 argument: path to .dll file!", RESULT_CODE::WRONG_ARGUMENT);
//        logger->destroyLogger(argv[0]);

//        return (int) RESULT_CODE::WRONG_ARGUMENT;
//    }
    size_t dim = 2;
    double
            * coords1 = new double[dim],
            * coords2 = new double[dim],
            * coords3 = new double[dim],
            * coords4 = new double[dim],
            * stepCoords = new double[dim],
            * dirCoords = new double[dim],
            * vCoords = new double [dim] {1., 2.},
            * wCoords = new double [dim] {-3., -4.};
    double ** coordsArray = new double * [8] {coords1, coords2, coords3, coords4, stepCoords, dirCoords, vCoords, wCoords};

    for(size_t i = 0; i < dim; ++i) {
        coords1[i] = i;
        coords2[i] = i + 2;
        coords3[i] = i + 1;
        coords4[i] = i + 3;
        stepCoords[i] = 1;
        dirCoords[i] = dim - i - 1;
    }

    IVector * vector1 = IVector::createVector(dim, coords1, logger),
            * vector2 = IVector::createVector(dim, coords2, logger),
            * vector3 = IVector::createVector(dim, coords3, logger),
            * vector4 = IVector::createVector(dim, coords4, logger),
            * stepVector = IVector::createVector(dim, stepCoords, logger),
            * dirVector = IVector::createVector(dim, dirCoords, logger),
            * v = IVector::createVector(dim, vCoords, logger),
            * w = IVector::createVector(dim, wCoords, logger),
            * add = IVector::add(v, w, logger),
            * sub = IVector::sub(v, w, logger),
            * reversedV = IVector::mul(v, -1, logger);
    double scalarMul = IVector::mul(v, w, logger);

    /* Set */
    IVector ** setArray = new IVector * [11] {v, w, add, sub, reversedV, vector1, vector2, vector3, vector4, stepVector, dirVector};
    ISet * set = ISet::createSet(logger);

    cout << "v, w, v + w, v - w, -v, (v, w):\n";

    for(size_t i = 0; i < 5; ++i) {
        set->insert(setArray[i], NORM, TOLERANCE);
        printVector(setArray[i]);
    }

    cout << scalarMul << "\nSet elements:\n";

    IVector * founded = nullptr;

    for(size_t i = 0; i < set->getSize(); ++i) {
        set->get(founded, i);

        printVector(founded);
        delete founded;
    }

    cout << "\n";

    /* Compact */
    ICompact * compact1 = ICompact::createCompact(vector1, vector2, logger);
    ICompact::iterator * it = compact1->begin(stepVector);

    it->setDirection(dirVector);

    RESULT_CODE rc = RESULT_CODE::SUCCESS;

    do {
        IVector * point = it->getPoint();

        printVector(point);
        rc = it->doStep();

        delete point;
    }
    while(rc == RESULT_CODE::SUCCESS);

    ICompact * compact2 = ICompact::createCompact(vector3, vector4, logger);
    bool isIntersects = false;

    compact2->isIntersects(compact1, isIntersects);

    cout << "\nIs intersects: " << isIntersects << "\n\n";

    ICompact * compact4 = ICompact::makeConvex(compact1, compact2, logger);

    delete it;
    it = compact4->begin(stepVector);
    it->setDirection(dirVector);

    rc = RESULT_CODE::SUCCESS;

    do {
        IVector * point = it->getPoint();

        printVector(point);
        rc = it->doStep();

        delete point;
    }
    while(rc == RESULT_CODE::SUCCESS);

    /* Deleting */
    for(size_t i = 0; i < 8; ++i) {
        delete [] coordsArray[i];
        coordsArray[i] = nullptr;
    }

    for(size_t i = 0; i < 11; ++i) {
        delete setArray[i];
        setArray[i] = nullptr;
    }

    delete [] coordsArray;
    delete [] setArray;
    delete set;

    coordsArray = nullptr;
    setArray = nullptr;
    set = nullptr;
    founded = nullptr;

//    logger->destroyLogger(argv[0]);
    logger->log("Finish", RESULT_CODE::SUCCESS);
    logger->destroyLogger(nullptr);
    cout << "\nFinish";

    return a.exec();
}
