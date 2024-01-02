#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "inputdata.h"
#include "resultdata.h"

class Processor {
public:
    Processor(InputData &data);
    ~Processor();
    ResultData processData();

private:
    InputData m_data;
    ResultData m_result;
};

#endif