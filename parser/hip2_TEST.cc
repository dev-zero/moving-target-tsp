/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2009 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */

#include <gtest/gtest.h>
#include <iostream>

#include "hip2.hh"
#include "../sources/file_source.hh"

class HIP2ParserTest :
    public ::testing::Test
{
protected:
    HIP2ParserTest()
    {
    }
    virtual ~HIP2ParserTest()
    {
    }
};

void check_appender_func(int id, int solutiontype, double ra, double decl, double plx, double plx_err, double motion_ra, double motion_decl, double colour_index,
    int check_id, int check_solutiontype, double check_ra, double check_decl, double check_plx, double check_plx_err, double check_motion_ra, double check_motion_decl, double check_colour_index, unsigned int* runs)
{
    ASSERT_EQ(id, check_id);
    ASSERT_EQ(solutiontype, check_solutiontype);
    ASSERT_DOUBLE_EQ(ra, check_ra);
    ASSERT_DOUBLE_EQ(decl, check_decl);
    ASSERT_DOUBLE_EQ(plx, check_plx);
    ASSERT_DOUBLE_EQ(plx_err, check_plx_err);
    ASSERT_DOUBLE_EQ(motion_ra, check_motion_ra);
    ASSERT_DOUBLE_EQ(motion_decl, check_motion_decl);
    ASSERT_DOUBLE_EQ(colour_index, check_colour_index);
    *runs += 1;
}

void counter_appender_func(int, int, double, double, double, double, double, double, double, unsigned int* runs)
{
    *runs += 1;
}

TEST_F(HIP2ParserTest, LoadValidLines)
{
    std::string valid_line("     1   5 0 1  0.0000159148  0.0190068680    4.55    -4.55    -1.19   1.29   0.66   1.33   1.25   0.75  90  0.91  0    0.0    0  9.2043 0.0020 0.017 0  0.482 0.025  0.550   1.19  -0.71   1.00  -0.02   0.02   1.00   0.45  -0.05   0.03   1.09  -0.41   0.09   0.08  -0.60   1.00");

    HIP2Parser parser;

    /* should we have to add more data is the idea to bind a shared_ptr<std::vector<datastruct>>,
     * where datastruct contains the values of one entry */
    unsigned int runs(0);
    parser(valid_line, std::bind(&check_appender_func,
                std::placeholders::_1,
                std::placeholders::_2,
                std::placeholders::_3,
                std::placeholders::_4,
                std::placeholders::_5,
                std::placeholders::_6,
                std::placeholders::_7,
                std::placeholders::_8,
                std::placeholders::_9,
                1, 5, 0.0000159148, 0.0190068680, 4.55, 1.33, -4.55, -1.19, 0.482, &runs));
    ASSERT_EQ(runs, 1);
}

TEST_F(HIP2ParserTest, LoadCompleteFile)
{
    unsigned int runs(0);

    FileSource<HIP2Parser> file("../../data/hip2.dat");
    
    file.load(std::bind(&counter_appender_func,
                std::placeholders::_1,
                std::placeholders::_2,
                std::placeholders::_3,
                std::placeholders::_4,
                std::placeholders::_5,
                std::placeholders::_6,
                std::placeholders::_7,
                std::placeholders::_8,
                std::placeholders::_9,
                &runs));
    ASSERT_EQ(runs, 117955);
    std::cout << "loaded " << runs << " entries" << std::endl;
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

