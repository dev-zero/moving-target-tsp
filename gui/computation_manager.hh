/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2011 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */

#ifndef COMPUTATION_MANAGER_HH
#define COMPUTATION_MANAGER_HH

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QMap>
#include <QtCore/QVariant>

#include <vector>

#include "gui/target_data.hh"

/// forward declarations
class QThread;
class ExhaustiveSearchRecursive;

class ComputationManager :
    public QObject
{
    Q_OBJECT
public:
    ComputationManager(QObject* p = 0);

    ~ComputationManager();

    /**
     * switch the used algorithm
     * @param identifier identifier for the algorithm
     */
    void switchAlgorithm(const QString& identifier);

    /**
     * reset parameters for the specified algorithm
     */
    void setParameters(const QString& identifier, const QMap<QString, QVariant>& parameters);


    /**
     * set the targets and essential parameters for the computation
     */
    void setData(const QList<TargetDataQt>& targets, double velocity);

public slots:
    /**
     * start the computation
     */
    void start();

    /**
     * abort the computation
     */
    void abort();

signals:
    void finished();
    void started();

    void solutionFound(const QList<std::array<double,3>>&, double, double);

private slots:
    void _algorithmFinished();

private:
    QThread* _thread;
    ExhaustiveSearchRecursive* _exhaustiveSearch;
    std::vector<TargetDataQt> _targets;
};

#endif // COMPUTATION_MANAGER_HH
