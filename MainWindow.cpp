#include "MainWindow.h"
#include <QApplication>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QFileDialog>
#include <QColorDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QLabel>
#include <QStringList>
#include <QScrollArea>

#include "GrafosPesados/utils/FloydWarshall.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
      , centralWidget(nullptr)
      , splitter(nullptr)
      , grafoWidget(nullptr)
      , controlPanel(nullptr)
      , grafoNoDirigido(nullptr)
      , grafoDirigido(nullptr)
      , esDirigido(false) {

    grafoNoDirigido = new GrafoPesado<QString>();
    grafoDirigido = new DiGrafoPesado<QString>();

    setupUI();
    setupMenus();
    setupStatusBar();
    connectSignals();
    configurarEjemploVuelos();

    // Configurar ventana
    setWindowTitle("Editor de Grafos Pesados");
    setMinimumSize(1200, 800);
    resize(1400, 900);

    // Actualizar estado inicial
    cambiarTipoGrafo();
}

MainWindow::~MainWindow() {
    delete grafoNoDirigido;
    delete grafoDirigido;
}

void MainWindow::setupUI() {
    centralWidget = new QWidget();
    setCentralWidget(centralWidget);

    // Crear splitter principal
    splitter = new QSplitter(Qt::Horizontal);

    // Crear widget de visualización
    grafoWidget = new GrafoWidget();
    grafoWidget->setMinimumSize(600, 400);

    // Crear panel de control con scroll
    QScrollArea *scrollArea = new QScrollArea();
    controlPanel = new QWidget();
    controlPanel->setMinimumWidth(400);
    controlPanel->setMaximumWidth(500);

    scrollArea->setWidget(controlPanel);
    scrollArea->setWidgetResizable(true);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    // Layout del panel de control
    QVBoxLayout *controlLayout = new QVBoxLayout(controlPanel);

    // Grupo tipo de grafo
    tipoGrafoGroup = new QGroupBox("Tipo de Grafo");
    QVBoxLayout *tipoLayout = new QVBoxLayout(tipoGrafoGroup);
    esDirigidoCheckBox = new QCheckBox("Grafo Dirigido");
    tipoLayout->addWidget(esDirigidoCheckBox);
    controlLayout->addWidget(tipoGrafoGroup);

    // Grupo vértices
    verticesGroup = new QGroupBox("Vértices");
    QVBoxLayout *verticesLayout = new QVBoxLayout(verticesGroup);

    QLabel *verticeLabel = new QLabel("Nombre del vértice:");
    verticeLineEdit = new QLineEdit();
    verticeLineEdit->setPlaceholderText("Ej: A, B, C...");

    QHBoxLayout *verticeButtonsLayout = new QHBoxLayout();
    insertarVerticeBtn = new QPushButton("Insertar");
    eliminarVerticeBtn = new QPushButton("Eliminar");
    verticeButtonsLayout->addWidget(insertarVerticeBtn);
    verticeButtonsLayout->addWidget(eliminarVerticeBtn);

    verticesLayout->addWidget(verticeLabel);
    verticesLayout->addWidget(verticeLineEdit);
    verticesLayout->addLayout(verticeButtonsLayout);
    controlLayout->addWidget(verticesGroup);

    // Grupo aristas
    aristasGroup = new QGroupBox("Aristas");
    QVBoxLayout *aristasLayout = new QVBoxLayout(aristasGroup);

    QLabel *origenLabel = new QLabel("Vértice origen:");
    origenCombo = new QComboBox();
    origenCombo->setEditable(false);

    QLabel *destinoLabel = new QLabel("Vértice destino:");
    destinoCombo = new QComboBox();
    destinoCombo->setEditable(false);

    QLabel *pesoLabel = new QLabel("Distancia:");
    distanciaSpinBox = new QDoubleSpinBox();
    distanciaSpinBox->setRange(0.0, 9999.0);
    distanciaSpinBox->setDecimals(2);
    distanciaSpinBox->setValue(1.0);

    QLabel *tiempoLabel = new QLabel("Tiempo:");
    tiempoSpinBox = new QDoubleSpinBox();
    tiempoSpinBox->setRange(0, 9999.0);
    tiempoSpinBox->setDecimals(2);
    tiempoSpinBox->setValue(0.0);

    QHBoxLayout *aristaButtonsLayout = new QHBoxLayout();
    insertarAristaBtn = new QPushButton("Insertar");
    eliminarAristaBtn = new QPushButton("Eliminar");
    aristaButtonsLayout->addWidget(insertarAristaBtn);
    aristaButtonsLayout->addWidget(eliminarAristaBtn);

    actualizarPesoBtn = new QPushButton("Actualizar Peso");

    aristasLayout->addWidget(origenLabel);
    aristasLayout->addWidget(origenCombo);
    aristasLayout->addWidget(destinoLabel);
    aristasLayout->addWidget(destinoCombo);
    aristasLayout->addWidget(pesoLabel);
    aristasLayout->addWidget(distanciaSpinBox);
    aristasLayout->addWidget(tiempoLabel);
    aristasLayout->addWidget(tiempoSpinBox);
    aristasLayout->addLayout(aristaButtonsLayout);
    aristasLayout->addWidget(actualizarPesoBtn);
    controlLayout->addWidget(aristasGroup);

    // Grupo algoritmos
    algoritmosGroup = new QGroupBox("Algoritmos de Caminos");
    QVBoxLayout *algoritmosLayout = new QVBoxLayout(algoritmosGroup);

    dijkstraBtn = new QPushButton("Dijkstra (Camino Más Corto)");
    dijkstraBtn->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; font-weight: bold; }");

    floydWarshallBtn = new QPushButton("Floyd-Warshall (Todos los Caminos)");
    floydWarshallBtn->setStyleSheet("QPushButton { background-color: #2196F3; color: white; font-weight: bold; }");

    todosCaminosDijkstraBtn = new QPushButton("Dijkstra desde Origen");
    todosCaminosDijkstraBtn->setStyleSheet("QPushButton { background-color: #FF9800; color: white; font-weight: bold; }");

    algoritmosLayout->addWidget(dijkstraBtn);
    algoritmosLayout->addWidget(floydWarshallBtn);
    algoritmosLayout->addWidget(todosCaminosDijkstraBtn);
    controlLayout->addWidget(algoritmosGroup);

    // Grupo información
    infoGroup = new QGroupBox("Información y Resultados");
    QVBoxLayout *infoLayout = new QVBoxLayout(infoGroup);

    infoTextEdit = new QTextEdit();
    infoTextEdit->setReadOnly(true);
    infoTextEdit->setMinimumHeight(300);
    infoTextEdit->setFont(QFont("Courier New", 9));

    QHBoxLayout *infoButtonsLayout = new QHBoxLayout();
    mostrarInfoBtn = new QPushButton("Mostrar Info");
    limpiarGrafoBtn = new QPushButton("Limpiar Grafo");
    infoButtonsLayout->addWidget(mostrarInfoBtn);
    infoButtonsLayout->addWidget(limpiarGrafoBtn);

    infoLayout->addWidget(infoTextEdit);
    infoLayout->addLayout(infoButtonsLayout);
    controlLayout->addWidget(infoGroup);

    // Agregar espaciador
    controlLayout->addStretch();

    // Agregar widgets al splitter
    splitter->addWidget(grafoWidget);
    splitter->addWidget(scrollArea);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 0);

    // Layout principal
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->addWidget(splitter);
    mainLayout->setContentsMargins(5, 5, 5, 5);
}

