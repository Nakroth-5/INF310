#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include <QGroupBox>
#include <QSplitter>

#include "GrafoWidget.h"
#include "GrafosPesados/DiGrafoPesado.h"
#include "GrafosPesados/GrafoPesado.h"


class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

private slots:
    void insertarVertice();

    void eliminarVertice();

    void insertarArista();

    void eliminarArista();

    void actualizarPeso();

    void limpiarGrafo();

    void mostrarInformacion() const;

    void cambiarTipoGrafo();

    void onVerticeSeleccionado(const QString &vertice);

    void onAristaSeleccionada(const QString &origen, const QString &destino);

private:
    void setupUI();

    void setupMenus();

    void setupToolbar();

    void setupStatusBar() const;

    void connectSignals();

    void actualizarVisualizacion() const;

    void actualizarInformacion();

    void mostrarMensaje(const QString &mensaje, bool esError = false);

    void configurarEjemploVuelos();

    void calcularRutaMasCorta();

    void mostrarResultadosDijkstra(const QString &origen, const QString &destino) const;

    void mostrarResultadosFloydWarshall() const;

    void mostrarTodosLosCaminosDijkstra(const QString &origen) const;

    void calcularFloydWarshall();


    // Widgets principales
    QWidget *centralWidget;
    QSplitter *splitter;
    GrafoWidget *grafoWidget;

    // Panel de control
    QWidget *controlPanel;
    QGroupBox *tipoGrafoGroup;
    QCheckBox *esDirigidoCheckBox;

    // Controles de vértices
    QGroupBox *verticesGroup;
    QLineEdit *verticeLineEdit;
    QPushButton *insertarVerticeBtn;
    QPushButton *eliminarVerticeBtn;

    // Controles de aristas
    QGroupBox *aristasGroup;
    QComboBox *origenCombo;
    QComboBox *destinoCombo;
    QDoubleSpinBox *distanciaSpinBox;
    QDoubleSpinBox *tiempoSpinBox;
    QPushButton *insertarAristaBtn;
    QPushButton *eliminarAristaBtn;
    QPushButton *actualizarPesoBtn;

    QGroupBox *algoritmosGroup;
    QPushButton *dijkstraBtn;
    QPushButton *floydWarshallBtn;
    QPushButton *todosCaminosDijkstraBtn;

    // Información
    QGroupBox *infoGroup;
    QTextEdit *infoTextEdit;
    QPushButton *mostrarInfoBtn;
    QPushButton *limpiarGrafoBtn;

    // Datos del grafo
    GrafoPesado<QString> *grafoNoDirigido;
    DiGrafoPesado<QString> *grafoDirigido;
    bool esDirigido;

    // Selección actual
    QString verticeSeleccionado;
    QString aristaOrigenSeleccionada;
    QString aristaDestinoSeleccionada;
};

#endif
