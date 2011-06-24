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
class SimulatedAnnealing;

class ComputationManager :
    public QObject
{
    Q_OBJECT
public:
    ComputationManager(QObject* p = 0);

    ~ComputationManager();

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

    /**
     * switch the used algorithm
     * @param identifier identifier for the algorithm
     */
    void switchAlgorithm(const QString& identifier);

signals:
    void finished();
    void started();

    void solutionFound(const QList<std::array<double,3>>&, double, double);

private slots:
    void _algorithmFinished();

private:
    QThread* _thread;
    ExhaustiveSearchRecursive* _exhaustiveSearch;
    SimulatedAnnealing* _simulatedAnnealing;
    std::vector<TargetDataQt> _targets;

    QString _selectedAlgorithm;
};

#endif // COMPUTATION_MANAGER_HH