void MainWindow::setupMenus() {
    // Menú Archivo
    QMenu *archivoMenu = menuBar()->addMenu("&Archivo");

    QAction *salirAction = archivoMenu->addAction("&Salir");
    salirAction->setShortcut(QKeySequence::Quit);
    connect(salirAction, &QAction::triggered, this, &QWidget::close);

    // Menú Algoritmos
    QMenu *algoritmosMenu = menuBar()->addMenu("&Algoritmos");

    QAction *dijkstraAction = algoritmosMenu->addAction("&Dijkstra");
    dijkstraAction->setShortcut(QKeySequence("Ctrl+D"));
    connect(dijkstraAction, &QAction::triggered, this, &MainWindow::calcularRutaMasCorta);

    QAction *floydAction = algoritmosMenu->addAction("&Floyd-Warshall");
    floydAction->setShortcut(QKeySequence("Ctrl+F"));
    connect(floydAction, &QAction::triggered, this, &MainWindow::calcularFloydWarshall);

    // Menú Ayuda
    QMenu *ayudaMenu = menuBar()->addMenu("&Ayuda");

    QAction *acercaDeAction = ayudaMenu->addAction("&Acerca de");
    connect(acercaDeAction, &QAction::triggered, [this]() {
        QMessageBox::about(this, "Acerca de",
                           "Editor de Grafos Pesados v2.0\n\n"
                           "Una aplicación para crear y manipular grafos pesados\n"
                           "tanto dirigidos como no dirigidos.\n\n"
                           "Incluye algoritmos de Dijkstra y Floyd-Warshall\n"
                           "para encontrar caminos más cortos.\n\n"
                           "Desarrollado con Qt y C++");
    });
}

