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
#include <tuple>

#include "target_data.hh"

class ComputationThread :
    public QThread
{
Q_OBJECT
public:
    ComputationThread() :
        QThread(),
        _currentSACoolingSchedule(10.0, 0.9, 100, 0.01)
    {
    }

    void run();

    std::tuple<double, double, unsigned int, double> getCurrentSACoolingSchedule() const
    {
        return _currentSACoolingSchedule;
    }
public slots:
    void start(const QList<TargetDataQt>& targets, double velocity, const QString& method);
    void stop();
    void setCurrentSACoolingSchedule(const std::tuple<double, double, unsigned int, double>& newSACoolingSchedule)
    {
        _currentSACoolingSchedule = newSACoolingSchedule;
    }

signals:
    void log(const QString&);

    /*
     * returns the position of the encounters, the total (external) time and the complete length
     */
    void solutionFound(const QList<std::array<double,3>>&, double, double);

private:
    bool _stop;
    QList<TargetDataQt> _targets;
    double _velocity;
    QString _method;
    std::tuple<double, double, unsigned int, double> _currentSACoolingSchedule;
};
#endif // COMPUTATION_THREAD_HH
