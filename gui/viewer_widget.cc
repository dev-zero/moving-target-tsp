/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2011 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */

#include <QVBoxLayout>
#include <QTimer>

#include <osgQt/GraphicsWindowQt>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/TrackballManipulator>
#include <osg/Switch>

#include "gui/viewer_widget.hh"

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
    return line_geode.release();
}

const double ViewerWidget::STAR_RADIUS = 1.0;

ViewerWidget::ViewerWidget(QWidget* p) :
    QWidget(p),
    _timer(new QTimer),
    _path(NULL)
{
    setThreadingModel(osgViewer::Viewer::SingleThreaded);

    osg::DisplaySettings* ds = osg::DisplaySettings::instance().get();

    osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
    traits->windowName = "";
    traits->windowDecoration = false;
    traits->x = 0;
    traits->y = 0;
    traits->width = 100;
    traits->height = 100;
    traits->doubleBuffer = true;
    traits->alpha = ds->getMinimumNumAlphaBits();
    traits->stencil = ds->getMinimumNumStencilBits();
    traits->sampleBuffers = ds->getMultiSamples();
    traits->samples = ds->getNumMultiSamples();

    osgQt::GraphicsWindowQt* gw(new osgQt::GraphicsWindowQt(traits.get()));

    osg::ref_ptr<osg::Camera> camera = new osg::Camera;
    camera->setGraphicsContext(gw);
    camera->setClearColor( osg::Vec4(0.2, 0.2, 0.6, 1.0) );
    camera->setViewport( new osg::Viewport(0, 0, traits->width, traits->height) );
    camera->setProjectionMatrixAsPerspective(30.0f, static_cast<double>(traits->width)/static_cast<double>(traits->height), 1.0f, 10000.0f );

    _root = new osg::Switch;
    _root->setNewChildDefaultValue(false); // add children hidden by default

    setCamera(camera);
    addEventHandler(new osgViewer::StatsHandler);
    setCameraManipulator(new osgGA::TrackballManipulator);
    setSceneData(_root);

    QWidget* graphWidget(gw->getGraphWidget());
    graphWidget->setObjectName("graphwidget");

    QVBoxLayout *verticalLayout(new QVBoxLayout);
    verticalLayout->addWidget(graphWidget);
    setLayout(verticalLayout);

    connect( _timer, SIGNAL(timeout()), this, SLOT(update()) );
    _timer->start(40);
}

void ViewerWidget::paintEvent(QPaintEvent*)
{
    frame();
}

unsigned int ViewerWidget::addTarget(const TargetDataQt& t)
{
    addStarToRoot(_root, t.position[0], t.position[1], t.position[2], STAR_RADIUS, osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
    return _root->getNumChildren()-1; // the inserted item is supposed to be the last one. Is that really true? ... premisse: addStarToRoot adds only one child
}

void ViewerWidget::displayPath(const QList<std::array<double,3>>& list)
{
    std::array<double,3> p = {{ 0.0, 0.0, 0.0 }};
    osg::ref_ptr<osg::Vec3Array> positions(new osg::Vec3Array);

    osg::ref_ptr<osg::Group> path(new osg::Group);
    foreach(p, list)
    {
        positions->push_back(osg::Vec3(p[0], p[1], p[2]));
        addStarToRoot(path.get(), p[0], p[1], p[2], STAR_RADIUS,  osg::Vec4(1.0f, 1.0f, 0.0f, 1.0f));
    }

    path->addChild(create_tour(positions.get()));

    _root->setNewChildDefaultValue(true);
    if (_path != NULL)
        _root->setChild(_root->getChildIndex(_path), path.get());
    else
        _root->addChild(path.get());

    _path = path.get();
    _root->setNewChildDefaultValue(false);
}

void ViewerWidget::clear()
{
    _root->removeChildren(0, _root->getNumChildren());
    _path = NULL;
}
void ViewerWidget::enableTarget(unsigned int idx)
{
    _root->setValue(idx, true);
}
void ViewerWidget::disableTarget(unsigned int idx)
{
    _root->setValue(idx, false);
}

