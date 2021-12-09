#pragma once

#include <QThread>
#include <QAbstractItemModel>
#include <QFile>
#include <QDir>
#include <QDesktopServices>
#include <QUrl>

class NearestPeakThread : public QThread
{
    Q_OBJECT
    QAbstractItemModel* m_model = nullptr;
    int m_currentPos = 0;
    QList<int> m_points;
    QPair<QString, qreal> m_optimalRoute;
    int m_counter = 0;

public:
    NearestPeakThread(QAbstractItemModel * model, int amountOfPoints)
        : m_model(model) {
        for ( int i = 2; i <= amountOfPoints; i++ ) m_points.append(i);
        m_optimalRoute.second = 0;
        m_points.append(1);
    }

protected:
    void run() {
        QDir::setCurrent(QDir::tempPath());
        QFile file("NearestPeak.txt");
        file.setPermissions(QFileDevice::ExeOther);
        if (file.open(QIODevice::WriteOnly)){
            QTextStream stream(&file);
            for (int i = 1; i <= m_points.size(); i++) {
                auto l_points = m_points;
                l_points.erase(std::remove_if(l_points.begin(), l_points.end(), [=](int& point){
                    return point == i;}), l_points.end());
                m_currentPos = i;
                 QPair<QString, double> distance;
                 while (!l_points.isEmpty()) {
                     auto pair = nearestPoint(l_points);
                     distance.first.append(pair.first);
                     distance.second += pair.second;
                 }
                 distance.second += distanceFromModel(i, m_currentPos);
                 if ((distance.second < m_optimalRoute.second) || !m_optimalRoute.second) updateOptimalRoute(i, distance.second, distance.first);
                 stream << routeFromPoints(i, distance.second, distance.first);
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
    QPair<QString, qreal> nearestPoint(QList<int>& points) {
        qreal distance = 0;
        int pos = 0;
        foreach(int point, points) {
            qreal l_distance;
            l_distance = (m_currentPos > point)
                      ? distanceFromModel(point, m_currentPos)
                      : distanceFromModel(m_currentPos, point );
            if ( l_distance && (!distance || ( distance > l_distance)) ) {
                pos = point;
                distance = l_distance;
            }
        }
        points.erase(std::remove_if(points.begin(), points.end(), [=](int& point){return point == pos;}), points.end());
        m_currentPos = pos;
        return {("," + QString::number(pos)), distance};
    }

    qreal distanceFromModel(int qRow, int qColumn )
    {
        if ((qColumn < 2) || (qRow > qColumn)) qSwap(qColumn, qRow);
        return m_model->data(m_model->index(qRow - 1, qColumn - 2), Qt::DisplayRole).toDouble();
    }

     void updateOptimalRoute(int i, qreal distance, QString points) {
        m_optimalRoute.first = QString::number(i) + points + ("," + QString::number(i));
        m_optimalRoute.second = distance;
    };

    QString routeFromPoints(int i, qreal distance, QString points) {
        m_counter++;
        return tr("Номер маршрута: ") + QString::number(m_counter) + " \"" + QString::number(i) + points + "," + QString::number(i) + "\" = " + QString::number(distance) + "\n";
    };

signals:
    void route(QString);
};

