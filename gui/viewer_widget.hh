/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2011 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */

#ifndef VIEWER_WIDGET_HH
#define VIEWER_WIDGET_HH

#include <QtGui/QWidget>
#include <QtCore/QList>
#include <osgViewer/CompositeViewer>
#include <array>

#include "gui/star.hh"
#include "base/target_data.hh"

/// forward declarations to reduce dependencies
class QTimer;
namespace osg
{
    class Switch;
    class Group;
}
namespace osgViewer
{
    class View;
}

class ViewerWidget :
    public QWidget, public osgViewer::CompositeViewer
{
    Q_OBJECT

public:
    
    ViewerWidget(QWidget* p);

    virtual void paintEvent(QPaintEvent*);

    unsigned int addTarget(const TargetDataQt& t);

    void displayPath(const QList<std::array<double,3>>& list);

public slots:
    void clear();
    void enableTarget(unsigned int idx);
    void disableTarget(unsigned int idx);

protected:
    QTimer* _timer;

private:
    osg::Switch* _root;
    osg::Group* _path;
    osgViewer::View* _view;

    static const double STAR_RADIUS;
};

#endif // VIEWER_WIDGET_HH
