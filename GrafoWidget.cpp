#include "GrafoWidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QApplication>
#include <QDebug>
#include <algorithm>
#include <QPainterPath>

GrafoWidget::GrafoWidget(QWidget *parent)
    : QWidget(parent)
      , grafoNoDirigido(nullptr)
      , grafoDirigido(nullptr)
      , esDirigido(false)
      , arrastrando(false)
      , colorVertice(70, 130, 180)
      , colorVerticeSeleccionado(255, 69, 0)
      , colorArista(200, 200, 0)
      , colorAristaSeleccionada(255, 20, 147)
      , colorTexto(255, 255, 255)
      , colorFondo(240, 240, 240) {
    setMinimumSize(600, 400);
    setMouseTracking(true);
    fuente = QFont("Arial", 10, QFont::Bold);

    // Configurar el widget
    setStyleSheet("background-color: rgb(240, 240, 240);");
    
    // Inicializar coordenadas fijas de las ciudades bolivianas
    inicializarCoordenadasCiudades();
}

void GrafoWidget::inicializarCoordenadasCiudades() {
    coordenadasCiudades = {

        {"Cobija", QPoint(225, 73)},
        {"Riberalta", QPoint(365, 70)},
        {"Rurrenabaque", QPoint(292, 253)},
        {"Trinidad", QPoint(407, 248)},
        {"Uyuni", QPoint(324, 583)},
        {"La Paz", QPoint(256, 368)},
        {"Oruro", QPoint(313, 444)},
        {"Cuzco", QPoint(60, 204)},
        {"Cochabamba", QPoint(358, 415)},
        {"Sucre", QPoint(415, 515)},
        {"Santa Cruz", QPoint(518, 428)},
        {"Camiri", QPoint(250, 420)},
        {"Tarija", QPoint(439, 646)},
        {"Yacuiba", QPoint(240, 520)},
        {"Guayaramerín", QPoint(403, 65)},
        {"Iquique", QPoint(153, 586)},
        {"Yacuiba", QPoint(492, 668)}
    };
}

void GrafoWidget::setGrafoNoDirigido(GrafoPesado<QString> *grafo) {
    grafoNoDirigido = grafo;
    esDirigido = false;
    actualizarVisualizacion();
}

void GrafoWidget::setGrafoDirigido(DiGrafoPesado<QString> *grafo) {
    grafoDirigido = grafo;
    esDirigido = true;
    actualizarVisualizacion();
}

void GrafoWidget::setEsDirigido(bool dirigido) {
    esDirigido = dirigido;
    actualizarVisualizacion();
}

void GrafoWidget::actualizarVisualizacion() {
    vertices.clear();
    aristas.clear();

    if (esDirigido && grafoDirigido) {
        // Actualizar vértices
        auto listaVertices = grafoDirigido->getVertices();
        for (const auto &vertice: listaVertices) {
            if (!vertices.contains(vertice)) {
                QPoint posicion = coordenadasCiudades.value(vertice, QPoint(0, 0));
                vertices[vertice] = VerticeVisual(posicion, vertice);
            }
        }

        // Actualizar aristas
        actualizarAristas();
    } else if (!esDirigido && grafoNoDirigido) {
        // Actualizar vértices
        auto listaVertices = grafoNoDirigido->getVertices();
        for (const auto &vertice: listaVertices) {
            if (!vertices.contains(vertice)) {
                QPoint posicion = coordenadasCiudades.value(vertice, QPoint(0, 0));
                vertices[vertice] = VerticeVisual(posicion, vertice);
            }
        }

        actualizarAristas();
    }

    // Solo calcular posiciones automáticas si no hay coordenadas predefinidas
    calcularPosicionesVertices();
    update();
}

void GrafoWidget::limpiar() {
    vertices.clear();
    aristas.clear();
    update();
}

void GrafoWidget::seleccionarVertice(const QString &vertice) {
    limpiarSeleccion();
    if (vertices.contains(vertice)) {
        vertices[vertice].seleccionado = true;
        update();
    }
}

void GrafoWidget::seleccionarArista(const QString &origen, const QString &destino) {
    limpiarSeleccion();
    for (auto &arista: aristas) {
        if (arista.origen == origen && arista.destino == destino) {
            arista.seleccionada = true;
            break;
        }
    }
    update();
}

