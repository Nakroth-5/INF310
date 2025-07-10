#pragma once

template<typename T>
FloydWarshall<T>::FloydWarshall(GrafoPesado<T> grafo) {
    this->grafo = grafo;
    this->matrizDePesos = std::vector<std::vector<double> >(grafo.cantidadVertices(),
                                                            std::vector<double>(grafo.cantidadVertices(), 0));
    this->matrizDePredecesores = std::vector<std::vector<int> >(grafo.cantidadVertices(),
                                                                std::vector<int>(grafo.cantidadVertices(), 0));
    this->listaVertices = grafo.getVertices();
    this->n = listaVertices.size();
    this->ejecutarFloyd();
}

template<typename T>
void FloydWarshall<T>::ejecutarFloyd() {
    inicializarMatrices();
    for (int k = 0; k < n; k++) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (matrizDePesos[i][k] + matrizDePesos[k][j] < matrizDePesos[i][j]) {
                    matrizDePesos[i][j] = matrizDePesos[i][k] + matrizDePesos[k][j];
                    matrizDePredecesores[i][j] = k;
                }
            }
        }
    }
}


template<typename T>
void FloydWarshall<T>::inicializarMatrices() {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i == j) {
                matrizDePesos[i][j] = 0;
                matrizDePredecesores[i][j] = -1;
            } else {
                matrizDePesos[i][j] = INFINITO;
                matrizDePredecesores[i][j] = j;
            }
        }
    }

    for (int i = 0; i < n; i++) {
        T vertice = listaVertices[i];
        auto adyacentes = grafo.getAdyacentesDelVertice(vertice);
        for (const auto &adyacente : adyacentes) {
            int nroAdyacente = grafo.getNroVertice(adyacente);
            matrizDePesos[i][nroAdyacente] = grafo.getPesoArista(vertice, adyacente);
        }
    }
}

template<typename T>
const std::vector<std::vector<double>> & FloydWarshall<T>::getMatrizDePesos() const {
    return matrizDePesos;
}

template<typename T>
const std::vector<std::vector<int>> & FloydWarshall<T>::getMatrizDePredecesores() const {
    return matrizDePredecesores;
}


template<typename T>
std::vector<T> FloydWarshall<T>::obtenerCaminoCompleto(const T &verticeOrigen, const T &verticeDestino) const {
    std::vector<T> caminoCompleto;

    grafo.validarVertice(verticeOrigen);
    grafo.validarVertice(verticeDestino);

    int nroOrigen = grafo.getNroVertice(verticeOrigen);
    int nroDestino = grafo.getNroVertice(verticeDestino);

    // Verificar si existe un camino
    if (matrizDePesos[nroOrigen][nroDestino] == INFINITO) {
        return caminoCompleto; // Devolver vector vacío si no hay camino
    }

    caminoCompleto.push_back(verticeOrigen);

    obtenerVerticesIntermedios(nroOrigen, nroDestino, caminoCompleto);

    caminoCompleto.push_back(verticeDestino);

    return caminoCompleto;
}

template<typename T>
void FloydWarshall<T>::obtenerVerticesIntermedios(int origen, int destino, std::vector<T> &camino) const {
    int intermedio = matrizDePredecesores[origen][destino];

    if (intermedio != destino) {
        obtenerVerticesIntermedios(origen, intermedio, camino);
        camino.push_back(listaVertices[intermedio]);
        obtenerVerticesIntermedios(intermedio, destino, camino);
    }
}

template<typename T>
double FloydWarshall<T>::obtenerDistanciaCamino(int nroOrigen, int nroDestino) const {

    if (matrizDePesos[nroOrigen][nroDestino] == INFINITO) {
        return -1; // o lanzar excepción
    }

    return matrizDePesos[nroOrigen][nroDestino];
}