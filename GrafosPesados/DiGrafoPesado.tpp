#pragma once
#include <algorithm>
#include <stdexcept>

template<typename T>
DiGrafoPesado<T>::DiGrafoPesado() = default;

template<typename T>
DiGrafoPesado<T>::DiGrafoPesado(const std::vector<T> &vertices) : GrafoPesado<T>(vertices) {
}

template<typename T>
void DiGrafoPesado<T>::insertarArista(const T &origen, const T &destino) {
    insertarArista(origen, destino, 0, 0);
}

template<typename T>
void DiGrafoPesado<T>::insertarArista(const T &origen, const T &destino, double distancia, double tiempo) {
    this->validarVertice(origen);
    this->validarVertice(destino);

    int nroDeOrigen = this->getNroVertice(origen);
    int nroDeDestino = this->getNroVertice(destino);

    if (this->existeAdyacencia(origen, destino))
        throw std::invalid_argument("La arista ya existe");

    this->listaAdyacencia[nroDeOrigen].push_back(AdyacenteConPeso(nroDeDestino, distancia, tiempo)
    );
}

template<typename T>
void DiGrafoPesado<T>::eliminarArista(const T &origen, const T &destino) {
    this->validarVertice(origen);
    this->validarVertice(destino);

    if (!this->existeAdyacencia(origen, destino))
        throw std::invalid_argument("La arista no existe");

    int nroDeOrigen = this->getNroVertice(origen);
    int nroDeDestino = this->getNroVertice(destino);

    this->listaAdyacencia[nroDeOrigen].erase(
        std::remove_if(this->listaAdyacencia[nroDeOrigen].begin(), this->listaAdyacencia[nroDeOrigen].end(),
                       [nroDeDestino](const AdyacenteConPeso &adyacente) {
                           return adyacente.indiceVertice == nroDeDestino;
                       }),
        this->listaAdyacencia[nroDeOrigen].end());
}

template<typename T>
int DiGrafoPesado<T>::grado(const T &vertice) {
    return gradoEntrada(vertice) + gradoSalida(vertice);
}

template<typename T>
int DiGrafoPesado<T>::cantidadAristas() const {
    int cantidadAristas = 0;
    for (const auto &adyacentes: this->listaAdyacencia) {
        cantidadAristas += adyacentes.size();
    }
    return cantidadAristas;
}

template<typename T>
int DiGrafoPesado<T>::gradoEntrada(const T &vertice) const {
    this->validarVertice(vertice);
    int nroDeVertice = this->getNroVertice(vertice);
    int gradoEntrada = 0;
    for (const auto &adyacentes: this->listaAdyacencia) {
        for (const auto &adyacente: adyacentes) {
            if (adyacente.indiceVertice == nroDeVertice)
                gradoEntrada++;
        }
    }
    return gradoEntrada;
}

template<typename T>
int DiGrafoPesado<T>::gradoSalida(const T &vertice) const {
    this->validarVertice(vertice);
    return this->listaAdyacencia[this->getNroVertice(vertice)].size();
}
