/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2009 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */

#include "gui/target_data_filter_proxy_model.hh"
#include "utils/common_calculation_functions.hh"

TargetDataFilterProxyModel::TargetDataFilterProxyModel(QObject* p) :
    QSortFilterProxyModel(p),
    _minimalDistance(0.0),
    _maximalDistance(0.0),
    _distanceFactor(1.0),
    _distanceFiltering(false)
{
}

void TargetDataFilterProxyModel::setFilterMinimalDistance(double distance)
{
    _minimalDistance = distance;
    invalidateFilter();
}

void TargetDataFilterProxyModel::setFilterMaximalDistance(double distance)
{
    _maximalDistance = distance;
    invalidateFilter();
}

void TargetDataFilterProxyModel::setFilterDistanceFactor(double factor)
{
    _distanceFactor = factor;
    invalidateFilter();
}

bool TargetDataFilterProxyModel::filterAcceptsRow(int sourceRow,
         const QModelIndex &sourceParent) const
{
    QModelIndex idx(sourceModel()->index(sourceRow, 0, sourceParent));

    return
        sourceModel()->data(idx).toString().contains(filterRegExp())
        && distanceInRange(sourceModel()->data(idx, Qt::UserRole + 1).value<TargetDataQt>());
}

bool TargetDataFilterProxyModel::distanceInRange(const TargetDataQt& target) const
{
    if (!_distanceFiltering)
        return true;

    const double v(norm(target.position));
    return (_minimalDistance <= v*_distanceFactor) && (v <= _maximalDistance*_distanceFactor);
}

void TargetDataFilterProxyModel::distanceFiltering(bool filtering)
{
    _distanceFiltering = filtering;
    invalidateFilter();
}
