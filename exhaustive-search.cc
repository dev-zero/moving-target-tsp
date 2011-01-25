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

#include <osgViewer/Viewer>
#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osg/Vec3>

#include "target.hh"
#include "sources/file_source.hh"
#include "parser/csv.hh"
#include "cli/exhaustive-search.hxx"
#include "star.hh"
#include "common_calculation_functions.hh"

class MovingTargetTSP
{
private:
    std::shared_ptr<std::vector<Target>> _targets;
public:
    MovingTargetTSP()
    {
        _targets.reset(new std::vector<Target>);
    }
    void add_target(const double& x, const double& y, const double& z,
        const double& v_x, const double& v_y, const double& v_z,
        const std::string& name)
    {
        Target t(x, y, z, v_x, v_y, v_z, name);
        _targets->push_back(t);
    }

    /* calculate the cartesian product of the target set and execute the
     * specified function on all pairs
     * TODO: remove the Target struct and use (array, string, array, string) for the func signature
     */
    typedef std::function<void (const Target& a, const Target& b)> cartesian_product_func;
    void for_each_pair(cartesian_product_func func)
    {
        for (std::vector<Target>::const_iterator i(_targets->begin()), i_end(_targets->end()); i != i_end; ++i)
        {
            for (std::vector<Target>::const_iterator j(_targets->begin()); j != i_end; ++j)
            {
                func(*i, *j);
            }
        }
    }

    void for_each_target(std::function<void (const Target&)> func)
    {
        for (std::vector<Target>::const_iterator i(_targets->begin()), i_end(_targets->end()); i != i_end; ++i)
            func(*i);
    }

    void for_each_permutated_target(std::function<void (const Target&, size_t)> func, std::function<size_t (size_t)> permutation)
    {
        for (size_t n(0); n < _targets->size(); ++n)
            func(_targets->at(permutation(n)), n);
    }


    Target get_target(size_t n) const
    {
        return _targets->at(n);
    }

    size_t size() const
    {
        return _targets->size();
    }
};

/* output our targets while they're added */
void target_appender(MovingTargetTSP& app,
        const double& x, const double& y, const double& z,
        const double& v_x, const double& v_y, const double& v_z,
        const std::string& name)
{
    app.add_target(x, y, z, v_x, v_y, v_z, name);
    std::cout << "target added" << std::endl;
    std::cout << "    name:     " << name << std::endl;
    std::cout << "    position: " << x << ", " << y << ", " << z << std::endl;
    std::cout << "    velocity: " << v_x << ", " << v_y << ", " << v_z << std::endl;
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

    /* handle commandline arguments */
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

    MovingTargetTSP app;

    target_appender(app, 0, 0, 0, 0, 0, 0, "start"); // create a fake start-target (currently hardcoded at 0/0/0 with velocity 0/0/0)

    file.load( std::bind(target_appender, app,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
                std::placeholders::_4, std::placeholders::_5, std::placeholders::_6,
                std::placeholders::_7) ); // bind the MovingTargetTSP instance to our appender-function and passing that to the source

    const double start_time(0.0); // initialize our start-time (could be hardocded but we may want to start at a different time than "now")

    double smallest_start_distance(std::numeric_limits<double>::infinity()); // the smallest distance between two targets at t=0, mostly needed to calculated an appropriate radius for the 3D-objects
    
    app.for_each_pair( std::bind(
                calculate_distance_and_direct_travelling_time,
                &smallest_start_distance, v,
                std::placeholders::_1, std::placeholders::_2) );

    target_appender(app, 0, 0, 0, 0, 0, 0, "end");

    /*
     * calculate the travelling time for all permutations
     */

    std::vector<unsigned long> shortest_path_permutation; // holding the permutation with the shortest path
    std::vector<double> shortest_path_times(app.size()), path_times(app.size()); // stores the corresponding times (answer to: at which 't' did we hit the target?)
    double shortest_path_total_time(std::numeric_limits<double>::infinity()); // ... and how long was the tour

    std::cout << std::endl;
    std::cout << "============" << std::endl;
    std::cout << "permutations" << std::endl;
    std::cout << "============" << std::endl << std::endl;

    std::vector<unsigned long> permutation(app.size());
    unsigned long count(0);
    std::generate(permutation.begin(), permutation.end(), [&count]() { return count++; });

    do
    {
        double total_t(start_time);

        std::cout << app.get_target(permutation.front()).name << " (" << total_t << ")";
        path_times.front() = total_t;

        for (auto j(permutation.begin()), j_end(permutation.end()-1); j != j_end; ++j)
        {
            total_t += calculate_time(v,
                    app.get_target(*j).position + total_t*app.get_target(*j).velocity,
                    app.get_target(*(j+1)).position + total_t*app.get_target(*(j+1)).velocity,
                    app.get_target(*(j+1)).velocity);
            std::cout << " -> " << app.get_target(*(j+1)).name << " (" << total_t << ")";
            path_times.at(std::distance(permutation.begin(), j+1)) = total_t;
        }
        std::cout << " = " << total_t << std::endl;
        if (total_t < shortest_path_total_time)
        {
            shortest_path_total_time = total_t;
            shortest_path_times.swap(path_times);
            shortest_path_permutation.assign(permutation.begin(), permutation.end());
        }
    } while (std::next_permutation(permutation.begin()+1, permutation.end()-1));

    /*
     * check whether we really found a valid tour
     */
    if (!shortest_path_permutation.size())
    {
        std::cout << "no shortest path found, try with higher velocity" << std::endl;
        return 1;
    }

    /*
     * repeat the shortest path
     */
    std::cout << "shortest path was: " << app.get_target(shortest_path_permutation.front()).name << " (" << shortest_path_times.front() << ")";
    for (auto i(shortest_path_permutation.begin()), i_end(shortest_path_permutation.end()-1), i_begin(shortest_path_permutation.begin()); i != i_end; ++i)
    {
        std::cout << " -> " << app.get_target(*(i+1)).name << " (" << shortest_path_times.at(std::distance(i_begin, i)+1) << ")";
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
    app.for_each_target(
            [&color, &smallest_start_distance, &addStar](const Target& t) {
                addStar(t.position(0), t.position(1), t.position(2), smallest_start_distance/2.0, color);
                color[0] = 0.0f; color[2] = 1.0f; // the targets should be blue
                } );

    osg::ref_ptr<osg::Vec3Array> target_positions_at_hit(new osg::Vec3Array); // store the positions to draw the tour

    app.for_each_permutated_target(
            [&shortest_path_times, &target_positions_at_hit, &addStar, &color, smallest_start_distance](const Target& t, size_t n) {
                osg::Vec3 position(
                    t.position(0) + shortest_path_times.at(n)*t.velocity(0),
                    t.position(1) + shortest_path_times.at(n)*t.velocity(1),
                    t.position(2) + shortest_path_times.at(n)*t.velocity(2));
                addStar(position[0], position[1], position[2], smallest_start_distance/2.0, color);
                target_positions_at_hit->push_back(position);
                },
            [&shortest_path_permutation](size_t n) {
                return shortest_path_permutation[n];
            }
        );

    // draw the tour
    root->addChild(create_tour(target_positions_at_hit).get());

    viewer.setSceneData(root.get());
    return viewer.run(); // start the OSG event loop
}

