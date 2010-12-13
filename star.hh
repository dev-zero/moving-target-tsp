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

static const char* CORONA_BILLBOARD_IMAGE = "../Corona.tga";

#endif // STAR_HH
