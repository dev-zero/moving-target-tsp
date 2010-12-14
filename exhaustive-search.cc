/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2010 Tiziano Müller <tm@dev-zero.ch>
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
#include "star.hh"

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


/* calculate the time duration to go from s_position to e_position which moves with e_velocity when travelling with velocity v */
inline double calculate_time(const double& v, const ublas::vector<double>& s_position, const ublas::vector<double>& e_position, const ublas::vector<double>& e_velocity)
{
    double a(v*v - ublas::inner_prod(e_velocity, e_velocity));
    double b(-2 * ublas::inner_prod(e_position - s_position, e_velocity));
    double c(-ublas::inner_prod(e_position - s_position, e_position - s_position));

    if (std::abs(a) < std::numeric_limits<double>::epsilon())
        return -c / b;
    // TODO: if b^2 =~ 4ac .. use different formula
    double t1( (-b + sqrt(b*b - 4*a*c)) / (2*a) );
    double t2( (-b - sqrt(b*b - 4*a*c)) / (2*a) );

    if (t1 < 0.0)
    {
        if (t2 < 0.0)
            return std::numeric_limits<double>::infinity();
        return t2;
    }
    else if(t2 < 0.0)
        return t1;
    else
        return std::min(t1, t2);
}

/* creates the required structures to draw a target using OSG */
inline osg::ref_ptr<osg::Geode> create_target(const osg::Vec3f& position, const double& radius, const osg::Vec4& color = osg::Vec4(1, 1, 0.5, 0.1))
{
    osg::ref_ptr<osg::Geode> target_geode(new osg::Geode);
    osg::ref_ptr<osg::Sphere> target_sphere(new osg::Sphere(position, radius));
    osg::ref_ptr<osg::ShapeDrawable> target_drawable(new osg::ShapeDrawable(target_sphere.get()));
    target_drawable->setColor( color );
    target_geode->addDrawable(target_drawable.get());
    return target_geode;
}

