//
// Created by evert on 09-07-25.
//

#ifndef FLOYDWARSHALL_H
#define FLOYDWARSHALL_H
#include <vector>

#include "../GrafoPesado.h"


template<typename T>
class FloydWarshall {
    struct CaminoResultadoFloyd {
        std::vector<T> camino;
        double distanciaTotal;
        double tiempoTotal;
        bool existeCamino;

        CaminoResultadoFloyd() : distanciaTotal(0), tiempoTotal(0), existeCamino(false) {}
    };
private:
    GrafoPesado<T> grafo;
    std::vector<std::vector<double> > matrizDePesos;
    std::vector<std::vector<int> > matrizDePredecesores;
    std::vector<T> listaVertices;
    double INFINITO = 1e9;
    int n;
    void obtenerVerticesIntermedios(int origen, int destino, std::vector<T> &camino) const;
public:
    explicit FloydWarshall(GrafoPesado<T> grafo);
    void ejecutarFloyd();
    void inicializarMatrices();
    const std::vector<std::vector<double>>& getMatrizDePesos() const;
    const std::vector<std::vector<int>>& getMatrizDePredecesores() const;
    double obtenerDistanciaCamino(int nroOrigen, int nroDestino) const;
    std::vector<T> obtenerCaminoCompleto(const T &verticeOrigen, const T &verticeDestino) const;
};


#endif
#include "FloydWarshall.tpp"