void MainWindow::setupStatusBar() const {
    statusBar()->showMessage("Listo");
}

void MainWindow::connectSignals() {
    connect(insertarVerticeBtn, &QPushButton::clicked, this, &MainWindow::insertarVertice);
    connect(eliminarVerticeBtn, &QPushButton::clicked, this, &MainWindow::eliminarVertice);
    connect(insertarAristaBtn, &QPushButton::clicked, this, &MainWindow::insertarArista);
    connect(eliminarAristaBtn, &QPushButton::clicked, this, &MainWindow::eliminarArista);
    connect(actualizarPesoBtn, &QPushButton::clicked, this, &MainWindow::actualizarPeso);
    connect(mostrarInfoBtn, &QPushButton::clicked, this, &MainWindow::mostrarInformacion);
    connect(limpiarGrafoBtn, &QPushButton::clicked, this, &MainWindow::limpiarGrafo);

    connect(dijkstraBtn, &QPushButton::clicked, this, &MainWindow::calcularRutaMasCorta);
    connect(floydWarshallBtn, &QPushButton::clicked, this, &MainWindow::calcularFloydWarshall);
    connect(todosCaminosDijkstraBtn, &QPushButton::clicked, [this]() {
        QString origen = origenCombo->currentText();
        if (!origen.isEmpty()) {
            mostrarTodosLosCaminosDijkstra(origen);
        } else {
            mostrarMensaje("Seleccione un vértice de origen", true);
        }
    });

    connect(esDirigidoCheckBox, &QCheckBox::toggled, this, &MainWindow::cambiarTipoGrafo);

    connect(verticeLineEdit, &QLineEdit::returnPressed, this, &MainWindow::insertarVertice);

    connect(grafoWidget, &GrafoWidget::verticeSeleccionado, this, &MainWindow::onVerticeSeleccionado);
    connect(grafoWidget, &GrafoWidget::aristaSeleccionada, this, &MainWindow::onAristaSeleccionada);
}


void MainWindow::insertarVertice() {
    QString vertices = verticeLineEdit->text().trimmed();
    if (vertices.isEmpty()) {
        mostrarMensaje("Por favor, ingrese un nombre para el vértice.", true);
        return;
    }

    try {
        for (auto listaVertices = vertices.split(","); const auto &vertice: listaVertices) {
            if (esDirigido)
                grafoDirigido->insertarVertice(vertice.trimmed());
            else
                grafoNoDirigido->insertarVertice(vertice.trimmed());
        }


        verticeLineEdit->clear();
        actualizarVisualizacion();
        mostrarMensaje("Vértice '" + vertices + "' insertado correctamente.");
    } catch (const std::exception &e) {
        mostrarMensaje("Error al insertar vértice: " + QString(e.what()), true);
    }
}

void MainWindow::eliminarVertice() {
    QString vertices = verticeLineEdit->text().trimmed();
    if (vertices.isEmpty()) {
        mostrarMensaje("Por favor, ingrese el nombre del vértice a eliminar.", true);
        return;
    }

    try {
        auto listaVertices = vertices.split(",");
        for (const auto &vertice: listaVertices) {
            if (esDirigido)
                grafoDirigido->eliminarVertice(vertice);
            else
                grafoNoDirigido->eliminarVertice(vertice);
        }
        verticeLineEdit->clear();
        actualizarVisualizacion();
        mostrarMensaje("Vértice '" + vertices + "' eliminado correctamente.");
    } catch (const std::exception &e) {
        mostrarMensaje("Error al eliminar vértice: " + QString(e.what()), true);
    }
}

