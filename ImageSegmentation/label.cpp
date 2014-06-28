#include "label.h"

label::label(QWidget *parent) :
    QLabel(parent)
{
    this->pressed = false;
    this->count = 0;
}

void label::mousePressEvent(QMouseEvent *ev)
{
    if(count != 4){
        QPoint point = ev->pos();
        if(count<2)this->foreground.append(point);
        else this->background.append(point);
        this->count = this->count+1;
        this->pressed = true;
        update();
     }
}

void label::paintEvent(QPaintEvent *e)
{
    QLabel::paintEvent(e);
    if(this->pressed){
        QPainter painter(this);
        QPen paintpen(Qt::red);

        paintpen.setWidth(4);
        painter.setPen(paintpen);
        for(int i=0; i < this->foreground.size();++i){
            painter.drawPoint(this->foreground[i]);
        }

        paintpen.setColor(Qt::blue);
        painter.setPen(paintpen);
        for(int i=0; i < this->background.size();++i){
            painter.drawPoint(this->background[i]);
        }
        this->pressed = false;
    }
}

QVector<QPoint> label::getForeground()
{
    return this->foreground;
}

QVector<QPoint> label::getBackground()
{
    return this->background;
}
