/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2010 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */

#ifndef FILE_HH
#define FILE_HH

#include <string>
#include <fstream>

template<typename Parser>
class FileSource
{
private:
    std::string _filename;
    Parser _parse;
public:
    FileSource(const std::string& filename) :
        _filename(filename)
    {
    }

    virtual void load(typename Parser::appender_func appender)
    {
        std::string file_data;
        std::ifstream file;
        file.exceptions(std::ios::badbit | std::ios::failbit);

        file.open(_filename.c_str());

        std::copy(
            std::istreambuf_iterator<char>(file.rdbuf()),
            std::istreambuf_iterator<char>(),
            std::back_inserter(file_data) );

        _parse(file_data, appender);
    }

    virtual void load2tuple(typename Parser::appender_tuple_func appender)
    {
        std::string file_data;
        std::ifstream file;
        file.exceptions(std::ios::badbit | std::ios::failbit);

        file.open(_filename.c_str());

        std::copy(
            std::istreambuf_iterator<char>(file.rdbuf()),
            std::istreambuf_iterator<char>(),
            std::back_inserter(file_data) );

        _parse(file_data, appender);
    }

    std::string get_identifier() const
    {
        return _filename;
    }
};

#endif // FILE_HH