void MainWindow::insertarArista() {
    QString origen = origenCombo->currentText();
    QString destino = destinoCombo->currentText();
    double distancia = distanciaSpinBox->value();
    double tiempo = tiempoSpinBox->value();

    if (origen.isEmpty() || destino.isEmpty()) {
        mostrarMensaje("Por favor, seleccione vértices de origen y destino.", true);
        return;
    }

    try {
        if (esDirigido)
            grafoDirigido->insertarArista(origen, destino, distancia, tiempo);
        else
            grafoNoDirigido->insertarArista(origen, destino, distancia, tiempo);

        actualizarVisualizacion();
        mostrarMensaje("Arista insertada correctamente.");
    } catch (const std::exception &e) {
        mostrarMensaje("Error al insertar arista: " + QString(e.what()), true);
    }
}

void MainWindow::eliminarArista() {
    QString origen = origenCombo->currentText();
    QString destino = destinoCombo->currentText();

    if (origen.isEmpty() || destino.isEmpty()) {
        mostrarMensaje("Por favor, seleccione vértices de origen y destino.", true);
        return;
    }

    try {
        if (esDirigido)
            grafoDirigido->eliminarArista(origen, destino);
        else
            grafoNoDirigido->eliminarArista(origen, destino);

        actualizarVisualizacion();
        mostrarMensaje("Arista eliminada correctamente.");
    } catch (const std::exception &e) {
        mostrarMensaje("Error al eliminar arista: " + QString(e.what()), true);
    }
}

void MainWindow::actualizarPeso() {
    QString origen = origenCombo->currentText();
    QString destino = destinoCombo->currentText();
    double peso = distanciaSpinBox->value();
    double tiempo = tiempoSpinBox->value();

    if (origen.isEmpty() || destino.isEmpty()) {
        mostrarMensaje("Por favor, seleccione vértices de origen y destino.", true);
        return;
    }

    try {
        if (esDirigido)
            grafoDirigido->actualizarPesoArista(origen, destino, peso, tiempo);
        else
            grafoNoDirigido->actualizarPesoArista(origen, destino, peso, tiempo);

        actualizarVisualizacion();
        mostrarMensaje("Peso actualizado correctamente.");
    } catch (const std::exception &e) {
        mostrarMensaje("Error al actualizar peso: " + QString(e.what()), true);
    }
}

void MainWindow::limpiarGrafo() {
    grafoNoDirigido = new GrafoPesado<QString>();
    grafoDirigido = new DiGrafoPesado<QString>();

    actualizarVisualizacion();
    infoTextEdit->clear();
    mostrarMensaje("Grafo limpiado correctamente.");
}


void MainWindow::cambiarTipoGrafo() {
    esDirigido = esDirigidoCheckBox->isChecked();

    if (esDirigido) {
        grafoWidget->setGrafoDirigido(grafoDirigido);
    } else {
        grafoWidget->setGrafoNoDirigido(grafoNoDirigido);
    }

    grafoWidget->setEsDirigido(esDirigido);
    actualizarVisualizacion();
}

void MainWindow::actualizarVisualizacion() const {
    grafoWidget->actualizarVisualizacion();

    origenCombo->clear();
    destinoCombo->clear();

    if (esDirigido) {
        for (const auto &vertice: grafoDirigido->getVertices()) {
            origenCombo->addItem(vertice);
            destinoCombo->addItem(vertice);
        }
    } else {
        for (const auto &vertice: grafoNoDirigido->getVertices()) {
            origenCombo->addItem(vertice);
            destinoCombo->addItem(vertice);
        }
    }
}

void MainWindow::onVerticeSeleccionado(const QString &vertice) {
    verticeSeleccionado = vertice;
    verticeLineEdit->setText(vertice);

    int index = origenCombo->findText(vertice);
    if (index != -1) {
        origenCombo->setCurrentIndex(index);
    }

    QPoint posicionLaPaz = grafoWidget->obtenerCoordenadasVertice(vertice);

    if (posicionLaPaz != QPoint(-1, -1)) {
        qDebug() << "La " << vertice << " está en las coordenadas:" << posicionLaPaz.x() << "," << posicionLaPaz.y();
    } else {
        qDebug() << "El vértice 'La Paz' no existe";
    }

    statusBar()->showMessage("Vértice seleccionado: " + vertice, 2000);
}

