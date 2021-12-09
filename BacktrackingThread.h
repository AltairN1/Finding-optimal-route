#pragma once

#include <QThread>
#include <QAbstractItemModel>
#include <QFile>
#include <QDir>
#include <QDesktopServices>
#include <QUrl>

namespace  {
    std::optional< const int > numberOfUniqueRoutes; // если количестово уникальных строк не требуется, то оставить пустым
}

class BacktrackingThread : public QThread
{
    Q_OBJECT
    QAbstractItemModel* m_model = nullptr;
    QList<int> m_points;
    QPair<QString, qreal> m_optimalRoute;
    qreal m_countСalculations = 0;
    int m_countIteration = 0;
    int m_counter = 0;

public:
    BacktrackingThread(QAbstractItemModel * model, const int amountOfPoints)
        : m_model(model) {
        for ( int i = 2; i <= amountOfPoints; i++ ) m_points.push_back(i);
        m_optimalRoute.second = 0;
    }

protected:
    void run() {
        QDir::setCurrent(QDir::tempPath());
        QFile file ("Backtracking.txt");
        file.setPermissions(QFileDevice::ExeOther);
        if (file.open(QIODevice::WriteOnly)){
            QTextStream stream(&file);
            calculatingOptimalRoute(stream);
            QString str ("\"" + m_optimalRoute.first + "\" = " + QString::number(m_optimalRoute.second) +
                         tr("; Кув = ") + QString::number(reductionFactor(), 'f', 2));
            stream << tr("\nОптимальный маршрут: ") + str;
            emit route(str);
            file.close();
            QDesktopServices::openUrl(QUrl::fromLocalFile(QDir::currentPath() + "\\"+ file.fileName()));
        } else {
            emit route (tr("Не удалось создать файл!"));
        }
    }

private:
    void calculatingOptimalRoute(QTextStream& stream) {
        do {
            if (m_points.first() > m_points.last()) continue;
            if (numberOfUniqueRoutes.has_value() && (m_countIteration >= numberOfUniqueRoutes.value())) break;
            qreal distance = m_model->data(m_model->index(0, m_points.first() - 2), Qt::DisplayRole).toDouble();
            QString summs (QString::number(distance));
            bool wasBreak = false;
            for (int i = 1; i < m_points.size(); i++) {
                distance += distanceFromModel(m_points.at(i-1), m_points.at(i));
                summs.push_back( "; " + QString::number(distance));
                if ( m_optimalRoute.second && (distance > m_optimalRoute.second)) {
                    wasBreak = true;
                    break;
                }
            }
            if ( !wasBreak ) {
                distance += distanceFromModel(1, m_points.last());
                summs.push_back("; " + QString::number(distance));
                if (!m_optimalRoute.second || (distance < m_optimalRoute.second)) {
                    updateOptimalRoute(distance);
                }
            }
            m_countIteration++;
            stream << routeFromPoints(distance, m_points, summs);
        } while (std::next_permutation(m_points.begin(), m_points.end()));
    }

    const qreal reductionFactor() { // коэффициент уменьшения вычислений
        return m_countСalculations / (m_countIteration * (m_points.size()));
    }

    const qreal distanceFromModel(int qRow, int qColumn )
    {
        if ((qColumn < 2) || (qRow > qColumn)) qSwap(qColumn, qRow);
        m_countСalculations++;
        return m_model->data(m_model->index(qRow - 1, qColumn - 2), Qt::DisplayRole).toDouble();
    }

    void updateOptimalRoute(const qreal distance) {
        m_optimalRoute.first = "1";
        foreach(int point, m_points) m_optimalRoute.first.push_back ("," + QString::number(point));
        m_optimalRoute.first.push_back(",1");
        m_optimalRoute.second = distance;
    };

    QString routeFromPoints(const qreal distance, const QList<int>& points, const QString summs) {
        m_counter++;
        QString str = tr("Номер маршрута: ") + QString::number(m_counter) + " \"" + QString::number(1);
        foreach(int point, points) str.push_back("," + QString::number(point));
        return str + ",1\" = " + QString::number(distance) + " (" + summs + ")\n";
    };

signals:
    void route(QString);
};
