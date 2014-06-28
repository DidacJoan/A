
#include "graph.h"
#include "label.h"

#include <cmath>
#include <limits>
#include <iostream>

#include <QFileDialog>
#include <QtGui/QApplication>
#include <QImage>
#include <QVector>
#include <QPoint>

using namespace std;

#define  SOURCE        Graph<int,int,int>::SOURCE
#define  SINK          Graph<int,int,int>::SINK
#define  CONST         170
#define  INF           std::numeric_limits<int>::max()

int width,height;
int average = 0;

// Invierte el color de los pixeles de toda la imagen
QImage invert(QImage img){

    QImage invert = img;
    for (int i = 0; i < img.width(); ++i) {
        for (int j = 0; j < img.height(); ++j) {
            QRgb color = img.pixel(i,j);
            color = 255 - color;
            invert.setPixel(i,j,color);
        }
    }
    // Guardar imagen en directorio actual
    invert.save("inverted.png");
    return invert;
}

// Pasa la imagen a escala de grises para facilitar la segmentación
QImage greyScale(QImage colorImg) {

    QImage greyImg = colorImg;

    for (int i = 0; i < greyImg.width(); ++i) {
        for (int j = 0; j < greyImg.height(); ++j) {
            QRgb color = greyImg.pixel(i,j);
            int r = qRed(color);
            int g = qGreen(color);
            int b = qBlue(color);
            int grey=(r * 11 + g * 16 + b * 5)/32;
            greyImg.setPixel(i,j,qRgb(grey, grey, grey));
        }
    }

    // Guardar imagen en directorio actual
    greyImg.save("GreyScale.png");
    return greyImg;
}

// Devuelve la intensidad del pixel
int getGreyValue(QImage img,int x, int y){
    QRgb grey = img.pixel(x,y);
    return qRed(grey);
}

// Normaliza el valor [0..255]
int normalizeValue(int value) {
    int newValue;
    float scaleFactor;
    scaleFactor = CONST/average;
    newValue = value*scaleFactor;
    if(newValue > 255) newValue = 255;
    return newValue;
}

// Devuelve el peso del pixel indicado
int getWeight(QImage img,int x, int y) {
    int value = getGreyValue(img,x,y);
    return normalizeValue(value);
}

// Calcula la media aritmetica de la intensidad
// de los píxeles de la imagen
void computeAverage(QImage img){
    int sum, count;
    sum = count = 0;
    for(int y = 0; y < height; ++y) {
        for(int x = 0; x < width; ++x) {
            sum += getGreyValue(img,x,y);
            ++count;
        }
    }
    average = sum/count;
}

// Devuelve el peso de una arista entre píxeles vecinos
// A mayor diferencia menor peso
int computeDifference(int value1, int value2){
    return abs(255-abs(value1-value2));
}

// Devuelve la suma de intensidades de una fila
int countRow(QImage img, int fila) {
    int sum = 0;
    for(int j = 0; j < width-1; ++j){
        sum+= getGreyValue(img,j,fila);
    }
    return sum;
}

// Devuelve la suma de intensidades de una columna
int countCol(QImage img, int col) {
    int sum = 0;
    for(int i = 0; i < height-1; ++i){
        sum+= getGreyValue(img,col,i);
    }
    return sum;
}

// Comprueba si el fondo es claro(True) o oscuro(False)
bool checkBackground(QImage img){

    int fil1,col1,fil2,col2,number,avg;

    number = 2*(width-1)+2*(height-1);
    fil1 = countRow(img,0);
    fil2 = countRow(img,height-1);
    col1 = countCol(img,0);
    col2 = countCol(img,width-1);

    avg = (fil1+fil2+col1+col2)/number;
    if(avg>(255/2))	return true;
    return false;
}

// Comprueba que id no este en el vector indicado,
// se usa para no añadir aristas de mas a los
// píxeles seleccionados por el usuario
bool notInVector(int id, vector<int> idVec) {
    for(uint i = 0; i < idVec.size(); ++i) {
        if(idVec[i]==id) return false;
    }
    return true;
}

