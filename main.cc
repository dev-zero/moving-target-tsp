/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2009 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */

#include <vector>
#include <memory>
#include <functional>
#include <iostream>
#include <limits>

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>

#include <osgViewer/Viewer>
#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osg/Vec3>

#include "target.hh"
#include "sources/file_source.hh"
#include "parser/csv.hh"
#include "cli/exhaustive-search.hxx"

namespace ublas
{
    using namespace boost::numeric::ublas;
}

void target_appender(std::shared_ptr<std::vector<Target>> targets,
        const double& x, const double& y, const double& z,
        const double& v_x, const double& v_y, const double& v_z,
        const std::string& name)
{
    Target t(x, y, z, v_x, v_y, v_z, name);
    targets->push_back(t);
    std::cout << "target added" << std::endl;
    std::cout << "    name:     " << t.name << std::endl;
    std::cout << "    position: " << t.position << std::endl;
    std::cout << "    velocity: " << t.velocity << std::endl;
}

inline double calculate_time(const double& v, const ublas::vector<double>& s_position, const ublas::vector<double>& e_position, const ublas::vector<double>& e_velocity)
{
    double a(v*v - ublas::inner_prod(e_velocity, e_velocity));
    double b(-2 * ublas::inner_prod(e_position - s_position, e_velocity));
    double c(-ublas::inner_prod(e_position - s_position, e_position - s_position));

    if (std::abs(a) < std::numeric_limits<double>::epsilon())
        return -c / b;

    double t1( (-b + sqrt(b*b - 4*a*c)) / (2*a) );
    double t2( (-b - sqrt(b*b - 4*a*c)) / (2*a) );

    if (t1 < 0.0)
        return t2;
    else if(t2 < 0.0)
        return t1;
    else
        return std::min(t1, t2);
}

void usage()
{
    std::cerr << "usage: exhaustive-search [options] <filename>" << std::endl
        << "options:" << std::endl;
    options::print_usage(std::cerr);
}

