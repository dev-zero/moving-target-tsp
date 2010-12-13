/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2009 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */

#include <osg/ref_ptr>
#include <osg/Group>
#include <osg/Vec4>
#include <osg/ClearNode>
#include <osg/Vec3>
#include <osgViewer/Viewer>

#include "star.hh"

int main(int, char**)
{
    osg::ref_ptr<osg::Group> root(new osg::Group);

    // clear everything
    osg::ref_ptr<osg::ClearNode> clearNode(new osg::ClearNode);
    clearNode->setClearColor( osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f) );
    root->addChild(clearNode.get());
    
    root->addChild(createStar(osg::Vec3(0, 0, 0), 109));

    osg::DisplaySettings::instance()->setNumMultiSamples(4); // anti-aliasing

    osgViewer::Viewer viewer;
    viewer.setSceneData(root);
    return viewer.run();
}