void GrafoWidget::limpiarSeleccion() {
    for (auto &vertice: vertices) {
        vertice.seleccionado = false;
    }
    for (auto &arista: aristas) {
        arista.seleccionada = false;
    }
    update();
}

void GrafoWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);


    QPixmap fondo(":/imagenes/bolivia_mapa.png");
    if (!fondo.isNull()) {
        QPixmap fondoEscalado = fondo.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

        int x = (width() - fondoEscalado.width()) / 2;
        int y = (height() - fondoEscalado.height()) / 2;
        
        painter.drawPixmap(x, y, fondoEscalado);
    } else {
        painter.fillRect(rect(), QColor(220, 220, 220));
    }

    // Dibujar aristas primero (para que queden debajo de los vértices)
    for (const auto &arista: aristas) {
        dibujarArista(painter, arista);
    }

    // Dibujar vértices
    for (const auto &vertice: vertices) {
        dibujarVertice(painter, vertice);
    }

    // Dibujar información si no hay vértices
    if (vertices.isEmpty()) {
        painter.setPen(QPen(Qt::black, 2));
        painter.setFont(QFont("Arial", 14, QFont::Bold));
        painter.drawText(rect(), Qt::AlignCenter,
                         "Haga clic en 'Insertar Vértice' para comenzar\n"
                         "o use el ejemplo de vuelos de Bolivia");
    }
}

void GrafoWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        QString verticeClickeado = obtenerVerticeEnPosicion(event->pos());

        if (!verticeClickeado.isEmpty()) {
            // Seleccionar vértice
            seleccionarVertice(verticeClickeado);
            emit verticeSeleccionado(verticeClickeado);

            // Iniciar arrastre
            arrastrando = true;
            verticeArrastrado = verticeClickeado;
            ultimaPosicionMouse = event->pos();
        } else {
            // Verificar si se hizo clic en una arista
            AristaVisual *aristaClickeada = obtenerAristaEnPosicion(event->pos());
            if (aristaClickeada) {
                seleccionarArista(aristaClickeada->origen, aristaClickeada->destino);
                emit aristaSeleccionada(aristaClickeada->origen, aristaClickeada->destino);
            } else {
                limpiarSeleccion();
            }
        }
    }
}

void GrafoWidget::mouseMoveEvent(QMouseEvent *event) {
    if (arrastrando && !verticeArrastrado.isEmpty()) {
        QPoint delta = event->pos() - ultimaPosicionMouse;

        if (vertices.contains(verticeArrastrado)) {
            QPoint nuevaPosicion = vertices[verticeArrastrado].posicion + delta;

            // Mantener el vértice dentro de los límites
            int margen = RADIO_VERTICE;
            nuevaPosicion.setX(qBound(margen, nuevaPosicion.x(), width() - margen));
            nuevaPosicion.setY(qBound(margen, nuevaPosicion.y(), height() - margen));

            vertices[verticeArrastrado].posicion = nuevaPosicion;
            ultimaPosicionMouse = event->pos();

            update();
        }
    }
}

void GrafoWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && arrastrando) {
        arrastrando = false;

        if (!verticeArrastrado.isEmpty() && vertices.contains(verticeArrastrado)) {
            emit verticeMovido(verticeArrastrado, vertices[verticeArrastrado].posicion);
        }

        verticeArrastrado.clear();
    }
}

void GrafoWidget::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    calcularPosicionesVertices();
}

void GrafoWidget::dibujarVertice(QPainter &painter, const VerticeVisual &vertice) {
    // Configurar colores
    QColor colorRelleno = vertice.seleccionado ? colorVerticeSeleccionado : QColor(colorVertice);
    QColor colorBorde = colorRelleno.darker(150);

    painter.setPen(QPen(colorBorde, 3));
    painter.setBrush(QBrush(colorRelleno));
    painter.drawEllipse(vertice.posicion, RADIO_VERTICE, RADIO_VERTICE);

    // Dibujar texto con fondo para mejor legibilidad
    painter.setPen(QPen(Qt::black));
    painter.setFont(QFont("Arial", 8, QFont::Bold));

    // Calcular posición del texto (debajo del círculo)
    QPoint textPos = vertice.posicion + QPoint(-30, RADIO_VERTICE + 15);
    
    // Dibujar fondo para el texto
    QFontMetrics fm(painter.font());
    QRect textRect = fm.boundingRect(vertice.etiqueta);
    textRect.moveCenter(QPoint(textPos.x() + 30, textPos.y()));
    textRect.adjust(-3, -1, 3, 1);
    
    painter.setBrush(QBrush(QColor(255, 255, 255, 200)));
    painter.setPen(QPen(Qt::black, 1));
    painter.drawRoundedRect(textRect, 3, 3);
    
    // Dibujar el texto
    painter.setPen(QPen(Qt::black));
    painter.drawText(textRect, Qt::AlignCenter, vertice.etiqueta);
}

