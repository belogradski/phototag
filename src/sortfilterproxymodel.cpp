#include "sortfilterproxymodel.h"
#include "imagelistmodel.h"

SortFilterProxyModel::SortFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

void SortFilterProxyModel::setFilterActive(bool active)
{
    mApplyFilter = active;
    invalidateFilter();
}

bool SortFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    QVariant leftData = sourceModel()->data(left);
    QVariant rightData = sourceModel()->data(right);
    if(leftData.type() == QVariant::String)
        return leftData.toString() < rightData.toString();

    Q_ASSERT(false);
    return false;
}


bool SortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    if(!mApplyFilter)
        return true;

    QModelIndex index0 = sourceModel()->index(sourceRow, 0, sourceParent);
    return !sourceModel()->data(index0, ImageListModel::HasGeoTag).toBool();
}
