/******************************************************************************
* Copyright 2015-2019 Xilinx, Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
******************************************************************************/

#pragma once

#ifndef _BASECLASS_H_
#define _BASECLASS_H_

/*
-------------------------------------------------------------------------------
*********************************************************   C L A S S E S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
class BaseThing 
{
public:   
    BaseThing() : section(NULL) { }
    
    uint32_t ComputeWordChecksum( void* firstWordPtr, size_t length)
    {
        uint32_t checksum = 0;
        size_t numChecksumedWords = length / sizeof(uint32_t);
        for(size_t i=0;i<	numChecksumedWords; i++) 
        {
            checksum += ((uint32_t*)firstWordPtr)[i];
        }
        /* Invert the Checksum value */
        checksum ^= 0xFFFFFFFF; 
        return checksum;
    }

    Section* section;
};


/******************************************************************************/
template <class T>
class Override
{
public:
    Override() : overriden(false), value(0) { }
    
    Override<T>& operator=(T value0) 
    {
        overriden = true;
        value = value0;
        return *this;
    }
    
    Override<T>& operator=(Override<T>& el)
    {
        overriden = el.overriden;
        value = el.value;
        return *this;
    }
    
    void Set(T value0)
    {
        overriden = true;
        value = value0;
    }
    
    T Value() const { return value; }
    
    T ValueOrDefault(T defValue) const { return overriden?value:defValue;}
    
    bool IsSet() const { return overriden;}

private:
    bool overriden;
    T value;
};
#endif
