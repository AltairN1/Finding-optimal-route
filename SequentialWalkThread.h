#pragma once

#include <QThread>
#include <QAbstractItemModel>
#include <QFile>
#include <QDir>
#include <QDesktopServices>
#include <QUrl>

namespace {
const bool changeDeparturePoint = false;
}

class SequentialWalkThread : public QThread
{
    Q_OBJECT
    QAbstractItemModel* m_model = nullptr;
    QList<int> m_points;
    QPair<QString, qreal> m_optimalRoute;
    int m_counter = 0;

public:
    SequentialWalkThread(QAbstractItemModel * model, const int amountOfPoints)
        : m_model(model) {
        for ( int i = 1; i <= amountOfPoints; i++ ) m_points.push_back(i);
        m_points.shrink_to_fit();
        m_optimalRoute.second = 0;
    }

protected:
    void run() {
        int iterationCount = changeDeparturePoint ? m_points.size() : 1;
        QDir::setCurrent(QDir::tempPath());
        QFile file ("SequentialWalk.txt");
        file.setPermissions(QFileDevice::ExeOther);
        if (file.open(QIODevice::WriteOnly)) {
            QTextStream stream(&file);
            for (int i = 1; i <= iterationCount; i++) {
                QList<int> l_points = m_points;
                l_points.erase(std::remove_if(l_points.begin(), l_points.end(),
                                              [=](int& point){ return point == i; }), l_points.end());
                do {
                    if (l_points.first() > l_points.last()) continue;
                    qreal distance = distanceFromModel(i, l_points.first());
                    for (int i = 1; i < l_points.size(); i++) {
                        distance += distanceFromModel(l_points.at(i-1), l_points.at(i));
                    }
                    distance += distanceFromModel(i, l_points.last());
                    if ((distance < m_optimalRoute.second) || !m_optimalRoute.second) {
                        updateOptimalRoute(i, distance, l_points);
                    }
                    stream << routeFromPoints(i, distance, l_points);
                } while (std::next_permutation(l_points.begin(), l_points.end()));
            }
            QString str("\"" + m_optimalRoute.first + "\" = " + QString::number(m_optimalRoute.second));
            stream << tr("\nОптимальный маршрут: ") + str;
            emit route(str);
            file.close();
           QDesktopServices::openUrl(QUrl::fromLocalFile(QDir::currentPath() + "\\"+ file.fileName()));
        } else {
            emit route(tr("Не удалось создать файл!"));
      }
    }

private:
    qreal distanceFromModel(int qRow, int qColumn )
    {
        if ((qColumn < 2) || (qRow > qColumn)) qSwap(qColumn, qRow);
        return m_model->data(m_model->index(qRow - 1, qColumn - 2), Qt::DisplayRole).toDouble();
    }

    void updateOptimalRoute(int i, qreal distance, const QList<int>& points) {
        m_optimalRoute.first = QString::number(i);
        foreach(int point, points) m_optimalRoute.first.push_back("," + QString::number(point));
        m_optimalRoute.first.push_back("," + QString::number(i));
        m_optimalRoute.second = distance;
    };

    QString routeFromPoints(int i, qreal distance, const QList<int>& points) {
        m_counter++;
        QString str = tr("Номер маршрута: ") + QString::number(m_counter) + " \"" + QString::number(i);
        foreach(int point, points) str.push_back("," + QString::number(point));
        return str + "," + QString::number(i) + "\" = " + QString::number(distance) + "\n";
    };

signals:
    void route(QString);
};