void MainWindow::onAristaSeleccionada(const QString &origen, const QString &destino) {
    aristaOrigenSeleccionada = origen;
    aristaDestinoSeleccionada = destino;

    int origenIndex = origenCombo->findText(origen);
    int destinoIndex = destinoCombo->findText(destino);

    if (origenIndex != -1) {
        origenCombo->setCurrentIndex(origenIndex);
    }
    if (destinoIndex != -1) {
        destinoCombo->setCurrentIndex(destinoIndex);
    }

    try {
        double pesoActual;
        double tiempoActual;
        if (esDirigido) {
            pesoActual = grafoDirigido->getPesoArista(origen, destino);
            tiempoActual = grafoDirigido->getTiempoArista(origen, destino);
        } else {
            pesoActual = grafoNoDirigido->getPesoArista(origen, destino);
            tiempoActual = grafoNoDirigido->getTiempoArista(origen, destino);
        }
        distanciaSpinBox->setValue(pesoActual);
        tiempoSpinBox->setValue(tiempoActual);
    } catch (const std::exception &e) {
    }

    statusBar()->showMessage("Arista seleccionada: " + origen + " → " + destino, 2000);
}

void MainWindow::mostrarMensaje(const QString &mensaje, bool esError) {
    if (esError) {
        QMessageBox::warning(this, "Error", mensaje);
        statusBar()->showMessage("Error: " + mensaje, 5000);
    } else {
        statusBar()->showMessage(mensaje, 2000);
    }
}

void MainWindow::configurarEjemploVuelos() {
    QStringList aeropuertos = {
        "La Paz", "Cochabamba", "Santa Cruz", "Sucre",
        "Tarija", "Trinidad", "Cobija", "Rurrenabaque", "Oruro",
        "Cuzco", "Uyuni", "Riberalta", "Guayaramerín", "Iquique",
        "Yacuiba"
    };

    for (const auto& aeropuerto : aeropuertos) {
        grafoNoDirigido->insertarVertice(aeropuerto);
    }

    grafoNoDirigido->insertarArista("Cochabamba", "La Paz", 240, 0.75);
    grafoNoDirigido->insertarArista("Cochabamba", "Sucre", 228, 0.6667);
    grafoNoDirigido->insertarArista("Cochabamba", "Oruro", 111, 0.5833);
    grafoNoDirigido->insertarArista("La Paz", "Cochabamba", 240, 0.75);
    grafoNoDirigido->insertarArista("La Paz", "Rurrenabaque", 243, 0.8333);
    grafoNoDirigido->insertarArista("La Paz", "Sucre", 443, 1.0);
    grafoNoDirigido->insertarArista("La Paz", "Cobija", 612, 1.0833);
    grafoNoDirigido->insertarArista("La Paz", "Tarija", 669, 1.1667);
    grafoNoDirigido->insertarArista("La Paz", "Trinidad", 401, 0.9167);
    grafoNoDirigido->insertarArista("La Paz", "Santa Cruz", 555, 1.0833);
    grafoNoDirigido->insertarArista("La Paz", "Uyuni", 415.211, 1);
    grafoNoDirigido->insertarArista("La Paz", "Cuzco", 519.818, 1.15);
    grafoNoDirigido->insertarArista("La Paz", "Iquique", 492.459, 1.20);
    grafoNoDirigido->insertarArista("Santa Cruz", "Cochabamba", 322, 0.75);
    grafoNoDirigido->insertarArista("Santa Cruz", "Sucre", 277, 0.75);
    grafoNoDirigido->insertarArista("Santa Cruz", "Trinidad", 365, 0.8333);
    grafoNoDirigido->insertarArista("Santa Cruz", "Oruro", 417, 1.0833);
    grafoNoDirigido->insertarArista("Santa Cruz", "La Paz", 555, 1.2167);
    grafoNoDirigido->insertarArista("Santa Cruz", "Sucre", 463.491, 1);
    grafoNoDirigido->insertarArista("Santa Cruz", "Iquique", 806.281, 1.50);
    grafoNoDirigido->insertarArista("Trinidad", "Guayaramerín", 447.398, 1.15);
    grafoNoDirigido->insertarArista("Trinidad", "Riberalta", 440.96, 0.50);
    grafoNoDirigido->insertarArista("Cochabamba", "Yacuiba", 568.098, 1.10);

    actualizarVisualizacion();
}


