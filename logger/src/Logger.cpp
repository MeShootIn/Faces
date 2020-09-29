#include <cstdio>
#include <cstring>
#include <cstddef>
#include <set>

#include "../include/ILogger.h"



namespace {
char ErrorName[][30] = {
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

class Logger : public ILogger {
public:
    void destroyLogger(void * pClient) override;
    void log(char const * pMsg, RESULT_CODE err) override;
    RESULT_CODE setLogFile(char const * pLogFile) override;

    static Logger * getLogger();
    static Logger * createLogger(void * pClient);
    static void printFormatted(FILE * logStream, char const * pMsg, RESULT_CODE err);

protected:
    ~Logger() override;

private:
    Logger();
    Logger(Logger const & anotherLogger) = delete;
    Logger & operator = (Logger const & anotherLogger) = delete;

    static Logger * logger;
    FILE * logFile;
    std::set <void *> clients;
};
}



/* ILogger */

ILogger::~ILogger() = default;

ILogger * ILogger::createLogger(void * pClient) {
    return Logger::createLogger(pClient);
}



/* Logger */

Logger * Logger::logger = nullptr;

Logger::Logger() : ILogger(), logFile(nullptr) {}

Logger::~Logger() {
    if(logFile != nullptr) {
        fclose(logFile);
        logFile = nullptr;
    }
}

void Logger::destroyLogger(void * pClient) {
    if(clients.find(pClient) != clients.end()) {
        clients.erase(pClient);
    } else {
        log("Cannot find client during logger destroying", RESULT_CODE::NOT_FOUND);

        return;
    }

    if(clients.empty()) {
        delete logger;
        logger = nullptr;
    }
}

void Logger::log(char const * pMsg, RESULT_CODE err) {
    FILE * logStream = logFile;

    if(logFile == nullptr) {
        logStream = stderr;
    }

    printFormatted(logStream, pMsg, err);
}

RESULT_CODE Logger::setLogFile(char const * pLogFile) {
    if(logFile != nullptr) {
        fclose(logFile);
    }

    logFile = fopen(pLogFile, "w");

    if(logFile == nullptr) {
        return RESULT_CODE::FILE_ERROR;
    }

    return RESULT_CODE::SUCCESS;
}

Logger * Logger::getLogger() {
    return logger;
}

Logger * Logger::createLogger(void * pClient) {
    if(logger == nullptr) {
        logger = new Logger();

        if(logger == nullptr) {
            printFormatted(stderr, "Not enough memory to create a logger instance", RESULT_CODE::OUT_OF_MEMORY);
        }
    }

    if(logger != nullptr) {
        logger->clients.insert(pClient);
    }

    return logger;
}

void Logger::printFormatted(FILE * logStream, char const * pMsg, RESULT_CODE err) {
    fprintf(logStream, "%s: %s\n", ErrorName[(int) err], pMsg);
}
