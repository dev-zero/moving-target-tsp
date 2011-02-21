/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2011 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */

#ifndef COMPUTATION_THREAD_HH
#define COMPUTATION_THREAD_HH

#include <QThread>
#include <QString>
#include <QList>
#include <array>

#include "target_data.hh"

class ComputationThread :
    public QThread
{
Q_OBJECT
public:
    void run();

public slots:
    void start(const QList<TargetDataQt>& targets, double velocity);
    void stop();

signals:
    void log(const QString&);
    void solutionFound(const QList<std::array<double,3>>&);

private:
    bool _stop;
    QList<TargetDataQt> _targets;
    double _velocity;
};
#endif // COMPUTATION_THREAD_HH
