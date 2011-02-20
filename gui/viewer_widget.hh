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

class ViewerWidget :
    public QWidget, public osgViewer::CompositeViewer
{
    Q_OBJECT
public:
    
    ViewerWidget(QWidget* p) : QWidget(p)
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
protected:
    QTimer _timer;
private:
    osg::Switch* _root;
    osgViewer::View* _view;
};

#endif // VIEWER_WIDGET_HH
