#include <QCoreApplication>
#include <iostream>

#include "include/ISet.h"



int main(int argc, char * argv[])
{
    std::cout << "started\n\n";
    QCoreApplication a(argc, argv);

    IVector::NORM const NORM = IVector::NORM::NORM_2;
    double const TOLERANCE = 1e-6;

    ILogger * logger = ILogger::createLogger(argv[0]);
    logger->setLogFile("log.txt");

    if(argc != 2) {
        logger->log("There must be only 1 argument: path to .dll file!", RESULT_CODE::WRONG_ARGUMENT);
        logger->destroyLogger(argv[0]);

        return (int) RESULT_CODE::WRONG_ARGUMENT + 1;
    }

    double * vCoords = new double [3] {1., 2., 3.};
    double * wCoords = new double [3] {-4., -5., -6.};
    IVector * v = IVector::createVector(3, vCoords, logger);
    IVector * w = IVector::createVector(3, wCoords, logger);
    IVector * result = IVector::add(v, w, logger);

//    delete v;
//    v = nullptr;
//    delete w;
//    w = nullptr;
//    delete result;
//    result = nullptr;

    ISet * set = ISet::createSet(logger);

    set->insert(v, NORM, TOLERANCE);
    set->insert(w, NORM, TOLERANCE);
    set->insert(result, NORM, TOLERANCE);

    IVector * founded = nullptr;
    set->get(founded, 1);

    std::cout << "founded coords:\n";
    for(size_t j = 0; j < founded->getDim(); ++j) {
        std::cout << founded->getCoord(j) << " ";
    }
    std::cout << "\n";

    delete founded;

//    for(size_t i = 0; i < set->getSize(); ++i) {
//        IVector * vector = nullptr;
//        set->get(vector, i);

//        for(size_t j = 0; j < vector->getDim(); ++j) {
//            std::cout << vector->getCoord(j) << " ";
//        }

//        std::cout << "\n";
//    }

    delete set;

    std::cout << sizeof (v) << " " << sizeof (w) << " " << sizeof (result) << "\n";

    logger->log("Hello, World!", RESULT_CODE::SUCCESS);
    logger->destroyLogger(argv[0]);
    std::cout << "\nfinished";

    return a.exec();
}
