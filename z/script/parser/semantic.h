/**
 * File:            lexer.h
 * Namespace:       z::script
 * Description:     Semantic analyser template. Checks
 *                  for invalid semantics in the given
 *                  syntax tree. Additionally, indexes
 *                  variables, functions, types, and
 *                  commands for easy reference later on.
 *
 *
 * Author:          Zachary Westerman
 * Email:           zacharywesterman@yahoo.com
 * Last modified:   20 Aug. 2017
**/

#pragma once
#ifndef SEMANTIC_H_INCLUDED
#define SEMANTIC_H_INCLUDED

#include <z/core/dynamicStack.h>
#include <z/core/timeout.h>
#include "../errors.h"

#include "phrase.h"

#include "../command_t.h"
#include "../function_t.h"

#ifndef NULL
    #define NULL 0
#endif // NULL

namespace z
{
    namespace script
    {
        template <typename CHAR>
        class semantic
        {
        private:
            const core::array< command_t<CHAR>* >* commands;
            const core::array< function_t<CHAR>* >* functions;

            phrase_t<CHAR>* root;

            int index;
            core::dynamicStack<int> index_stack;

            bool is_done;

            void enter_node(int);
            void exit_node();


        public:
            core::array< parser_error<CHAR> > error_buffer;

            semantic(const core::array< command_t<CHAR>* >& _commands,
                     const core::array< function_t<CHAR>* >& _functions)
            {
                index = 0;
                root = NULL;

                is_done = true;

                commands = &_commands;
                functions = &_functions;
            };

            ~semantic(){};


            void setInput(phrase_t<CHAR>* new_root)
            {
                index_stack.dump();

                index = 0;
                root = new_root;

                is_done = (root == NULL);
            }

            inline bool error() {return (error_buffer.size() > 0);}

            inline bool done() {return is_done;}


            bool analyse(const core::timeout&);
        };



        template <typename CHAR>
        void semantic<CHAR>::enter_node(int entry)
        {
            root = root->children[entry];
            index_stack.push(entry+1);

            index = 0;
        }

        template <typename CHAR>
        void semantic<CHAR>::exit_node()
        {
            if (!index_stack.pop(index))
                is_done = true;
            else
                root = root->parent;
        }



        ///Main semantic analysis function.
        //Returns true if finished, false otherwise.
        template <typename CHAR>
        bool semantic<CHAR>::analyse(const core::timeout& time)
        {
            return true;
        }
    }
}

#endif // SEMANTIC_H_INCLUDED