void MainWindow::mostrarInformacion() const {
    QString info;

    if (esDirigido) {
        info += "=== RED DE VUELOS DIRIGIDOS ===\n\n";
    } else {
        info += "=== RED DE VUELOS NO DIRIGIDOS ===\n\n";
    }

    info += "Aeropuertos: " + QString::number(esDirigido ?
            grafoDirigido->cantidadVertices() :
            grafoNoDirigido->cantidadVertices()) + "\n";

    info += "Rutas de vuelo: " + QString::number(esDirigido ?
            grafoDirigido->cantidadAristas() :
            grafoNoDirigido->cantidadAristas()) + "\n";

    info += "Distancia total: " + QString::number(esDirigido ?
            grafoDirigido->pesoTotal() :
            grafoNoDirigido->pesoTotal()) + " km\n\n";

    info += "Lista de aeropuertos:\n";
    auto vertices = esDirigido ? grafoDirigido->getVertices() : grafoNoDirigido->getVertices();
    for (const auto &aeropuerto: vertices) {
        info += "- " + aeropuerto + "\n";
    }

    info += "\nRutas disponibles:\n";
    for (const auto &aeropuerto: vertices) {
        auto adyacentes = esDirigido ?
                          grafoDirigido->getAdyacentesDelVerticeConPeso(aeropuerto) :
                          grafoNoDirigido->getAdyacentesDelVerticeConPeso(aeropuerto);

        for (const auto &ady: adyacentes) {
            QString destino = esDirigido ?
                             grafoDirigido->getVertice(ady.indiceVertice) :
                             grafoNoDirigido->getVertice(ady.indiceVertice);

            if (esDirigido || aeropuerto < destino) { // Para no mostrar duplicados en no dirigido
                info += aeropuerto + " → " + destino + " (" +
                        QString::number(ady.distancia) + " km)\n";
            }
        }
    }

    infoTextEdit->setText(info);
    statusBar()->showMessage("Información actualizada", 2000);
}
void MainWindow::calcularRutaMasCorta() {
    QString origen = origenCombo->currentText();
    QString destino = destinoCombo->currentText();

    if (origen.isEmpty() || destino.isEmpty()) {
        mostrarMensaje("Seleccione vértices de origen y destino", true);
        return;
    }

    mostrarResultadosDijkstra(origen, destino);
}

void MainWindow::calcularFloydWarshall() {
    try {
        mostrarResultadosFloydWarshall();
    } catch (const std::exception &e) {
        mostrarMensaje("Error al ejecutar Floyd-Warshall: " + QString(e.what()), true);
    }
}

