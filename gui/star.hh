/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2009 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */

#ifndef STAR_HH
#define STAR_HH

#include <osg/ref_ptr>
#include <osg/Node>
#include <osg/Vec3>
#include <osg/Vec4>

osg::ref_ptr<osg::Node> createStar(
    const osg::Vec3f& position,
    const double& radius,
    const osg::Vec4& color = osg::Vec4(1, 1, 0.5, 0.5));

osg::ref_ptr<osg::Geode> createSimpleStar(
        const double& x, const double& y, const double& z,
        const double& r,
        const osg::Vec4& color
        );

void addStarToRoot(
        osg::Group* root,
        double x, double y, double z,
        double r,
        osg::Vec4 color
        );

#endif // STAR_HH
