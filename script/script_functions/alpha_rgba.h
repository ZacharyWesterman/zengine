#pragma once
#ifndef ALPHA_RGBA_H_INCLUDED
#define ALPHA_RGBA_H_INCLUDED

#include "../../core/array.h"
#include "../../core/string.h"
#include "../../core/stringUtils/eval_string.h"
#include "../script_errors.h"


namespace script
{
    namespace FUNCTION
    {
        //alpha function must take 1 parameter. Gets the alpha value from a color.
        template <typename CHAR>
        error_flag alpha_rgba(const core::array< core::string<CHAR> >& params, core::string<CHAR>& output)
        {
            error_flag func_error = error::NONE;

            if (params.size() > 1)
            {
                func_error |= error::TOO_MANY_PARAMS;
            }
            else if (params.size() < 1)
            {
                func_error |= error::TOO_FEW_PARAMS;
            }
            else
            {
                double color = core::value(params.at(0));

                long alpha = ((long)color & 0x000000FF);

                output = (double)alpha;
            }

            return func_error;
        }
        //end of alpha_rgba

    }
}

#endif // ALPHA_RGBA_H_INCLUDED