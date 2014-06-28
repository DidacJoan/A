#ifndef LABEL_H
#define LABEL_H

#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <QVector>

class label : public QLabel
{
    Q_OBJECT
public:
    explicit label(QWidget *parent = 0);

    void mousePressEvent(QMouseEvent *ev);
    void paintEvent(QPaintEvent *ev);
    QVector<QPoint> getForeground();
    QVector<QPoint> getBackground();

    QVector<QPoint> foreground;
    QVector<QPoint> background;
    int count;
    bool pressed;

signals:

public slots:


};

#endif // LABEL_H