int main(int argc, char* argv[]) {

    QApplication app(argc,argv);
    QImage img;
    label myLabel;

    // Preguntar directorio donde se encuentra la imagen a analizar
    QString fileName = QFileDialog::getOpenFileName(NULL, "Open File", "/home", "Images (*.png *.jpg)");
    if (fileName.isNull()) return -1;

    // Asignar imagen abierta a la QImage con la que trabajar
    img = QImage(fileName);

    // Label para que el usuario indique los píxeles que desea marcar
    myLabel.setPixmap(QPixmap::fromImage(img));
    myLabel.show();

    // Ejecución de la aplicación Qt,
    // al cerrar todas las ventanas se sigue con la ejecución
    app.exec();

    width = img.width();
    height = img.height();

    cout << "Width:  " << width << endl;
    cout << "Height: " << height << endl;

    img = greyScale(img);

    // Si el fondo es claro se invierte la imagen
    if(checkBackground(img)) img = invert(img);

    computeAverage(img);

    int numPixels = width*height;
    // Vertices = Pixels, Aristas = una por cada vecino(4 por nodo)
    Graph<int, int, int> graph (numPixels, 4*numPixels);
    graph.add_node(numPixels);

    // Se añaden las aristas a SOURCE y SINK desde los píxeles
    // seleccionados por el usuario
    QVector<QPoint> foreground = myLabel.getForeground();
    QVector<QPoint> background = myLabel.getBackground();

    vector<int> id_entered(foreground.size()+background.size());
    int count = 0;
    
    for(int i = 0; i < foreground.size(); ++i){
        int pixel_id = foreground[i].y()*width+foreground[i].x();
        id_entered[count] = pixel_id;
        ++count;
        graph.add_tweights(pixel_id, INF, 0);
    }

    for(int i = 0; i < background.size(); ++i){
       int pixel_id = background[i].y()*width+background[i].x();
       id_entered[count] = pixel_id;
       ++count;
       graph.add_tweights(pixel_id, 0, INF);
    }

    //     0  1  2  . .
    //  0 [1][2][3] . .
    //  1 [4][5][6] . .
    //  2 [7][8][9] . .
    //  .  .  .  .
    //  .  .  .  .
    // Formula para acceder dada una fila i y una columna j: i*width + j

    // Primero añadimos aristas para todos los nodos menos la ultima fila y
    // la ultima columna, ya que se expande hacia abajo y hacia la derecha
    for(int i = 0; i < height-1; ++i) {
        for(int j = 0; j < width-1; ++j) {

            int pixel_id, pixel_weight, neighbour_weight1, neighbour_weight2, edge_weight1, edge_weight2;

            pixel_id = i*(width)+j;
            
            pixel_weight = getWeight(img,j,i);

            neighbour_weight1 = getWeight(img,j+1,i);
            neighbour_weight2 = getWeight(img,j,i+1);

            edge_weight1 = computeDifference(neighbour_weight1,pixel_weight);
            edge_weight2 = computeDifference(neighbour_weight2,pixel_weight);

            // añadimos aristas a los nodos vecinos de la derecha y abajo, las otras dos ya estan añadidas
            graph.add_edge(pixel_id, pixel_id+1,     edge_weight1, edge_weight1);
            graph.add_edge(pixel_id, pixel_id+width, edge_weight2, edge_weight2);

            // añadimos una arista al nodo source y una al nodo sink
            if(notInVector(pixel_id,id_entered)){    
                graph.add_tweights(pixel_id, (pixel_weight), (255-pixel_weight));
            }
        }
    }

    // Se añaden las aristas de la ultima fila
    for(int j = 0; j < width-1; ++j) {
        int fila, pixel_id, pixel_weight, neighbour_weight, edge_weight;

        fila = (height-1);
        pixel_id = fila*(width)+j;
        
        pixel_weight = getWeight(img,j,fila);

        neighbour_weight = getWeight(img,j+1,fila);

        edge_weight = computeDifference(neighbour_weight,pixel_weight);

        // añadimos aristas a los nodos vecinos de la derecha
        graph.add_edge(pixel_id, pixel_id+1, edge_weight, edge_weight);

        // añadimos una arista al nodo source y una al nodo sink
        if(notInVector(pixel_id,id_entered)) {
            graph.add_tweights(pixel_id, (pixel_weight), (255-pixel_weight));
        }
    }

    // Se añaden las aristas de la ultima columna
    for(int i = 0; i < height-1; ++i) {
        int columna, pixel_id, pixel_weight, neighbour_weight, edge_weight;

        columna = (width-1);
        pixel_id = i*(width)+columna;
        pixel_weight = getWeight(img,columna,i);

        neighbour_weight = getWeight(img,columna,i+1);

        edge_weight = computeDifference(neighbour_weight,pixel_weight);

        // añadimos aristas a los nodos vecinos de abajo
        graph.add_edge(pixel_id, pixel_id+width, edge_weight, edge_weight);


        // añadimos una arista al nodo source y una al nodo sink
        if(notInVector(pixel_id,id_entered)){
            graph.add_tweights(pixel_id, (pixel_weight), (255-pixel_weight));
        }
    }

    if(notInVector(numPixels-1,id_entered)){
        // añadimos las aristas del nodo restante (pixel_id = numPixels-1) con el source y el sink
        int pixel_weight = getWeight(img,width-1, height-1);
        graph.add_tweights(numPixels-1, (pixel_weight), (255-pixel_weight));
    }

    int maxflow = graph.maxflow();
    cout << "Maxflow: " << maxflow << endl;

    // Por todo pixel se le asigna un color dependiendo de si pertenece
    // al foreground(negro) o al backround(blanco)
    for(int y = 0; y < height; ++y) {
        for(int x = 0; x < width; ++x) {
            int pixel_id = y*(width)+x;
            if(graph.what_segment(pixel_id)==SOURCE) img.setPixel(x, y, qRgb(0,0,0));
            else img.setPixel(x, y, qRgb(255,255,255));
        }
    }

    // Guardamos el resultado
    img.save("resultado.png");

}