int main(int argc, char* argv[])
{
    double v(5.0);
    std::string filename("");

    try
    {
        int end;
        options o(argc, argv, end);

        if (o.help())
        {
            usage ();
            return 0;
        }

        v = o.velocity();

        if (o.file().empty())
        {
            std::cerr << "you have to specify a CVS-file containing the targets" << std::endl;
            usage();
            return 1;
        }
        filename = o.file();
    }
    catch (const cli::exception& e)
    {
        std::cerr << e << std::endl;
        usage();
        return 1;
    }


    FileSource<CSVParser> file(filename.c_str());
    std::shared_ptr<std::vector<Target>> targets(new std::vector<Target>);
    file.load( std::bind(&target_appender, targets,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
                std::placeholders::_4, std::placeholders::_5, std::placeholders::_6,
                std::placeholders::_7) );

    Target start(0, 0, 0, 0, 0, 0, "start");
    const double start_time(0.0);

    // adding the start at front
    targets->insert(targets->begin(), start);

    double smallest_start_distance(std::numeric_limits<double>::infinity());

    for (std::vector<Target>::const_iterator i(targets->begin()), i_end(targets->end()); i != i_end; ++i)
    {
        for (std::vector<Target>::const_iterator j(i+1); j != i_end; ++j)
        {
            double distance(ublas::norm_1(j->position - i->position));
            std::cout << "distance between targets " << i->name << " and " << j->name << " is: " << distance << std::endl;
            std::cout << "    direct travelling time: " << calculate_time(v, i->position, j->position, j->velocity) << std::endl;

            if ( (distance > 0.0) && (distance < smallest_start_distance) )
                smallest_start_distance = distance;
        }
    }
    
    // ... and at the end such that we get a closed cycle
    targets->push_back(start);

    std::cout << std::endl;
    std::cout << "============" << std::endl;
    std::cout << "permutations" << std::endl;
    std::cout << "============" << std::endl << std::endl;

    std::vector<std::shared_ptr<std::vector<unsigned long>>> permutations;

    // the first permutation is the ascending sequence of the numbers 0..n
    permutations.push_back( std::shared_ptr<std::vector<unsigned long>>(new std::vector<unsigned long>(targets->size())) );
    for (auto i(permutations.front()->begin()),
            i_end(permutations.front()->end()),
            i_begin(permutations.front()->begin()); i != i_end; ++i)
        *i = std::distance(i_begin, i);


    std::shared_ptr<std::vector<unsigned long>> permutation(new std::vector<unsigned long>(*permutations.front()));
    // we leave the front and end elements alone since they point to our start point
    while (std::next_permutation(permutation->begin()+1, permutation->end()-1))
    {
        permutations.push_back(permutation);
        permutation.reset(new std::vector<unsigned long>(*permutation));
    }
    
    std::shared_ptr<std::vector<unsigned long>> shortest_path_permutation;
    std::shared_ptr<std::vector<double>> shortest_path_times;
    double shortest_path_total_time(std::numeric_limits<double>::infinity());

    for (auto i(permutations.begin()), i_end(permutations.end()); i != i_end; ++i)
    {
        double total_t(start_time);
        std::shared_ptr<std::vector<double>> times(new std::vector<double>);

        std::cout << targets->at((*i)->front()).name << " (" << total_t << ")";
        times->push_back(total_t);
        for (auto j((*i)->begin()), j_end((*i)->end()-1); j != j_end; ++j)
        {
            total_t += calculate_time(v,
                    targets->at(*j).position + total_t*targets->at(*j).velocity,
                        targets->at(*(j+1)).position + total_t*targets->at(*(j+1)).velocity,
                        targets->at(*(j+1)).velocity);
            std::cout << " -> " << targets->at(*(j+1)).name << " (" << total_t << ")";
            times->push_back(total_t);
        }
        std::cout << " = " << total_t << std::endl;
        if (total_t < shortest_path_total_time)
        {
            shortest_path_total_time = total_t;
            shortest_path_times = times;
            shortest_path_permutation = *i;
        }
    }

    std::cout << "shortest path was: " << targets->at(shortest_path_permutation->front()).name << " (" << shortest_path_times->at(0) << ")";
    for (auto i(shortest_path_permutation->begin()), i_end(shortest_path_permutation->end()-1), i_begin(shortest_path_permutation->begin()); i != i_end; ++i)
    {
        std::cout << " -> " << targets->at(*(i+1)).name << " (" << shortest_path_times->at(std::distance(i_begin, i)+1) << ")";
    }
    std::cout << std::endl;

    osgViewer::Viewer viewer;
    osg::ref_ptr<osg::Group> root(new osg::Group);

    osg::Vec4 color(1.0f, 0.0f, 0.0f, 1.0f); // red
    for (auto i(targets->begin()), i_end(targets->end()-1); i != i_end; ++i)
    {
        osg::ref_ptr<osg::Geode> target_geode(new osg::Geode);
        osg::ref_ptr<osg::Sphere> target_sphere(new osg::Sphere(osg::Vec3f(i->position(0), i->position(1), i->position(2)), smallest_start_distance/2.0));
        osg::ref_ptr<osg::ShapeDrawable> target_drawable(new osg::ShapeDrawable(target_sphere.get()));
        target_drawable->setColor( color );
        target_geode->addDrawable(target_drawable.get());
        root->addChild(target_geode.get());

        color[0] = 0.0f; color[2] = 1.0f; // all others should be blue
    }


    osg::ref_ptr<osg::Vec3Array> target_positions_at_hit(new osg::Vec3Array);

    for (auto i(shortest_path_permutation->begin()), i_end(shortest_path_permutation->end()-1), i_begin(shortest_path_permutation->begin()); i != i_end; ++i)
    {
        osg::ref_ptr<osg::Geode> target_geode(new osg::Geode);
        osg::Vec3 position(
            targets->at(*i).position(0) + shortest_path_times->at(std::distance(i_begin, i))*targets->at(*i).velocity(0),
            targets->at(*i).position(1) + shortest_path_times->at(std::distance(i_begin, i))*targets->at(*i).velocity(1),
            targets->at(*i).position(2) + shortest_path_times->at(std::distance(i_begin, i))*targets->at(*i).velocity(2));
        osg::ref_ptr<osg::Sphere> target_sphere(new osg::Sphere(position, smallest_start_distance/2.0));
        osg::ref_ptr<osg::ShapeDrawable> target_drawable(new osg::ShapeDrawable(target_sphere.get()));
        target_geode->addDrawable(target_drawable.get());
        root->addChild(target_geode.get());
        target_positions_at_hit->push_back(position);
    }
    osg::ref_ptr<osg::Geode> line_geode(new osg::Geode);
    osg::ref_ptr<osg::Geometry> line_geometry(new osg::Geometry);
    line_geometry->setVertexArray(target_positions_at_hit);
    osg::ref_ptr<osg::Vec4Array> colors(new osg::Vec4Array);
    colors->push_back(osg::Vec4(1.0f,1.0f,0.0f,1.0f));
    line_geometry->setColorArray(colors);
    line_geometry->setColorBinding(osg::Geometry::BIND_OVERALL);
    line_geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP, 0, target_positions_at_hit->size()));
    line_geode->addDrawable(line_geometry);
    root->addChild(line_geode.get());

    viewer.setSceneData(root.get());
    return viewer.run();

//    return 0;
}

