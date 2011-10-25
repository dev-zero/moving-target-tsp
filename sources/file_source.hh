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

/**
 * Implements a file source which loads a file into a string for later parsing
 * using the specified parser.
 */
template<typename Parser>
class FileSource
{
private:
    std::string _filename;
    Parser _parse;
public:
    /**
     * Construct the object using the specified filename
     */
    FileSource(const std::string& filename) :
        _filename(filename)
    {
    }

    /**
     * Open the file, copy the content into a string and parse it
     * using the parser specified as template argument.
     * \param appender A kind of callback function passed along to the parser
     */
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

    /**
     * The same as load but uses a different signature for the callback
     * where the parameter is a tuple instead of single variables
     */
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

    /**
     * Return a source identifier (here it is the filename)
     */
    std::string get_identifier() const
    {
        return _filename;
    }
};

#endif // FILE_HH