void MainWindow::mostrarResultadosDijkstra(const QString &origen, const QString &destino) const {
    QString info = "=== ALGORITMO DE DIJKSTRA ===\n\n";
    info += QString("Origen: %1\nDestino: %2\n\n").arg(origen, destino);

    try {
        if (esDirigido) {
            auto camino = grafoDirigido->obtenerCaminoTradicional(origen, destino);

            if (camino.empty()) {
                info += "❌ No existe camino entre " + origen + " y " + destino + "\n";
            } else {
                info += "✅ CAMINO ENCONTRADO:\n";
                info += "Ruta: ";
                for (size_t i = 0; i < camino.size(); ++i) {
                    info += camino[i];
                    if (i < camino.size() - 1) {
                        info += " → ";
                    }
                }
                info += "\n";

                double distanciaTotal = grafoDirigido->obtenerCostoMinimo(origen, destino);
                info += QString("Distancia total: %1 km\n").arg(distanciaTotal);

                double tiempoTotal = grafoDirigido->obtenerTiempoTradicional(origen, destino);
                info += QString("Tiempo total: %1 horas\n").arg(tiempoTotal);
            }
        } else {

            auto camino = grafoNoDirigido->obtenerCaminoTradicional(origen, destino);

            if (camino.empty()) {
                info += "❌ No existe camino entre " + origen + " y " + destino + "\n";
            } else {
                info += "✅ CAMINO ENCONTRADO:\n";
                info += "Ruta: ";
                for (size_t i = 0; i < camino.size(); ++i) {
                    info += camino[i];
                    if (i < camino.size() - 1) {
                        info += " → ";
                    }
                }
                info += "\n";

                double distanciaTotal = grafoNoDirigido->obtenerCostoMinimo(origen, destino);
                info += QString("Distancia total: %1 km\n").arg(distanciaTotal);

                double tiempoTotal = grafoNoDirigido->obtenerTiempoTradicional(origen, destino);
                info += QString("Tiempo total: %1 horas\n").arg(tiempoTotal);
            }
        }

        info += "\n--- INFORMACIÓN DEL ALGORITMO ---\n";
        info += "Algoritmo utilizado: Dijkstra (implementación tradicional)\n";
        info += "Complejidad temporal: O(V²)\n";
        info += "Estructuras utilizadas: Arrays/Vectores\n";

    } catch (const std::exception &e) {
        info += "❌ Error: " + QString(e.what()) + "\n";
    }

    infoTextEdit->setText(info);
    statusBar()->showMessage("Dijkstra calculado", 2000);
}
void MainWindow::mostrarResultadosFloydWarshall() const {
    QString info = "=== ALGORITMO DE FLOYD-WARSHALL ===\n\n";
    info += "Calculando todas las distancias más cortas...\n\n";

    try {
        if (esDirigido) {
            FloydWarshall floyd(*grafoDirigido);
            floyd.ejecutarFloyd();

            auto vertices = grafoDirigido->getVertices();

            info += "✅ MATRIZ DE DISTANCIAS CALCULADA\n\n";
            info += "📍 CAMINOS MÁS CORTOS:\n";
            info += "======================\n\n";

            std::vector<std::tuple<QString, QString, double, std::vector<QString>>> caminos;

            for (size_t i = 0; i < vertices.size(); ++i) {
                for (size_t j = 0; j < vertices.size(); ++j) {
                    if (i != j) {
                        auto camino = floyd.obtenerCaminoCompleto(vertices[i], vertices[j]);
                        if (!camino.empty()) {
                            double distancia = floyd.obtenerDistanciaCamino(i, j);
                            caminos.push_back({vertices[i], vertices[j], distancia, camino});
                        }
                    }
                }
            }

            std::sort(caminos.begin(), caminos.end(),
                     [](const auto &a, const auto &b) { return std::get<2>(a) < std::get<2>(b); });

            for (const auto &[origen, destino, distancia, camino] : caminos) {
                info += QString("🎯 %1 → %2: %3 km\n")
                       .arg(origen)
                       .arg(destino)
                       .arg(distancia, 0, 'f', 2);

                info += "   Ruta: ";
                for (size_t k = 0; k < camino.size(); ++k) {
                    info += camino[k];
                    if (k < camino.size() - 1) info += " → ";
                }
                info += "\n\n";
            }

            info += "📊 ESTADÍSTICAS:\n";
            info += "================\n";
            info += QString("Total de vértices: %1\n").arg(vertices.size());
            info += QString("Caminos encontrados: %1\n").arg(caminos.size());
            info += QString("Caminos posibles: %1\n").arg(vertices.size() * (vertices.size() - 1));

            if (!caminos.empty()) {
                info += QString("Distancia mínima: %1 km\n").arg(std::get<2>(caminos.front()), 0, 'f', 2);
                info += QString("Distancia máxima: %1 km\n").arg(std::get<2>(caminos.back()), 0, 'f', 2);
            }

        } else {
            FloydWarshall floyd(*grafoNoDirigido);
            floyd.ejecutarFloyd();
            auto vertices = grafoNoDirigido->getVertices();

            info += "✅ MATRIZ DE DISTANCIAS CALCULADA\n\n";
            info += "📍 CAMINOS MÁS CORTOS:\n";
            info += "======================\n\n";

            std::vector<std::tuple<QString, QString, double, std::vector<QString>>> caminos;

            for (size_t i = 0; i < vertices.size(); ++i) {
                for (size_t j = i + 1; j < vertices.size(); ++j) {
                    auto camino = floyd.obtenerCaminoCompleto(vertices[i], vertices[j]);
                    if (!camino.empty()) {
                        double distancia = floyd.obtenerDistanciaCamino(i, j);
                        caminos.push_back({vertices[i], vertices[j], distancia, camino});
                    }
                }
            }

            std::sort(caminos.begin(), caminos.end(),
                     [](const auto &a, const auto &b) { return std::get<2>(a) < std::get<2>(b); });

            for (const auto &[origen, destino, distancia, camino] : caminos) {
                info += QString("🎯 %1 ↔ %2: %3 km\n")
                       .arg(origen)
                       .arg(destino)
                       .arg(distancia, 0, 'f', 2);

                info += "   Ruta: ";
                for (size_t k = 0; k < camino.size(); ++k) {
                    info += camino[k];
                    if (k < camino.size() - 1) info += " → ";
                }
                info += "\n\n";
            }

            info += "📊 ESTADÍSTICAS:\n";
            info += "================\n";
            info += QString("Total de vértices: %1\n").arg(vertices.size());
            info += QString("Caminos encontrados: %1\n").arg(caminos.size());
            info += QString("Caminos posibles: %1\n").arg((vertices.size() * (vertices.size() - 1)) / 2);

            if (!caminos.empty()) {
                info += QString("Distancia mínima: %1 km\n").arg(std::get<2>(caminos.front()), 0, 'f', 2);
                info += QString("Distancia máxima: %1 km\n").arg(std::get<2>(caminos.back()), 0, 'f', 2);
            }
        }

        info += "\n--- INFORMACIÓN DEL ALGORITMO ---\n";
        info += "Algoritmo utilizado: Floyd-Warshall\n";
        info += "Complejidad temporal: O(V³)\n";
        info += "Estructuras utilizadas: Matriz de adyacencia\n";
        info += "Ventaja: Encuentra todos los caminos más cortos entre todos los pares de vértices\n";

    } catch (const std::exception &e) {
        info += "❌ Error: " + QString(e.what()) + "\n";
    }

    infoTextEdit->setText(info);
    statusBar()->showMessage("Floyd-Warshall calculado", 2000);
}

