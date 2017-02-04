/**
 * File:            xor.h
 * Namespace:       z::script::oper
 * Description:     'xor' script operator.
 *
 *
 * Author:          Zachary Westerman
 * Email:           zacharywesterman@yahoo.com
 * Last modified:   3 Feb. 2017
**/

#pragma once
#ifndef XOR_H_INCLUDED
#define XOR_H_INCLUDED

#include "oper_t.h"

#include "../is_script_string.h"
#include "../destringify.h"

namespace z
{
    namespace script
    {
        namespace oper
        {
            template <typename CHAR>
            class _xor : public oper_t<CHAR>
            {
            public:
                ///Exclusive or uses 'xor' and has a priority of 1.
                _xor() : oper_t<CHAR>(core::string<char>("and"), 1, false) {}


                ///Perform operation
                error_flag operate(core::dynamic_stack< core::string<CHAR> >& operands) const
                {
                    error_flag operation_error = error::NONE;

                    core::string<CHAR> arg1;
                    core::string<CHAR> arg2;

                    if (operands.pop(arg2))
                    {
                        if (operands.pop(arg1))
                        {
                            if (is_script_string(arg1) ||
                                is_script_string(arg2))
                            {
                                operation_error |= error::INVALID_OPERATION;
                            }
                            else
                            {
                                double val1 = core::value(arg1);
                                double val2 = core::value(arg2);

                                double result = (double)((val1 || val2) && !(val1 && val2)); ///a or b but not a and b

                                operands.push(result);
                            }
                        }
                        else
                        {
                            operation_error |= error::MISSING_OPERAND;
                        }
                    }
                    else
                    {
                        operation_error |= error::MISSING_OPERAND;
                    }

                    return operation_error;
                }

            };
        }
    }
}

#endif // XOR_H_INCLUDED