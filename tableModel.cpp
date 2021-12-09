#include "tableModel.h"

#include <QVariant>

int tableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_points.size() - 1;
}

int tableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_points.size() - 1;
}

QVariant tableModel::data(const QModelIndex &index, int role) const
{
    if ((role == Qt::DisplayRole) && (index.row() <= index.column())) {
        qreal x = m_points.at(index.column() + 1).x() - m_points.at(index.row()).x();
        qreal y = m_points.at(index.column() + 1).y() - m_points.at(index.row()).y();
        qreal AB = sqrt(x*x + y*y);
        return QVariant(round(AB*100)/100);
    }
    return QVariant();
}

QVariant tableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(role);
    if (role == Qt::DisplayRole) {
        section += (orientation == Qt::Horizontal) ? 2 : 1;
        return (QString("q") + QString::number(section));
    }
    return QVariant();
}
