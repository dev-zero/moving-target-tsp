/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2009 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */

#include <vector>

#include "csv.hh"
   
void CSVParser::operator()(const std::string& text, appender_func func)
{
    std::string::size_type offset(0), next_eol(0), pos(0);
    std::vector<std::string> tokens(7);

    while (offset < text.length())
    {
        next_eol = text.find_first_of('\n', offset);

        // ignore empty lines
        if (next_eol-offset == 0)
        {
            offset = next_eol+1;
            continue;
        }

        try
        {
            std::string::size_type line_offset(offset);
            for (std::vector<std::string>::iterator i(tokens.begin()), i_end(tokens.end()); i != i_end; ++i)
            {
                // we hit the end in the former iteration
                if (pos == next_eol)
                    throw InvalidLine();

                pos = text.find_first_of(',', line_offset);

                // if we did not find any delimiters we may have found...
                // - the last field
                //   which means that the next delimiter is on the next line (pos > next_eol) or there is no delimiter
                //   since we are on the last line (pos = std::string::npos > next_eol)
                // - an invalid line
                //   which is going to be catched on the next iteration
                if (pos > next_eol)
                    pos = next_eol;

                *i = text.substr(line_offset, pos-line_offset);

                line_offset = pos+1;
            }
            // TBD: use strtod instead of atof to be able to catch errors
            func(atof(tokens[1].c_str()),
                    atof(tokens[2].c_str()),
                    atof(tokens[3].c_str()),
                    atof(tokens[4].c_str()),
                    atof(tokens[5].c_str()),
                    atof(tokens[6].c_str()),
                    tokens[0]);

        }
        catch (InvalidLine& e)
        {
            // TBD: logging
            offset = next_eol+1;
            continue;
        }

        offset = next_eol+1;
    }
}