void GrafoWidget::dibujarArista(QPainter &painter, const AristaVisual &arista) {
    if (!vertices.contains(arista.origen) || !vertices.contains(arista.destino)) {
        return;
    }

    const VerticeVisual &verticeOrigen = vertices[arista.origen];
    const VerticeVisual &verticeDestino = vertices[arista.destino];

    // Calcular puntos de inicio y fin en el borde de los círculos
    QPoint puntoInicio = calcularPuntoEnCirculo(verticeOrigen.posicion, verticeDestino.posicion, RADIO_VERTICE);
    QPoint puntoFin = calcularPuntoEnCirculo(verticeDestino.posicion, verticeOrigen.posicion, RADIO_VERTICE);

    // Configurar estilo de la línea según selección
    QColor colorLinea = arista.seleccionada ? colorAristaSeleccionada : QColor(colorArista); // Azul para rutas
    QPen penLinea(colorLinea, GROSOR_ARISTA + 1);

    if (esDirigido) {
        // Verificar si existe la ruta inversa
        bool tieneVuelta = false;
        for (const auto &a : aristas) {
            if (a.origen == arista.destino && a.destino == arista.origen) {
                tieneVuelta = true;
                break;
            }
        }

        if (tieneVuelta) {
            // Dibujar línea curva para vuelos de ida y vuelta
            QPainterPath path;
            QPoint ctrlPt1, ctrlPt2;

            // Calcular puntos de control para la curva
            QLineF line(puntoInicio, puntoFin);
            QPointF perpendicular = line.normalVector().unitVector().p2() - line.p1();
            int offset = 30; // Separación de la curva

            ctrlPt1 = puntoInicio + QPoint(perpendicular.x() * offset, perpendicular.y() * offset);
            ctrlPt2 = puntoFin + QPoint(perpendicular.x() * offset, perpendicular.y() * offset);

            path.moveTo(puntoInicio);
            path.cubicTo(ctrlPt1, ctrlPt2, puntoFin);

            painter.setPen(penLinea);
            painter.drawPath(path);

            // Dibujar flecha en el punto medio de la curva
            QPointF puntoMedioCurva = path.pointAtPercent(0.5);
            QPointF puntoFlecha = path.pointAtPercent(0.6);
            dibujarFlechaArista(painter, puntoMedioCurva.toPoint(), puntoFlecha.toPoint());

            // Posición del texto en la curva
            QPointF puntoTexto = path.pointAtPercent(0.3);
            dibujarInfoVuelo(painter, puntoTexto.toPoint(), arista.peso);
        } else {
            // Vuelo unidireccional - línea recta con flecha
            painter.setPen(penLinea);
            painter.drawLine(puntoInicio, puntoFin);
            dibujarFlechaArista(painter, puntoInicio, puntoFin);
            dibujarInfoVuelo(painter, QPoint((puntoInicio.x() + puntoFin.x()) / 2,
                                          (puntoInicio.y() + puntoFin.y()) / 2),
                                          arista.peso);
        }
    } else {
        // Grafo no dirigido - línea recta simple
        painter.setPen(penLinea);
        painter.drawLine(puntoInicio, puntoFin);
        dibujarInfoVuelo(painter, QPoint((puntoInicio.x() + puntoFin.x()) / 2,
                                      (puntoInicio.y() + puntoFin.y()) / 2),
                                      arista.peso);
    }
}

