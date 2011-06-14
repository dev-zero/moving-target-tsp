
/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2011 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */

#include "hip.hh"

#include <cstdlib>
#include <iostream>

namespace HIP {
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
                atoi(text.substr(offset + 8,  6).c_str()),
text.substr(offset + 15,  1).c_str()[0],
text.substr(offset + 17,  11),
text.substr(offset + 29,  11),
atof(text.substr(offset + 41,  5).c_str()),
atoi(text.substr(offset + 47,  1).c_str()),
text.substr(offset + 49,  1).c_str()[0],
atof(text.substr(offset + 51,  12).c_str()),
atof(text.substr(offset + 64,  12).c_str()),
text.substr(offset + 77,  1).c_str()[0],
atof(text.substr(offset + 79,  7).c_str()),
atof(text.substr(offset + 87,  8).c_str()),
atof(text.substr(offset + 96,  8).c_str()),
atof(text.substr(offset + 105,  6).c_str()),
atof(text.substr(offset + 112,  6).c_str()),
atof(text.substr(offset + 119,  6).c_str()),
atof(text.substr(offset + 126,  6).c_str()),
atof(text.substr(offset + 133,  6).c_str()),
atof(text.substr(offset + 140,  5).c_str()),
atof(text.substr(offset + 146,  5).c_str()),
atof(text.substr(offset + 152,  5).c_str()),
atof(text.substr(offset + 158,  5).c_str()),
atof(text.substr(offset + 164,  5).c_str()),
atof(text.substr(offset + 170,  5).c_str()),
atof(text.substr(offset + 176,  5).c_str()),
atof(text.substr(offset + 182,  5).c_str()),
atof(text.substr(offset + 188,  5).c_str()),
atof(text.substr(offset + 194,  5).c_str()),
atoi(text.substr(offset + 200,  3).c_str()),
atof(text.substr(offset + 204,  5).c_str()),
atof(text.substr(offset + 217,  6).c_str()),
atof(text.substr(offset + 224,  5).c_str()),
atof(text.substr(offset + 230,  6).c_str()),
atof(text.substr(offset + 237,  5).c_str()),
text.substr(offset + 243,  1).c_str()[0],
atof(text.substr(offset + 245,  6).c_str()),
atof(text.substr(offset + 252,  5).c_str()),
text.substr(offset + 258,  1).c_str()[0],
atof(text.substr(offset + 260,  4).c_str()),
atof(text.substr(offset + 265,  4).c_str()),
text.substr(offset + 270,  1).c_str()[0],
text.substr(offset + 272,  1).c_str()[0],
atof(text.substr(offset + 274,  7).c_str()),
atof(text.substr(offset + 282,  6).c_str()),
atof(text.substr(offset + 289,  5).c_str()),
atoi(text.substr(offset + 295,  3).c_str()),
text.substr(offset + 299,  1).c_str()[0],
atof(text.substr(offset + 301,  5).c_str()),
atof(text.substr(offset + 307,  5).c_str()),
atof(text.substr(offset + 313,  7).c_str()),
text.substr(offset + 321,  1).c_str()[0],
text.substr(offset + 323,  1).c_str()[0],
text.substr(offset + 325,  1).c_str()[0],
text.substr(offset + 327,  10),
text.substr(offset + 338,  1).c_str()[0],
atoi(text.substr(offset + 340,  2).c_str()),
atoi(text.substr(offset + 343,  2).c_str()),
text.substr(offset + 346,  1).c_str()[0],
text.substr(offset + 348,  1).c_str()[0],
text.substr(offset + 350,  1).c_str()[0],
text.substr(offset + 352,  2),
atoi(text.substr(offset + 355,  3).c_str()),
atof(text.substr(offset + 359,  7).c_str()),
atof(text.substr(offset + 367,  5).c_str()),
atof(text.substr(offset + 373,  5).c_str()),
atof(text.substr(offset + 379,  4).c_str()),
text.substr(offset + 384,  1).c_str()[0],
text.substr(offset + 386,  1).c_str()[0],
text.substr(offset + 388,  1).c_str()[0],
atoi(text.substr(offset + 390,  6).c_str()),
text.substr(offset + 397,  10),
text.substr(offset + 408,  10),
text.substr(offset + 419,  10),
atof(text.substr(offset + 430,  4).c_str()),
text.substr(offset + 435,  12),
text.substr(offset + 448,  1).c_str()[0]
                ))
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
                atoi(text.substr(offset + 8,  6).c_str()),
