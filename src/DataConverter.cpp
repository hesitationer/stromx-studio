/* 
*  Copyright 2012 Thomas Fidler
*
*  This file is part of stromx-studio.
*
*  Stromx-studio is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  Stromx-studio is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with stromx-studio.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "DataConverter.h"

#include <QApplication>
#include <QVariant>
#include <QString>
#include <QStringList>

#include <stromx/runtime/DataRef.h>
#include <stromx/runtime/Image.h>
#include <stromx/runtime/Parameter.h>
#include <stromx/runtime/String.h>
#include <stromx/runtime/TriggerData.h>
#include <stromx/runtime/Variant.h>

#include "Common.h"
#include "Image.h"
#include "Matrix.h"

namespace
{
    template<class T>
    bool isEqual(const stromx::runtime::Data & lhs, const stromx::runtime::Data & rhs)
    {
        try
        {
            return stromx::runtime::data_cast<T>(lhs) == stromx::runtime::data_cast<T>(rhs);
        }
        catch(stromx::runtime::BadCast&)
        {
            return false;
        }
    }
}

QVariant DataConverter::toQVariant(const stromx::runtime::Data& data, const stromx::runtime::Parameter& param, int role)
{
    try
    {
        QString dataString;
        
        if (data.isVariant(stromx::runtime::Variant::NONE))
        {
            if(role == Qt::DisplayRole)
                return QString("None");
        }
        
        if (data.isVariant(stromx::runtime::Variant::TRIGGER))
        {
            if(role == Qt::DisplayRole)
                return QString("Trigger");
            
            if(role == TriggerRole)
                return QString("Trigger");
        }
        
        if (data.isVariant(stromx::runtime::Variant::BOOL))
        {
            const stromx::runtime::Bool & boolData = stromx::runtime::data_cast<stromx::runtime::Bool>(data);
                
            if(role == Qt::DisplayRole)
            {
                return bool(boolData) ? QApplication::tr("True") : QApplication::tr("False");
            }
            else if(role == Qt::EditRole)
            {
                return boolData ? 1 : 0;
            } 
            else if(role == ChoicesRole)
            {
                QStringList choices;
                
                choices.append(QApplication::tr("False"));
                choices.append(QApplication::tr("True"));
                
                return choices;
            }
        }
        
        if (data.isVariant(stromx::runtime::Variant::ENUM))
        {
            const stromx::runtime::Enum& value = stromx::runtime::data_cast<stromx::runtime::Enum>(data);
            unsigned int intValue = (unsigned int)(value);
            const std::vector<stromx::runtime::EnumDescription> & vectorEnumDesc = param.descriptions();
            
            if(role == Qt::DisplayRole)
            {
                for(std::vector<stromx::runtime::EnumDescription>::const_iterator iter_enumDesc = vectorEnumDesc.begin();
                    iter_enumDesc != vectorEnumDesc.end();
                    ++iter_enumDesc)
                {
                    if (intValue == iter_enumDesc->value())
                        return fromStromxTitle(iter_enumDesc->title());
                }
                return QString(QObject::tr("<Unknown ENUM>"));
            }
            else if(role == Qt::EditRole)
            {
                int choice = 0;
                for(std::vector<stromx::runtime::EnumDescription>::const_iterator iter_enumDesc = vectorEnumDesc.begin();
                    iter_enumDesc != vectorEnumDesc.end();
                    ++iter_enumDesc)
                {
                    if (intValue == iter_enumDesc->value())
                        return choice;
                    ++choice;
                }
                return -1;
            } 
            else if(role == ChoicesRole)
            {
                QStringList choices;
                for(std::vector<stromx::runtime::EnumDescription>::const_iterator iter_enumDesc = vectorEnumDesc.begin();
                    iter_enumDesc != vectorEnumDesc.end();
                    ++iter_enumDesc)
                {
                    choices.append(fromStromxTitle(iter_enumDesc->title()));
                }
                return choices;
            }
        }
        
        if (data.isVariant(stromx::runtime::Variant::INT_8))
        {
            if(role == Qt::DisplayRole || role == Qt::EditRole)
            {
                const stromx::runtime::Int8 & int8Data = stromx::runtime::data_cast<stromx::runtime::Int8>(data);
                return int(int8Data);
            }
        }
        
        if (data.isVariant(stromx::runtime::Variant::UINT_8))
        {
            if(role == Qt::DisplayRole || role == Qt::EditRole)
            {
                const stromx::runtime::UInt8 & uint8Data = stromx::runtime::data_cast<stromx::runtime::UInt8>(data);
                return uint(uint8Data);
            }
        }
        
        if (data.isVariant(stromx::runtime::Variant::INT_16))
        {
            if(role == Qt::DisplayRole || role == Qt::EditRole)
            {
                const stromx::runtime::Int16 & int16Data = stromx::runtime::data_cast<stromx::runtime::Int16>(data);
                return int(int16Data);
            }
        }
        
        if (data.isVariant(stromx::runtime::Variant::UINT_16))
        {
            if(role == Qt::DisplayRole || role == Qt::EditRole)
            {
                const stromx::runtime::UInt16 & uint16Data = stromx::runtime::data_cast<stromx::runtime::UInt16>(data);
                return uint(uint16Data);
            }
        }
        
        if (data.isVariant(stromx::runtime::Variant::INT_32))
        {
            if(role == Qt::DisplayRole || role == Qt::EditRole)
            {
                const stromx::runtime::Int32 & int32Data = stromx::runtime::data_cast<stromx::runtime::Int32>(data);
                return int(int32Data);
            }
        }
        
        if (data.isVariant(stromx::runtime::Variant::UINT_32))
        {
            if(role == Qt::DisplayRole || role == Qt::EditRole)
            {
                const stromx::runtime::UInt32 & uint32Data = stromx::runtime::data_cast<stromx::runtime::UInt32>(data);
                return uint(uint32Data);
            }
        }
        
        if (data.isVariant(stromx::runtime::Variant::FLOAT_32))
        {
            if(role == Qt::DisplayRole || role == Qt::EditRole)
            {
                const stromx::runtime::Float32 & floatData = stromx::runtime::data_cast<stromx::runtime::Float32>(data);
                return double(floatData);
            }
        }
        
        if (data.isVariant(stromx::runtime::Variant::FLOAT_64))
        {
            if(role == Qt::DisplayRole || role == Qt::EditRole)
            {
                const stromx::runtime::Float64 & doubleData = stromx::runtime::data_cast<stromx::runtime::Float64>(data);
                return double(doubleData);
            }
        }
        
        if (data.isVariant(stromx::runtime::Variant::STRING))
        {
            if(role == Qt::DisplayRole || role == Qt::EditRole)
            {
                const stromx::runtime::String & stringData = stromx::runtime::data_cast<stromx::runtime::String>(data);
                return QString::fromStdString(std::string(stringData));
            }
        }
        
        if (data.isVariant(stromx::runtime::Variant::IMAGE))
        {
            if(role == Qt::DisplayRole || role == ImageRole)
            {
                const stromx::runtime::Image & imageData = stromx::runtime::data_cast<stromx::runtime::Image>(data);
                return QString(QObject::tr("Width: %1 | Height: %2"))
                    .arg(imageData.width()).arg(imageData.height());
            }
        }
        
        if (data.isVariant(stromx::runtime::Variant::MATRIX))
        {
            if(role == Qt::DisplayRole)
            {
                const stromx::runtime::Matrix & matrixData = stromx::runtime::data_cast<stromx::runtime::Matrix>(data);
                return QString(QObject::tr("Rows: %1 | Columns: %2"))
                    .arg(matrixData.rows()).arg(matrixData.cols());
            }
            
            if(role == MatrixRole)
            {
                const stromx::runtime::Matrix & matrixData = stromx::runtime::data_cast<stromx::runtime::Matrix>(data);
                const Matrix matrix(matrixData);
                QVariant variantData;
                variantData.setValue<Matrix>(matrix);
                return variantData;
            }
        }
        
        return QVariant();
    }
    catch(stromx::runtime::BadCast&)
    {
        return QVariant();
    }  
}

stromx::runtime::DataRef DataConverter::toStromxData(const QVariant& variant, const stromx::runtime::Parameter& param)
{
    if(param.variant().isVariant(stromx::runtime::Variant::TRIGGER))
    {
        if(variant.type() == QVariant::Bool && variant.toBool())
            return stromx::runtime::DataRef(new stromx::runtime::TriggerData());
    }
    
    if(param.variant().isVariant(stromx::runtime::Variant::ENUM))
    {
        if(variant.type() == QVariant::Int)
        {  
            // find the description whose value equals the input data
            int index = variant.toInt();
            if((unsigned int)(index) < param.descriptions().size())
            {    
                unsigned int value = param.descriptions()[index].value();
                return stromx::runtime::DataRef(new stromx::runtime::Enum(value));
            }
        }
    }
    
    if(param.variant().isVariant(stromx::runtime::Variant::BOOL))
    {
        if(variant.type() == QVariant::Int)
            return stromx::runtime::DataRef(new stromx::runtime::Bool(variant.toInt()));
    }
    
    if(param.variant().isVariant(stromx::runtime::Variant::INT_8))
    {
        if(variant.type() == QVariant::Int)
            return stromx::runtime::DataRef(new stromx::runtime::Int8(variant.toInt()));
    }
    
    if(param.variant().isVariant(stromx::runtime::Variant::UINT_8))
    {
        if(variant.type() == QVariant::Int)
            return stromx::runtime::DataRef(new stromx::runtime::UInt8(variant.toInt()));
    }
    
    if(param.variant().isVariant(stromx::runtime::Variant::INT_16))
    {
        if(variant.type() == QVariant::Int)
            return stromx::runtime::DataRef(new stromx::runtime::Int16(variant.toInt()));
    }
    
    if(param.variant().isVariant(stromx::runtime::Variant::UINT_16))
    {
        if(variant.type() == QVariant::Int)
            return stromx::runtime::DataRef(new stromx::runtime::UInt16(variant.toInt()));
    }
    
    if(param.variant().isVariant(stromx::runtime::Variant::INT_32))
    {
        if(variant.type() == QVariant::Int)
            return stromx::runtime::DataRef(new stromx::runtime::Int32(variant.toInt()));
    }
    
    if(param.variant().isVariant(stromx::runtime::Variant::UINT_32))
    {
        if(variant.type() == QVariant::Int)
            return stromx::runtime::DataRef(new stromx::runtime::UInt32(variant.toInt()));
    }
    
    if(param.variant().isVariant(stromx::runtime::Variant::FLOAT_32))
    {
        if(variant.type() == QVariant::Double)
            return stromx::runtime::DataRef(new stromx::runtime::Float32(variant.toFloat()));
    }
    
    if(param.variant().isVariant(stromx::runtime::Variant::FLOAT_64))
    {
        if(variant.type() == QVariant::Double)
            return stromx::runtime::DataRef(new stromx::runtime::Float64(variant.toDouble()));
    }
    
    if(param.variant().isVariant(stromx::runtime::Variant::STRING))
    {
        if(variant.type() == QVariant::String)
            return stromx::runtime::DataRef(new stromx::runtime::String(variant.toString().toStdString()));
    }
    
    if(param.variant().isVariant(stromx::runtime::Variant::IMAGE))
    {
        if(variant.type() == QVariant::Image)
            return stromx::runtime::DataRef(new Image(variant.value<QImage>()));
    }
    
    if(param.variant().isVariant(stromx::runtime::Variant::MATRIX))
    {
        if(variant.canConvert<Matrix>())
            return stromx::runtime::DataRef(new Matrix(variant.value<Matrix>()));
    }
    
    return stromx::runtime::DataRef();
}

bool DataConverter::stromxDataEqualsTarget(const stromx::runtime::Data& newValue, const stromx::runtime::Data& targetValue)
{
    if(! newValue.isVariant(targetValue.variant()))
        return false;
    
    if(targetValue.isVariant(stromx::runtime::Variant::BOOL))
    {
        return isEqual<stromx::runtime::Bool>(newValue, targetValue);
    }
    
    if(targetValue.isVariant(stromx::runtime::Variant::INT_8))
    {
        return isEqual<stromx::runtime::Int8>(newValue, targetValue);
    }
    
    if(targetValue.isVariant(stromx::runtime::Variant::UINT_8))
    {
        return isEqual<stromx::runtime::UInt8>(newValue, targetValue);
    }
    
    if(targetValue.isVariant(stromx::runtime::Variant::INT_16))
    {
        return isEqual<stromx::runtime::Int16>(newValue, targetValue);
    }
    
    if(targetValue.isVariant(stromx::runtime::Variant::UINT_16))
    {
        return isEqual<stromx::runtime::UInt16>(newValue, targetValue);
    }
    
    if(targetValue.isVariant(stromx::runtime::Variant::INT_32))
    {
        return isEqual<stromx::runtime::Int32>(newValue, targetValue);
    }
    
    if(targetValue.isVariant(stromx::runtime::Variant::UINT_32))
    {
        return isEqual<stromx::runtime::UInt32>(newValue, targetValue);
    }
    
    if(targetValue.isVariant(stromx::runtime::Variant::ENUM))
    {
        return isEqual<stromx::runtime::Enum>(newValue, targetValue);
    }
    
    if(targetValue.isVariant(stromx::runtime::Variant::FLOAT_32))
    {
        return isEqual<stromx::runtime::Float32>(newValue, targetValue);
    }
    
    if(targetValue.isVariant(stromx::runtime::Variant::FLOAT_64))
    {
        return isEqual<stromx::runtime::Float64>(newValue, targetValue);
    }
    
    if(targetValue.isVariant(stromx::runtime::Variant::STRING))
    {
        return isEqual<stromx::runtime::String>(newValue, targetValue);
    }
    
    if(targetValue.isVariant(stromx::runtime::Variant::MATRIX))
    {
        return isEqual<stromx::runtime::Matrix>(newValue, targetValue);
    }
    
    return false;
}


