/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2011 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */

#include "hip2.hh"

#include <cstdlib>
#include <iostream>

namespace HIP2 {


void
Parser::operator()(const std::string& text, appender_func func)
{
    std::string::size_type offset(0), next_eol(0);

    while (offset < text.length())
    {
        next_eol = text.find_first_of('\n', offset);

        // ignore completely empty lines
        if (next_eol == offset)
        {
            offset = next_eol+1;
            continue;
        }

        if (next_eol-offset < 158)
            throw InvalidLine();

        if (!func(
                atoi(text.substr(offset, 6).c_str()),
                atoi(text.substr(offset + 7, 3).c_str()),
                atof(text.substr(offset + 15, 13).c_str()),
                atof(text.substr(offset + 29, 13).c_str()),
                atof(text.substr(offset + 43, 7).c_str()),
                atof(text.substr(offset + 83, 6).c_str()),
                atof(text.substr(offset + 51, 8).c_str()),
                atof(text.substr(offset + 60, 8).c_str()),
                atof(text.substr(offset + 152, 6).c_str())))
            return;

        // we found the last line without carriage return
        if (next_eol == std::string::npos)
            break;

        offset = next_eol+1;
    }
}

void
Parser::operator()(const std::string& text, appender_tuple_func func)
{
    std::string::size_type offset(0), next_eol(0);

    while (offset < text.length())
    {
        next_eol = text.find_first_of('\n', offset);

        // ignore completely empty lines
        if (next_eol == offset)
        {
            offset = next_eol+1;
            continue;
        }

        if (next_eol-offset < 158)
            throw InvalidLine();

        if (!func(std::make_tuple(
                atoi(text.substr(offset +   0,  6).c_str()),
                atoi(text.substr(offset +   7,  3).c_str()),
                atoi(text.substr(offset +   8,  1).c_str()),
                atoi(text.substr(offset +   8,  1).c_str()),
                atof(text.substr(offset +  15, 13).c_str()),
                atof(text.substr(offset +  29, 13).c_str()),
                atof(text.substr(offset +  43,  7).c_str()),
                atof(text.substr(offset +  51,  8).c_str()),
                atof(text.substr(offset +  60,  8).c_str()),
                atof(text.substr(offset +  69,  6).c_str()),
                atof(text.substr(offset +  76,  6).c_str()),
                atof(text.substr(offset +  83,  6).c_str()),
                atof(text.substr(offset + 152,  6).c_str()),
                atof(text.substr(offset + 159,  5).c_str()),
                atof(text.substr(offset + 165,  6).c_str())
                )))
            return;

        // we found the last line without carriage return
        if (next_eol == std::string::npos)
            break;

        offset = next_eol+1;
    }
}

}