text.substr(offset + 15,  1).c_str()[0],
text.substr(offset + 17,  11),
text.substr(offset + 29,  11),
atof(text.substr(offset + 41,  5).c_str()),
atoi(text.substr(offset + 47,  1).c_str()),
text.substr(offset + 49,  1).c_str()[0],
atof(text.substr(offset + 51,  12).c_str()),
atof(text.substr(offset + 64,  12).c_str()),
text.substr(offset + 77,  1).c_str()[0],
atof(text.substr(offset + 79,  7).c_str()),
atof(text.substr(offset + 87,  8).c_str()),
atof(text.substr(offset + 96,  8).c_str()),
atof(text.substr(offset + 105,  6).c_str()),
atof(text.substr(offset + 112,  6).c_str()),
atof(text.substr(offset + 119,  6).c_str()),
atof(text.substr(offset + 126,  6).c_str()),
atof(text.substr(offset + 133,  6).c_str()),
atof(text.substr(offset + 140,  5).c_str()),
atof(text.substr(offset + 146,  5).c_str()),
atof(text.substr(offset + 152,  5).c_str()),
atof(text.substr(offset + 158,  5).c_str()),
atof(text.substr(offset + 164,  5).c_str()),
atof(text.substr(offset + 170,  5).c_str()),
atof(text.substr(offset + 176,  5).c_str()),
atof(text.substr(offset + 182,  5).c_str()),
atof(text.substr(offset + 188,  5).c_str()),
atof(text.substr(offset + 194,  5).c_str()),
atoi(text.substr(offset + 200,  3).c_str()),
atof(text.substr(offset + 204,  5).c_str()),
atof(text.substr(offset + 217,  6).c_str()),
atof(text.substr(offset + 224,  5).c_str()),
atof(text.substr(offset + 230,  6).c_str()),
atof(text.substr(offset + 237,  5).c_str()),
text.substr(offset + 243,  1).c_str()[0],
atof(text.substr(offset + 245,  6).c_str()),
atof(text.substr(offset + 252,  5).c_str()),
text.substr(offset + 258,  1).c_str()[0],
atof(text.substr(offset + 260,  4).c_str()),
atof(text.substr(offset + 265,  4).c_str()),
text.substr(offset + 270,  1).c_str()[0],
text.substr(offset + 272,  1).c_str()[0],
atof(text.substr(offset + 274,  7).c_str()),
atof(text.substr(offset + 282,  6).c_str()),
atof(text.substr(offset + 289,  5).c_str()),
atoi(text.substr(offset + 295,  3).c_str()),
text.substr(offset + 299,  1).c_str()[0],
atof(text.substr(offset + 301,  5).c_str()),
atof(text.substr(offset + 307,  5).c_str()),
atof(text.substr(offset + 313,  7).c_str()),
text.substr(offset + 321,  1).c_str()[0],
text.substr(offset + 323,  1).c_str()[0],
text.substr(offset + 325,  1).c_str()[0],
text.substr(offset + 327,  10),
text.substr(offset + 338,  1).c_str()[0],
atoi(text.substr(offset + 340,  2).c_str()),
atoi(text.substr(offset + 343,  2).c_str()),
text.substr(offset + 346,  1).c_str()[0],
text.substr(offset + 348,  1).c_str()[0],
text.substr(offset + 350,  1).c_str()[0],
text.substr(offset + 352,  2),
atoi(text.substr(offset + 355,  3).c_str()),
atof(text.substr(offset + 359,  7).c_str()),
atof(text.substr(offset + 367,  5).c_str()),
atof(text.substr(offset + 373,  5).c_str()),
atof(text.substr(offset + 379,  4).c_str()),
text.substr(offset + 384,  1).c_str()[0],
text.substr(offset + 386,  1).c_str()[0],
text.substr(offset + 388,  1).c_str()[0],
atoi(text.substr(offset + 390,  6).c_str()),
text.substr(offset + 397,  10),
text.substr(offset + 408,  10),
text.substr(offset + 419,  10),
atof(text.substr(offset + 430,  4).c_str()),
text.substr(offset + 435,  12),
text.substr(offset + 448,  1).c_str()[0]
                )))
            return;

        // we found the last line without carriage return
        if (next_eol == std::string::npos)
            break;

        offset = next_eol+1;
    }
}

}
