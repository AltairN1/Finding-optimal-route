#pragma once

#include <QList>
#include <QPointF>
#include <QAbstractTableModel>

class tableModel : public QAbstractTableModel
{
    QVariant headerData(int section, Qt::Orientation orientation,int role) const override;
    int rowCount(const QModelIndex & parent) const override;
    int columnCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;

    QList<QPointF> m_points;

public:
    tableModel(QList<QPointF> points) : m_points(points) {}
};