void GrafoWidget::dibujarInfoVuelo(QPainter &painter, const QPoint &posicion, double distancia) {
    painter.save();

    // Configurar fuente y color
    QFont font("Arial", 8, QFont::Bold);
    painter.setFont(font);
    painter.setPen(QPen(Qt::black));

    // Texto a mostrar (distancia en km)
    /*QString texto = QString("%1 km").arg(distancia, 0, 'f', 0);

    // Calcular tamaño del texto
    QFontMetrics fm(font);
    QRect textRect = fm.boundingRect(texto);
    textRect.moveCenter(posicion);

    // Añadir margen al rectángulo del texto
    int margin = 4;
    textRect.adjust(-margin, -margin, margin, margin);

    // Dibujar fondo y texto con mejor visibilidad
    painter.setBrush(QBrush(QColor(255, 255, 255, 230)));
    painter.setPen(QPen(Qt::black, 1));
    painter.drawRoundedRect(textRect, 4, 4);
    painter.drawText(textRect, Qt::AlignCenter, texto);
*/
    painter.restore();
}

void GrafoWidget::dibujarFlechaArista(QPainter &painter, const QPoint &inicio, const QPoint &fin) {
    if (!esDirigido) return;

    painter.save();

    // Calcular la dirección de la flecha
    QLineF line(fin, inicio);
    double angle = std::atan2(-line.dy(), line.dx());

    // Tamaño de la flecha
    int arrowSize = TAMAÑO_FLECHA;
    QPointF arrowP1 = fin + QPointF(sin(angle + M_PI / 3) * arrowSize,
                                    cos(angle + M_PI / 3) * arrowSize);
    QPointF arrowP2 = fin + QPointF(sin(angle + M_PI - M_PI / 3) * arrowSize,
                                    cos(angle + M_PI - M_PI / 3) * arrowSize);

    // Dibujar la flecha
    QPolygonF arrowHead;
    arrowHead << fin << arrowP1 << arrowP2;

    painter.setBrush(QBrush(painter.pen().color()));
    painter.drawPolygon(arrowHead);

    painter.restore();
}

void GrafoWidget::calcularPosicionesVertices() {
    if (vertices.isEmpty()) return;

    int numVertices = vertices.size();
    QPoint centro(width() / 2, height() / 2);


    for (auto &vertice: vertices) {
        if (vertice.posicion == QPoint(0, 0)) {

            if (numVertices == 1) {
                vertice.posicion = centro;
            } else {
                // Distribución circular para vértices sin coordenadas fijas
                static int contador = 0;
                int radioCirculo = qMin(width(), height()) / 3;
                if (radioCirculo < 50) radioCirculo = 50;
                
                double angulo = 2 * M_PI * contador / numVertices;
                int x = centro.x() + static_cast<int>(radioCirculo * cos(angulo));
                int y = centro.y() + static_cast<int>(radioCirculo * sin(angulo));
                
                vertice.posicion = QPoint(x, y);
                contador++;
            }
        }
    }
}

void GrafoWidget::actualizarAristas() {
    aristas.clear();

    if (esDirigido && grafoDirigido) {
        for (const auto &origen: grafoDirigido->getVertices()) {
            auto adyacentes = grafoDirigido->getAdyacentesDelVerticeConPeso(origen);
            for (const auto &ady: adyacentes) {
                QString destino = grafoDirigido->getVertice(ady.indiceVertice);
                aristas.append(AristaVisual(origen, destino, ady.distancia));
            }
        }
    } else if (!esDirigido && grafoNoDirigido) {
        std::set<std::pair<QString, QString> > aristasYaAgregadas;

        for (const auto &origen: grafoNoDirigido->getVertices()) {
            auto adyacentes = grafoNoDirigido->getAdyacentesDelVerticeConPeso(origen);
            for (const auto &ady: adyacentes) {
                QString destino = grafoNoDirigido->getVertice(ady.indiceVertice);

                std::pair arista = {origen, destino};
                std::pair aristaInversa = {destino, origen};

                if (aristasYaAgregadas.find(arista) == aristasYaAgregadas.end() &&
                    aristasYaAgregadas.find(aristaInversa) == aristasYaAgregadas.end()) {
                    aristas.append(AristaVisual(origen, destino, ady.distancia));
                    aristasYaAgregadas.insert(arista);
                }
            }
        }
    }
}

