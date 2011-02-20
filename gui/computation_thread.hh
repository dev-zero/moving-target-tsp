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

        for(QList<TargetDataQt>::const_iterator t(_targets.begin()), t_end(_targets.end()); t != t_end; ++t)
        {
            log(QString("found target '%1' in the tour").arg(t->name));
        }
        while (!_stop)
            sleep(3);
    }

public slots:
    void start(const QList<TargetDataQt>& targets)
    {
        _targets = targets;
        emit log(QString("calculating tour for %1 targets").arg(_targets.size()));
        QThread::start();
    }
    void stop()
    {
        _stop = true;
    }

signals:
    void log(const QString&);

private:
    bool _stop;
    QList<TargetDataQt> _targets;
};
#endif // COMPUTATION_THREAD_HH
