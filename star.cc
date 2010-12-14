/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2009 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */

#include <osg/Vec3>
#include <osg/Vec4>
#include <osg/Node>
#include <osg/LightSource>
#include <osg/LightModel>
#include <osg/Geode>
#include <osg/ShapeDrawable>
#include <osg/Material>
#include <osg/Billboard>
#include <osg/Texture2D>
#include <osgDB/ReadFile>

#include "star.hh"

static const char* CORONA_BILLBOARD_IMAGE = "../corona.png";

osg::ref_ptr<osg::Node> createStar(const osg::Vec3f& position, const double& radius, const osg::Vec4& color)
{
    /*
    osg::ref_ptr<osg::LightSource> sunLightSource(new osg::LightSource);

    osg::ref_ptr<osg::Light> sunLight(sunLightSource->getLight());
    sunLight->setPosition( osg::Vec4(position, 1.0f) );
    sunLight->setAmbient( osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f) ); // set the ambient component of the source to 0
    sunLightSource->setLight(sunLight);

    sunLightSource->setLocalStateSetModes(osg::StateAttribute::ON);
    sunLightSource->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::ON);

    osg::ref_ptr<osg::LightModel> lightModel(new osg::LightModel);
    lightModel->setAmbientIntensity( osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f) );
    sunLightSource->getOrCreateStateSet()->setAttribute(lightModel);
*/
    osg::ref_ptr<osg::Geode> solarSun(new osg::Geode);
    osg::ref_ptr<osg::Sphere> solarSunSphere(new osg::Sphere( position, radius ));
    osg::ref_ptr<osg::ShapeDrawable> solarSunDrawable(new osg::ShapeDrawable(solarSunSphere.get()));
    solarSun->addDrawable(solarSunDrawable.get());

    osg::ref_ptr<osg::StateSet> sunStateSet(solarSun->getOrCreateStateSet());
    osg::ref_ptr<osg::Material> material(new osg::Material);
    material->setEmission( osg::Material::FRONT_AND_BACK, color );
    sunStateSet->setAttributeAndModes( material, osg::StateAttribute::ON ); // turn the light on

    osg::ref_ptr<osg::Billboard> sunBillboard(new osg::Billboard());
    sunBillboard->setMode(osg::Billboard::POINT_ROT_EYE);

    /*
    // setup the Billboard geometry.
    osg::ref_ptr<osg::Geometry> billboardGeometry(new osg::Geometry);
*/
    // define a quad
    const double coronaRadius(radius*1.0);
/*
    osg::ref_ptr<osg::Vec3Array> coords(new osg::Vec3Array(4));
    (*coords)[0] = position + osg::Vec3(-coronaRadius, 0, -coronaRadius);
    (*coords)[1] = position + osg::Vec3(coronaRadius, 0, -coronaRadius);
    (*coords)[2] = position + osg::Vec3(coronaRadius, 0, coronaRadius);
    (*coords)[3] = position + osg::Vec3(-coronaRadius, 0, coronaRadius);;
    
    billboardGeometry->setVertexArray(coords);

    osg::Vec3Array* norms = new osg::Vec3Array(1);
    (*norms)[0] = osg::Vec3(coronaRadius, 0, 0)^osg::Vec3(0, 0, coronaRadius);
    (*norms)[0].normalize();
    
    billboardGeometry->setNormalArray(norms);
    billboardGeometry->setNormalBinding(osg::Geometry::BIND_OVERALL);

    // texture coordinates
    osg::ref_ptr<osg::Vec2Array> tcoords(new osg::Vec2Array(4));
    (*tcoords)[0].set(0.0f,0.0f);
    (*tcoords)[1].set(1.0f,0.0f);
    (*tcoords)[2].set(1.0f,1.0f);
    (*tcoords)[3].set(0.0f,1.0f);
    billboardGeometry->setTexCoordArray(0,tcoords);
    
    // set the color for the quad
    osg::ref_ptr<osg::Vec4Array> colours(new osg::Vec4Array(1));
    (*colours)[0] = color;
    billboardGeometry->setColorArray(colours);
    billboardGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

    billboardGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,4));
    */

    osg::ref_ptr<osg::Geometry> billboardGeometry(osg::createTexturedQuadGeometry(
                position + osg::Vec3d(-coronaRadius, 0.0, -coronaRadius),  //Center vector
                osg::Vec3d(2*coronaRadius, 0.0, 0.0),  //Width vector
                osg::Vec3d(0.0, 0.0, 2*coronaRadius),  //Height vector
                1.0,                        //Texture coordinates left
                1.0                         //Texture coordinatse top
                ));

    osg::ref_ptr<osg::Texture2D> texture(new osg::Texture2D);
    osg::ref_ptr<osg::Image> image(osgDB::readImageFile(CORONA_BILLBOARD_IMAGE));
    texture->setImage(image.get());
   
    if (!image.get())
        std::cerr << "error while opening the texture for the billboard: " << CORONA_BILLBOARD_IMAGE << std::endl;

    osg::ref_ptr<osg::StateSet> stateset(new osg::StateSet);
    stateset->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);
    stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
    stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
    billboardGeometry->setStateSet(stateset);

    sunBillboard->addDrawable(
        billboardGeometry.get()
        );