QString GrafoWidget::obtenerVerticeEnPosicion(const QPoint &posicion) {
    for (auto it = vertices.begin(); it != vertices.end(); ++it) {
        QPoint centro = it->posicion;
        int distancia = static_cast<int>(sqrt(pow(posicion.x() - centro.x(), 2) +
                                              pow(posicion.y() - centro.y(), 2)));

        if (distancia <= RADIO_VERTICE) {
            return it.key();
        }
    }
    return QString();
}

AristaVisual *GrafoWidget::obtenerAristaEnPosicion(const QPoint &posicion) {
    for (auto &arista: aristas) {
        if (!vertices.contains(arista.origen) || !vertices.contains(arista.destino)) {
            continue;
        }

        const VerticeVisual &verticeOrigen = vertices[arista.origen];
        const VerticeVisual &verticeDestino = vertices[arista.destino];

        // Calcular distancia del punto a la línea
        QPoint p1 = verticeOrigen.posicion;
        QPoint p2 = verticeDestino.posicion;
        QPoint p = posicion;

        // Fórmula de distancia de punto a línea
        double A = p.x() - p1.x();
        double B = p.y() - p1.y();
        double C = p2.x() - p1.x();
        double D = p2.y() - p1.y();

        double dot = A * C + B * D;
        double lenSq = C * C + D * D;

        if (lenSq == 0) continue;

        double param = dot / lenSq;

        double xx, yy;
        if (param < 0) {
            xx = p1.x();
            yy = p1.y();
        } else if (param > 1) {
            xx = p2.x();
            yy = p2.y();
        } else {
            xx = p1.x() + param * C;
            yy = p1.y() + param * D;
        }

        double dx = p.x() - xx;
        double dy = p.y() - yy;
        double distancia = sqrt(dx * dx + dy * dy);

        if (distancia <= 8) { // Aumentar área de click
            return &arista;
        }
    }
    return nullptr;
}

QPoint GrafoWidget::calcularPuntoEnCirculo(const QPoint &centro, const QPoint &objetivo, int radio) {
    QVector2D direccion(objetivo.x() - centro.x(), objetivo.y() - centro.y());
    direccion.normalize();

    return QPoint(centro.x() + static_cast<int>(direccion.x() * radio),
                  centro.y() + static_cast<int>(direccion.y() * radio));
}

QPoint GrafoWidget::obtenerCoordenadasVertice(const QString &nombreVertice) const {
    if (vertices.contains(nombreVertice)) {
        return vertices[nombreVertice].posicion;
    }

    return QPoint(-1, -1);
}

QMap<QString, QPoint> GrafoWidget::obtenerTodasLasCoordenadas() const {
    QMap<QString, QPoint> coordenadas;

    for (auto it = vertices.begin(); it != vertices.end(); ++it) {
        coordenadas[it.key()] = it.value().posicion;
    }

    return coordenadas;
}

bool GrafoWidget::establecerCoordenadasVertice(const QString &nombreVertice, const QPoint &nuevasPosicion) {
    if (vertices.contains(nombreVertice)) {
        int margen = RADIO_VERTICE;
        QPoint posicionValida = nuevasPosicion;

        posicionValida.setX(qBound(margen, posicionValida.x(), width() - margen));
        posicionValida.setY(qBound(margen, posicionValida.y(), height() - margen));

        vertices[nombreVertice].posicion = posicionValida;
        update();

        return true;
    }

    return false;
}

bool GrafoWidget::existeVertice(const QString &nombreVertice) const {
    return vertices.contains(nombreVertice);
}


VerticeVisual GrafoWidget::obtenerInfoVertice(const QString &nombreVertice) const {
    if (vertices.contains(nombreVertice)) {
        return vertices[nombreVertice];
    }

    return VerticeVisual();|
}

void GrafoWidget::imprimirCoordenadasVertices() const {
    qDebug() << "=== Coordenadas actuales de los vértices ===";

    for (auto it = vertices.begin(); it != vertices.end(); ++it) {
        qDebug() << QString("Vértice '%1': (%2, %3)")
                    .arg(it.key())
                    .arg(it.value().posicion.x())
                    .arg(it.value().posicion.y());
    }

    qDebug() << "============================================";
}