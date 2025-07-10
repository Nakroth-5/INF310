#ifndef GRAFOWIDGET_H
#define GRAFOWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QPoint>
#include <QMap>
#include <QVector>
#include <QFont>
#include <QFontMetrics>
#include <QPen>
#include <QBrush>
#include <QColor>
#include <QPolygon>
#include <cmath>

#include "GrafosPesados/DiGrafoPesado.h"
#include "GrafosPesados/GrafoPesado.h"

struct VerticeVisual {
    QPoint posicion;
    QString etiqueta;
    bool seleccionado;

    VerticeVisual() : seleccionado(false) {
    }

    VerticeVisual(const QPoint &pos, const QString &etiq)
        : posicion(pos), etiqueta(etiq), seleccionado(false) {
    }
};

struct AristaVisual {
    QString origen;
    QString destino;
    double peso;
    bool seleccionada;

    AristaVisual() : peso(0), seleccionada(false) {
    }

    AristaVisual(const QString &orig, const QString &dest, double p)
        : origen(orig), destino(dest), peso(p), seleccionada(false) {
    }
};

class GrafoWidget : public QWidget {
    Q_OBJECT

public:
    explicit GrafoWidget(QWidget *parent = nullptr);

    void setGrafoNoDirigido(GrafoPesado<QString> *grafo);

    void setGrafoDirigido(DiGrafoPesado<QString> *grafo);

    void setEsDirigido(bool dirigido);

    void actualizarVisualizacion();

    void limpiar();

    void seleccionarVertice(const QString &vertice);

    void seleccionarArista(const QString &origen, const QString &destino);

    void limpiarSeleccion();

    QPoint obtenerCoordenadasVertice(const QString &nombreVertice) const;

    QMap<QString, QPoint> obtenerTodasLasCoordenadas() const;

    bool establecerCoordenadasVertice(const QString &nombreVertice, const QPoint &nuevasPosicion);

    bool existeVertice(const QString &nombreVertice) const;

    VerticeVisual obtenerInfoVertice(const QString &nombreVertice) const;

    void imprimirCoordenadasVertices() const;

signals:
    void verticeSeleccionado(const QString &vertice);

    void aristaSeleccionada(const QString &origen, const QString &destino);

    void verticeMovido(const QString &vertice, const QPoint &nuevaPosicion);

protected:
    void paintEvent(QPaintEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;

    void resizeEvent(QResizeEvent *event) override;

private:
    void dibujarVertice(QPainter &painter, const VerticeVisual &vertice);

    void dibujarArista(QPainter &painter, const AristaVisual &arista);
    void dibujarInfoVuelo(QPainter &painter, const QPoint &posicion, double distancia);
    void dibujarFlechaArista(QPainter &painter, const QPoint &inicio, const QPoint &fin);

    void calcularPosicionesVertices();

    void actualizarAristas();

    QString obtenerVerticeEnPosicion(const QPoint &posicion);

    AristaVisual *obtenerAristaEnPosicion(const QPoint &posicion);

    static QPoint calcularPuntoEnCirculo(const QPoint &centro, const QPoint &objetivo, int radio);

    // Nuevo método para inicializar coordenadas
    void inicializarCoordenadasCiudades();

    // Datos del grafo
    GrafoPesado<QString> *grafoNoDirigido;
    DiGrafoPesado<QString> *grafoDirigido;
    bool esDirigido;

    // Visualización
    QMap<QString, VerticeVisual> vertices;
    QVector<AristaVisual> aristas;
    
    // Coordenadas fijas para ciudades bolivianas
    QMap<QString, QPoint> coordenadasCiudades;

    // Configuración visual
    static const int RADIO_VERTICE = 10;
    static const int GROSOR_ARISTA = 1;
    static const int TAMAÑO_FLECHA = 10;

    // Colores
    QColor colorVertice;
    QColor colorVerticeSeleccionado;
    QColor colorArista;
    QColor colorAristaSeleccionada;
    QColor colorTexto;
    QColor colorFondo;

    // Interacción
    bool arrastrando;
    QString verticeArrastrado;
    QPoint ultimaPosicionMouse;

    // Fuente
    QFont fuente;
};

#endif