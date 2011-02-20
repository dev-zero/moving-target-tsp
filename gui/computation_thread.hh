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
#include <QList>

#include <gui/target_data.hh>

class ComputationThread :
    public QThread
{
Q_OBJECT
public:
    void run()
    {
        _stop = false;

        while (!_stop)
            sleep(3);
    }

public slots:
    void start(const QList<TargetDataQt>& targets)
    {
        _targets = targets;
        QThread::start();
    }
    void stop()
    {
        _stop = true;
    }
private:
    bool _stop;
    QList<TargetDataQt> _targets;
};
#endif // COMPUTATION_THREAD_HH
