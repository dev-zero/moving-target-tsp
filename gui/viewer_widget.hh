/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2011 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */

#ifndef VIEWER_WIDGET_HH
#define VIEWER_WIDGET_HH

#include <QWidget>
#include <QVBoxLayout>
#include <QTimer>

#include <osgQt/GraphicsWindowQt>
#include <osgViewer/CompositeViewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/TrackballManipulator>
#include <osg/Switch>

#include "star.hh"
#include "target_data.hh"

#include <iostream>

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

class ViewerWidget :
    public QWidget, public osgViewer::CompositeViewer
{
    Q_OBJECT
public:
    
    ViewerWidget(QWidget* p) :
        QWidget(p),
        _path(NULL)
    {
        setThreadingModel(osgViewer::CompositeViewer::SingleThreaded);

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

        osg::ref_ptr<osg::Camera> camera = new osg::Camera;
        camera->setGraphicsContext( new osgQt::GraphicsWindowQt(traits.get()) );
        
        camera->setClearColor( osg::Vec4(0.2, 0.2, 0.6, 1.0) );
        camera->setViewport( new osg::Viewport(0, 0, traits->width, traits->height) );
        camera->setProjectionMatrixAsPerspective(30.0f, static_cast<double>(traits->width)/static_cast<double>(traits->height), 1.0f, 10000.0f );

        _view = new osgViewer::View;
        _root = new osg::Switch;
        _root->setNewChildDefaultValue(false); // add children hidden by default

        _view->setCamera( camera );
        addView( _view );

        _view->addEventHandler( new osgViewer::StatsHandler );
        _view->setCameraManipulator( new osgGA::TrackballManipulator );
        _view->setSceneData(_root);

        osgQt::GraphicsWindowQt* gw = dynamic_cast<osgQt::GraphicsWindowQt*>( camera->getGraphicsContext() );

        QVBoxLayout *verticalLayout(new QVBoxLayout);
        QWidget* graphWidget(gw->getGraphWidget());
        graphWidget->setObjectName("graphwidget");
        verticalLayout->addWidget(graphWidget);
        setLayout(verticalLayout);

        connect( &_timer, SIGNAL(timeout()), this, SLOT(update()) );
        _timer.start(5);
    }

    virtual void paintEvent(QPaintEvent*)
    {
        frame();
    }

    unsigned int addTarget(const TargetDataQt& t)
    {
        addStarToRoot(_root, t.position[0], t.position[1], t.position[2], 0.01, osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
        return _root->getNumChildren()-1; // the inserted item is supposed to be the last one. Is that really true? ... premisse: addStarToRoot adds only one child
    }
    void clear()
    {
        _root->removeChildren(0, _root->getNumChildren());
    }
    void enableTarget(unsigned int idx)
    {
        _root->setValue(idx, true);
    }
    void disableTarget(unsigned int idx)
    {
        _root->setValue(idx, false);
    }

    void displayPath(const QList<std::array<double,3>>& list)
    {
        std::array<double,3> p = {{ 0.0, 0.0, 0.0 }};
        osg::ref_ptr<osg::Vec3Array> positions(new osg::Vec3Array);

        osg::ref_ptr<osg::Group> path(new osg::Group);
        foreach(p, list)
        {
            positions->push_back(osg::Vec3(p[0], p[1], p[2]));
            addStarToRoot(path.get(), p[0], p[1], p[2], 0.01,  osg::Vec4(1.0f, 1.0f, 0.0f, 1.0f));
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
protected:
    QTimer _timer;
private:
    osg::Switch* _root;
    osg::Group* _path;
    osgViewer::View* _view;
};

#endif // VIEWER_WIDGET_HH
