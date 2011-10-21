/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2011 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */

#ifndef TARGET_DATA_FILTER_PROXY_MODEL_HH
#define TARGET_DATA_FILTER_PROXY_MODEL_HH

#include <QtGui/QSortFilterProxyModel>

#include "base/target_data.hh"

class TargetDataFilterProxyModel :
    public QSortFilterProxyModel
{
    Q_OBJECT
public:
    TargetDataFilterProxyModel(QObject* parent = 0);

    double filterMinimalDistance() const { return _minimalDistance; }

    double filterMaximalDistance() const { return _maximalDistance; }

public slots:
    void distanceFiltering(bool);

    void setFilterMinimalDistance(double distance);
    void setFilterMaximalDistance(double distance);

    void setFilterDistanceFactor(double factor);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

private:
    double _minimalDistance, _maximalDistance, _distanceFactor;

    bool distanceInRange(const TargetDataQt& target) const;

    bool _distanceFiltering;
};

#endif // TARGET_DATA_FILTER_PROXY_MODEL_HH
