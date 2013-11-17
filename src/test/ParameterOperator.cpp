#include "test/ParameterOperator.h"

#include <stromx/runtime/DataProvider.h>
#include <stromx/runtime/EnumParameter.h>
#include <stromx/runtime/Id2DataComposite.h>
#include <stromx/runtime/Id2DataPair.h>
#include <stromx/runtime/MatrixParameter.h>
#include <stromx/runtime/NumericParameter.h>
#include <stromx/runtime/OperatorException.h>
#include <stromx/runtime/ParameterGroup.h>
#include <stromx/runtime/Primitive.h>

using namespace stromx::runtime;

const std::string ParameterOperator::TYPE("ParameterOperator");
const std::string ParameterOperator::PACKAGE("TestPackage");
const Version ParameterOperator::VERSION(1, 2, 3);

ParameterOperator::ParameterOperator()
  : OperatorKernel(TYPE, PACKAGE, VERSION, setupInitParameters())
{
}

void ParameterOperator::initialize()
{
    OperatorKernel::initialize(setupInputs(), setupOutputs(), setupParameters());
    
    m_matrixParam.resize(3, 4, stromx::runtime::Matrix::FLOAT_32);
    for (unsigned int i = 0; i < m_matrixParam.rows(); ++i)
        for (unsigned int j = 0; j < m_matrixParam.cols(); ++j)
            m_matrixParam.at<float>(i, j) = float(i + j);
        
    m_intMatrixParam.resize(1, 2, stromx::runtime::Matrix::UINT_32);
    for (unsigned int i = 0; i < m_intMatrixParam.rows(); ++i)
        for (unsigned int j = 0; j < m_intMatrixParam.cols(); ++j)
            m_intMatrixParam.at<uint32_t>(i, j) = i + j;
}

void ParameterOperator::setParameter(unsigned int id, const Data& value)
{
    try
    {
        switch(id)
        {
        case INT_PARAM:
            m_intParam = data_cast<Int32>(value);
            break;
        case INITIALIZE_PARAM:
            m_initializeParam = data_cast<UInt32>(value);
            break;
        case ENUM_PARAM:
            m_enumParam = data_cast<Enum>(value);
            break;
        case BOOL_PARAM:
            m_boolParam = data_cast<Bool>(value);
            break;
        case MATRIX_PARAM:
            m_matrixParam = data_cast<stromx::runtime::Matrix>(value);
            break;
        case INT_MATRIX_PARAM:
            m_intMatrixParam = data_cast<stromx::runtime::Matrix>(value);
            break;
        default:
            throw WrongParameterId(id, *this);
        }
    }
    catch(std::bad_cast&)
    {
        throw WrongParameterType(*parameters()[id], *this);
    }
}

const DataRef ParameterOperator::getParameter(const unsigned int id) const
{
    switch(id)
    {
    case INT_PARAM:
        return m_intParam;
    case INITIALIZE_PARAM:
        return m_initializeParam;
    case ENUM_PARAM:
        return m_enumParam;
    case BOOL_PARAM:
        return m_boolParam;
    case MATRIX_PARAM:
        return m_matrixParam;
    case INT_MATRIX_PARAM:
        return m_intMatrixParam;
    default:
        throw WrongParameterId(id, *this);
    }
}  

void ParameterOperator::execute(DataProvider& provider)
{
    Id2DataPair input1(INPUT_1);
    Id2DataPair input2(INPUT_2);
    
    provider.receiveInputData(input1 && input2);
    
    // execute...
    
    Id2DataPair output1(OUTPUT_1, input1.data());
    Id2DataPair output2(OUTPUT_2, input2.data());
    provider.sendOutputData(output1 && output2);
}
        
const std::vector<const Description*> ParameterOperator::setupInputs()
{
    std::vector<const Description*> inputs;
    Description* description = 0;
    
    description = new Description(INPUT_1, DataVariant::NONE);
    description->setTitle("Input 1");
    inputs.push_back(description);
    
    description = new Description(INPUT_2, DataVariant::NONE);
    description->setTitle("Input 2");
    inputs.push_back(description);
    
    return inputs;
}

const std::vector<const Description*> ParameterOperator::setupOutputs()
{
    std::vector<const Description*> outputs;
    Description* description = 0;
    
    description = new Description(OUTPUT_1, DataVariant::NONE);
    description->setTitle("Output 1");
    outputs.push_back(description);
    
    description = new Description(OUTPUT_2, DataVariant::NONE);
    description->setTitle("Output 2");
    outputs.push_back(description);
    
    return outputs;
}

const std::vector<const Parameter*> ParameterOperator::setupParameters()
{
    std::vector<const Parameter*> parameters;
    
    ParameterGroup* group1 = new ParameterGroup(PARAM_GROUP_1);
    group1->setTitle("Group 1");
    parameters.push_back(group1);
    
    ParameterGroup* group2 = new ParameterGroup(PARAM_GROUP_2, group1);
    group2->setTitle("Group 2");
    parameters.push_back(group2);
    
    NumericParameter<Int32>* intParam = new NumericParameter<Int32>(INT_PARAM, group1);
    intParam->setTitle("Integer parameter");
    intParam->setMax(Int32(1000));
    intParam->setMin(Int32(-1000));
    intParam->setAccessMode(Parameter::INITIALIZED_WRITE);
    parameters.push_back(intParam);
    
    EnumParameter* enumParam = new EnumParameter(ENUM_PARAM, group2);
    enumParam->setTitle("Enum parameter");
    enumParam->setAccessMode(Parameter::ACTIVATED_WRITE);
    enumParam->add(EnumDescription(Enum(0), "Choice 0"));
    enumParam->add(EnumDescription(Enum(1), "Choice 1"));
    enumParam->add(EnumDescription(Enum(2), "Choice 2"));
    parameters.push_back(enumParam);
    
    Parameter* boolParam = new Parameter(BOOL_PARAM, DataVariant::BOOL);
    boolParam->setTitle("Boolean parameter");
    boolParam->setAccessMode(Parameter::INITIALIZED_WRITE);
    parameters.push_back(boolParam);
    
    Parameter* matrixParam = new MatrixParameter(MATRIX_PARAM, DataVariant::FLOAT_32_MATRIX);
    matrixParam->setTitle("Matrix parameter");
    matrixParam->setAccessMode(Parameter::INITIALIZED_WRITE);
    parameters.push_back(matrixParam);
    
    MatrixParameter* intMatrixParam = new MatrixParameter(INT_MATRIX_PARAM, DataVariant::UINT_32_MATRIX);
    intMatrixParam->setTitle("Integer matrix parameter");
    intMatrixParam->setAccessMode(Parameter::INITIALIZED_WRITE);
    intMatrixParam->setRows(1);
    intMatrixParam->setCols(2);
    parameters.push_back(intMatrixParam);
    
    return parameters;
}

const std::vector<const Parameter*> ParameterOperator::setupInitParameters()
{
    std::vector<const Parameter*> parameters;
    
    Parameter* initializeParam = new Parameter(INITIALIZE_PARAM, DataVariant::UINT_32);
    initializeParam->setTitle("Initialize parameter");
    initializeParam->setAccessMode(Parameter::NONE_WRITE);
    parameters.push_back(initializeParam);
    
    return parameters;
}