/* creates the tour objects to visualize it using OSG */
inline osg::ref_ptr<osg::Geode> create_tour(osg::ref_ptr<osg::Vec3Array> target_positions_at_hit)
{
    osg::ref_ptr<osg::Geode> line_geode(new osg::Geode);
    osg::ref_ptr<osg::Geometry> line_geometry(new osg::Geometry);
    line_geometry->setVertexArray(target_positions_at_hit);

    osg::ref_ptr<osg::Vec4Array> colors(new osg::Vec4Array);
    colors->push_back(osg::Vec4(1.0f,1.0f,0.0f,1.0f));
    line_geometry->setColorArray(colors);
    line_geometry->setColorBinding(osg::Geometry::BIND_OVERALL);

    line_geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP, 0, target_positions_at_hit->size()));
    line_geode->addDrawable(line_geometry);
    return line_geode;
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
            std::cerr << "you have to specify a CSV-file containing the targets" << std::endl;
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

    /* load the targets from the file */
    FileSource<CSVParser> file(filename.c_str()); // create a file source using a CSVParser
    std::shared_ptr<std::vector<Target>> targets(new std::vector<Target>); // container holding our targets
    file.load( std::bind(&target_appender, targets,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
                std::placeholders::_4, std::placeholders::_5, std::placeholders::_6,
                std::placeholders::_7) ); // bind the target-container-pointer to our appender-function and passing that to the source

    Target start(0, 0, 0, 0, 0, 0, "start"); // create a fake start-target (currently hardcoded at 0/0/0 with velocity 0/0/0)
    const double start_time(0.0); // initialize our start-time (could be hardocded but we may want to start at a different time than "now")

    // insert the start at front
    targets->insert(targets->begin(), start);

    double smallest_start_distance(std::numeric_limits<double>::infinity()); // the smallest distance between two targets at t=0, mostly needed to calculated an appropriate radius for the 3D-objects

    /* calculate the distance at t=0 between all targets */
    for (std::vector<Target>::const_iterator i(targets->begin()), i_end(targets->end()); i != i_end; ++i)
    {
        for (std::vector<Target>::const_iterator j(i+1); j != i_end; ++j)
        {
            double distance(ublas::norm_1(j->position - i->position));
            std::cout << "distance between targets " << i->name << " and " << j->name << " is: " << distance << std::endl;
            std::cout << "    direct travelling time: " << calculate_time(v, i->position, j->position, j->velocity) << std::endl;

            if ( (distance > 0.0) && (distance < smallest_start_distance) ) // ignore the distance if it is 0 (to make sure we have a positiv radius)
                smallest_start_distance = distance;
        }
    }

    // ... and at the end such that we get a closed cycle (doing it here to simplify the algorithm aboe)
    targets->push_back(start);


    /*
     * generate the permutations
     */

    std::vector<std::shared_ptr<std::vector<unsigned long>>> permutations; // container holding all the target permutations

    // the first permutation is the ascending sequence of the numbers 0..n
    permutations.push_back( std::shared_ptr<std::vector<unsigned long>>(new std::vector<unsigned long>(targets->size())) );
    for (auto i(permutations.front()->begin()),
            i_end(permutations.front()->end()),
            i_begin(permutations.front()->begin()); i != i_end; ++i)
        *i = std::distance(i_begin, i);


    /* storing all permutations of 1..n which is quiet suboptimal since it will require n! objects */

    std::shared_ptr<std::vector<unsigned long>> permutation(new std::vector<unsigned long>(*permutations.front()));
    // we leave the front and end elements alone since they point to our start point
    while (std::next_permutation(permutation->begin()+1, permutation->end()-1))
    {
        permutations.push_back(permutation); // store each permutation in the permutation container
        permutation.reset(new std::vector<unsigned long>(*permutation));
    }

    /*
     * calculate the travelling time based on the permutations
     */

    std::shared_ptr<std::vector<unsigned long>> shortest_path_permutation; // holding the permutation with the shortest path
    std::shared_ptr<std::vector<double>> shortest_path_times; // stores the corresponding times (answer to: at which 't' did we hit the target?)
    double shortest_path_total_time(std::numeric_limits<double>::infinity()); // ... and how long was the tour

    std::cout << std::endl;
    std::cout << "============" << std::endl;
    std::cout << "permutations" << std::endl;
    std::cout << "============" << std::endl << std::endl;

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

    /*
     * check whether we really found a valid tour
     */
    if (!shortest_path_permutation)
    {
        std::cout << "no shortest path found, try with higher velocity" << std::endl;
        return 1;
    }

    /*
     * repeat the shortest path
     */
    std::cout << "shortest path was: " << targets->at(shortest_path_permutation->front()).name << " (" << shortest_path_times->at(0) << ")";
    for (auto i(shortest_path_permutation->begin()), i_end(shortest_path_permutation->end()-1), i_begin(shortest_path_permutation->begin()); i != i_end; ++i)
    {
        std::cout << " -> " << targets->at(*(i+1)).name << " (" << shortest_path_times->at(std::distance(i_begin, i)+1) << ")";
    }
    std::cout << std::endl;

    /*
     * visualize the start situation as well as the tour
     */
    osgViewer::Viewer viewer;
    osg::ref_ptr<osg::Group> root(new osg::Group);

    std::function<void (double, double, double, double, const osg::Vec4&)> addStar(
            std::bind(addStarToRoot, root.get(), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));

    osg::Vec4 color(1.0f, 0.0f, 0.0f, 1.0f); // mark the start target (red)
    for (auto i(targets->begin()), i_end(targets->end()-1); i != i_end; ++i)
    {
        addStar(i->position(0), i->position(1), i->position(2), smallest_start_distance/2.0, color);
        color[0] = 0.0f; color[2] = 1.0f; // the targets should be blue
    }


    osg::ref_ptr<osg::Vec3Array> target_positions_at_hit(new osg::Vec3Array); // store the positions to draw the tour

    for (auto i(shortest_path_permutation->begin()), i_end(shortest_path_permutation->end()-1), i_begin(shortest_path_permutation->begin()); i != i_end; ++i)
    {
        osg::Vec3 position(
                targets->at(*i).position(0) + shortest_path_times->at(std::distance(i_begin, i))*targets->at(*i).velocity(0),
                targets->at(*i).position(1) + shortest_path_times->at(std::distance(i_begin, i))*targets->at(*i).velocity(1),
                targets->at(*i).position(2) + shortest_path_times->at(std::distance(i_begin, i))*targets->at(*i).velocity(2));

        addStar(position[0], position[1], position[2], smallest_start_distance/2.0, color);

        target_positions_at_hit->push_back(position);
    }

    // draw the tour
    root->addChild(create_tour(target_positions_at_hit).get());

    viewer.setSceneData(root.get());
    return viewer.run(); // start the OSG event loop
}