void MainWindow::mostrarTodosLosCaminosDijkstra(const QString &origen) const {
    QString info = "=== DIJKSTRA DESDE " + origen.toUpper() + " ===\n\n";

    try {
        auto distancias = esDirigido ? grafoDirigido->dijkstra(origen) : grafoNoDirigido->dijkstra(origen);

        auto vertices = esDirigido ? grafoDirigido->getVertices() : grafoNoDirigido->getVertices();

        info += "📍 DISTANCIAS MÍNIMAS DESDE " + origen + ":\n";
        info += "========================================\n\n";

        std::vector<std::pair<QString, double>> verticesOrdenados;

        for (int i = 0; i < vertices.size(); ++i) {
            const QString &vertice = vertices[i];
            if (vertice != origen && distancias[i] < 1e10) {
                verticesOrdenados.push_back({vertice, distancias[i]});
            }
        }


        std::sort(verticesOrdenados.begin(), verticesOrdenados.end(),
                  [](const auto &a, const auto &b) { return a.second < b.second; });


        for (const auto &par : verticesOrdenados) {
            info += QString("🎯 %1 → %2: %3 km\n")
                   .arg(origen)
                   .arg(par.first)
                   .arg(par.second, 0, 'f', 2);

            auto camino = esDirigido ?
                         grafoDirigido->obtenerCaminoTradicional(origen, par.first) :
                         grafoNoDirigido->obtenerCaminoTradicional(origen, par.first);

            if (!camino.empty()) {
                info += "   Ruta: ";
                for (size_t i = 0; i < camino.size(); ++i) {
                    info += camino[i];
                    if (i < camino.size() - 1) info += " → ";
                }
                info += "\n";
            }
            info += "\n";
        }

        std::vector<QString> verticesNoAlcanzables;
        for (int i = 0; i < vertices.size(); ++i) {
            const QString &vertice = vertices[i];
            if (vertice != origen && distancias[i] >= 1e10) {
                verticesNoAlcanzables.push_back(vertice);
            }
        }

        if (!verticesNoAlcanzables.empty()) {
            info += "❌ VÉRTICES NO ALCANZABLES:\n";
            info += "============================\n";
            for (const auto &vertice : verticesNoAlcanzables) {
                info += QString("🚫 %1 → %2: Sin conexión\n").arg(origen).arg(vertice);
            }
            info += "\n";
        }

        info += "📊 ESTADÍSTICAS:\n";
        info += "================\n";
        info += QString("Total de vértices: %1\n").arg(vertices.size() - 1); // -1 para excluir origen
        info += QString("Destinos alcanzables: %1\n").arg(verticesOrdenados.size());
        info += QString("Destinos no alcanzables: %1\n").arg(verticesNoAlcanzables.size());

        if (!verticesOrdenados.empty()) {
            info += QString("Distancia mínima: %1 km\n").arg(verticesOrdenados.front().second, 0, 'f', 2);
            info += QString("Distancia máxima: %1 km\n").arg(verticesOrdenados.back().second, 0, 'f', 2);
        }

    } catch (const std::exception &e) {
        info += "❌ Error: " + QString(e.what()) + "\n";
    }

    infoTextEdit->setText(info);
    statusBar()->showMessage("Dijkstra desde " + origen + " calculado", 2000);
}
