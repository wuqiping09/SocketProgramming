#include "processor.h"
#include <iostream>
#include <string.h>

Processor::Processor(InputData &data): m_data(data) {

}

Processor::~Processor() {

}

ResultData Processor::processData() {
    strcpy(m_result.s, m_data.s);
    m_result.len = m_data.len;
    return m_result;
}