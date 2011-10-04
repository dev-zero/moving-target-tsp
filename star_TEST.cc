/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2011 Tiziano Müller <tm@dev-zero.ch>
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
#include <osg/MatrixTransform>
#include <cstdlib>

#include "star.hh"

int main(int argc, char* argv[])
{
    size_t numberOfStars(5);
    int width(100);

    if (argc > 1)
        numberOfStars = atoi(argv[1]);

    osg::ref_ptr<osg::Group> root(new osg::Group);

    // clear everything
    osg::ref_ptr<osg::ClearNode> clearNode(new osg::ClearNode);
    clearNode->setClearColor( osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f) );
    root->addChild(clearNode.get());
 
    osg::ref_ptr<osg::Geode> starTemplate(createSimpleStar(0, 0, 0, 1, osg::Vec4(1, 1, 1, 1)));
//    root->addChild(starTemplate.get());

    for (size_t i(0); i < numberOfStars; ++i)
    {
        osg::ref_ptr<osg::MatrixTransform> star(new osg::MatrixTransform);
        star->setMatrix(osg::Matrix::translate(rand() % width - width/2, rand() % width - width/2, rand() % width - width/2));
        star->addChild(starTemplate.get());
        root->addChild(star.get());    
    }

//    osg::DisplaySettings::instance()->setNumMultiSamples(4); // anti-aliasing

    osgViewer::Viewer viewer;
    viewer.setSceneData(root);
    return viewer.run();
}