/*
    sunLightSource->addChild(sunBillboard);
    sunLightSource->addChild(solarSun);

    return sunLightSource;*/

//    sunBillboard->addDrawable(solarSunDrawable, osg::Vec3(0, 0, 0));

    return sunBillboard;
}

void addStarToRoot(
        osg::Group* root,
        double x, double y, double z,
        double r,
        osg::Vec4 color)
{
#if 0
    root->addChild(createStar(osg::Vec3f(x, y, z), r, color).get());
    return
#endif

    osg::Vec3 position(x, y, z);

    osg::ref_ptr<osg::Geode> solarSun(new osg::Geode);
    osg::ref_ptr<osg::Sphere> solarSunSphere(new osg::Sphere( position, r));
    osg::ref_ptr<osg::ShapeDrawable> solarSunDrawable(new osg::ShapeDrawable(solarSunSphere.get()));
    solarSun->addDrawable(solarSunDrawable.get());

    
    osg::ref_ptr<osg::StateSet> sunStateSet(solarSun->getOrCreateStateSet());
    osg::ref_ptr<osg::Material> material(new osg::Material);
    material->setEmission( osg::Material::FRONT_AND_BACK, color );
    sunStateSet->setAttributeAndModes( material, osg::StateAttribute::ON ); // turn the light on

    osg::ref_ptr<osg::Billboard> sunBillboard(new osg::Billboard());
    sunBillboard->setMode(osg::Billboard::POINT_ROT_EYE);

    const double coronaRadius(r*2.0);

    osg::ref_ptr<osg::Geometry> billboardGeometry(osg::createTexturedQuadGeometry(
                osg::Vec3d(-coronaRadius, 0.0, -coronaRadius),  //Center vector
                osg::Vec3d(2*coronaRadius, 0.0, 0.0),  //Width vector
                osg::Vec3d(0.0, 0.0, 2*coronaRadius),  //Height vector
                1.0,                        //Texture coordinates left
                1.0                         //Texture coordinatse top
                ));

    osg::ref_ptr<osg::Vec4Array> colours(new osg::Vec4Array(1));
    (*colours)[0] = color;
    billboardGeometry->setColorArray(colours);
    billboardGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

    osg::ref_ptr<osg::Texture2D> texture(new osg::Texture2D);
    osg::ref_ptr<osg::Image> image(osgDB::readImageFile(CORONA_BILLBOARD_IMAGE));
    texture->setImage(image.get());
   
    if (!image.get())
        std::cerr << "error while opening the texture for the billboard: " << CORONA_BILLBOARD_IMAGE << std::endl;

    osg::ref_ptr<osg::StateSet> stateset(new osg::StateSet);

    stateset->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);
    stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
    stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
    billboardGeometry->setStateSet(stateset);

    sunBillboard->addDrawable(
        billboardGeometry.get(),
        position);

    root->addChild(sunBillboard.get());
    root->addChild(solarSun.get());
}


