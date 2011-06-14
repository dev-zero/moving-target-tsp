#!/usr/bin/env python3

import re

HEADER_TEMPLATE="""
/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2011 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */

#ifndef HIP_HH
#define HIP_HH

#include <functional>
#include <string>
#include <tuple>

namespace HIP
{
struct InvalidLine :
    public std::exception
{
};

struct Parser
{
    typedef std::function<bool ($tupletypes)> appender_func;
    void operator()(const std::string& text, appender_func func);

    typedef std::tuple<$tupletypes> data;
    typedef std::function<bool (const data&)> appender_tuple_func;
    void operator()(const std::string& text, appender_tuple_func func);
};

enum fields
{
    $fields
};

const char field_descriptions[][42] = {
    $fielddescriptions
};
}
#endif // HIP_HH
"""

SOURCE_TEMPLATE="""
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
        next_eol = text.find_first_of('\\n', offset);

        // ignore completely empty lines
        if (next_eol == offset)
        {
            offset = next_eol+1;
            continue;
        }

        if (next_eol-offset < $linelength)
            throw InvalidLine();

        if (!func(
                $tuplefuncs
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
        next_eol = text.find_first_of('\\n', offset);

        // ignore completely empty lines
        if (next_eol == offset)
        {
            offset = next_eol+1;
            continue;
        }

        if (next_eol-offset < $linelength)
            throw InvalidLine();

        if (!func(std::make_tuple(
                $tuplefuncs
                )))
            return;

        // we found the last line without carriage return
        if (next_eol == std::string::npos)
            break;

        offset = next_eol+1;
    }
}

}
"""

class HipparcosSpecParser:
    def __init__(self):
        self._re_bytes = re.compile('(\d+) *(?:- *(\d+))?')
        self._re_format = re.compile('([AIF])(\d+)(?:\.(\d+))?')
        self._fields = []

    def _get_format(self, f):
        if f[0] == 'A':
            if int(f[1]) == 1:
                return 'char'
            else:
                return 'std::string'
        if f[0] == 'I':
            if int(f[1]) <= 2:
                return 'short'
            else:
                return 'int'
        if f[0] == 'F':
            return 'double'

    def load(self, file, fields_exclude=[0]):
        fields = []

        for line in file:
            if line[0] != ' ': # valid lines should start with a space char
                continue

            field = {
                    'bytes':  line[1:8],
                    'format': line[10:14],
                    'units':  line[16:21],
                    'label':  line[22:33],
                    'explanations': line[34:75],
                    'number': line[75:80] }

            field = {k:v.strip() for (k,v) in field.items()}

            m = self._re_bytes.match(field['bytes'])
            if not m: # ignore lines with an invalid byte or byte sequence in the first field
                continue

            if field['label'] == '---': # ignore fields with completely empty labels
                continue

            field['label'] = field['label'].replace('-', '_').replace(':', '_').replace('(','').replace(')','_').upper()

            field['bytes'] = m.groups()

            m = self._re_format.match(field['format'])
            field['format'] = self._get_format(m.groups())

            if field['units'] == '---':
                field['units'] = None

            field['number'] = int(field['number'][2:-1])

            if field['number'] in fields_exclude:
                continue

            field['explanations'] = field['explanations'].replace('"', '')

            fields.append(field)

        self._fields = fields


    def emit(self, headerfilename, sourcefilename):
        tupletypes = ', '.join([f['format'] for f in self._fields])
        fields = ',\n    '.join([f['label'] for f in self._fields])
        fielddescriptions = ',\n    '.join(['"%s"' % f['explanations'] for f in self._fields])

        tuplefuncs=[]
        typetoconversion={
                'int': 'atoi(%s.c_str())',
                'short': 'atoi(%s.c_str())',
                'double': 'atof(%s.c_str())',
                'char': '%s.c_str()[0]',
                'std::string': '%s' }
        for f in self._fields:
            field_length = 1
            start = int(f['bytes'][0])-1
            if f['bytes'][1] != None:
                field_length = int(f['bytes'][1])-start
            text = 'text.substr(offset + %i,  %i)' % (start, field_length)
            tuplefuncs.append( typetoconversion[f['format']] % text )

        from string import Template

        header = Template(HEADER_TEMPLATE).substitute(tupletypes=tupletypes, fields=fields, fielddescriptions=fielddescriptions)
        source = Template(SOURCE_TEMPLATE).substitute(linelength=158, tuplefuncs=',\n'.join(tuplefuncs))

        headerfile = open(headerfilename, 'w')
        sourcefile = open(sourcefilename, 'w')
        headerfile.write(header)
        sourcefile.write(source)


if __name__ == '__main__':
    from sys import argv
    p = HipparcosSpecParser()
    p.load(file=open(argv[1]))
    p.emit(headerfilename='hip.hh', sourcefilename='hip.cc')
