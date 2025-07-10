//
// Created by evert on 09-07-25.
//
#pragma once
#include <algorithm>
#include <stdexcept>
#include <sstream>
#include <iostream>

template<typename T>
GrafoPesado<T>::GrafoPesado() = default;

template<typename T>
GrafoPesado<T>::GrafoPesado(const std::vector<T> &vertices) {
    for (const T &vertice: vertices) {
        insertarVertice(vertice);
    }
}

template<typename T>
void GrafoPesado<T>::insertarVertice(const T &vertice) {
    if (getNroVertice(vertice) != NRO_VERTICE_INVALIDO)
        throw std::invalid_argument("El vertice ya existe");
    listaVertices.push_back(vertice);
    listaAdyacencia.emplace_back();
}

template<typename T>
void GrafoPesado<T>::eliminarVertice(const T &vertice) {
    validarVertice(vertice);
    int nroDeVertice = getNroVertice(vertice);

    listaVertices.erase(listaVertices.begin() + nroDeVertice);
    listaAdyacencia.erase(listaAdyacencia.begin() + nroDeVertice);

    for (auto &adyacentes: listaAdyacencia) {
        adyacentes.erase(std::remove_if(adyacentes.begin(), adyacentes.end(),
                                        [nroDeVertice](const AdyacenteConPeso &adyacente) {
                                            return adyacente.indiceVertice == nroDeVertice;
                                        }),
                         adyacentes.end());
        for (auto &adyacenteConPeso: adyacentes) {
            if (adyacenteConPeso.indiceVertice > nroDeVertice)
                adyacenteConPeso.indiceVertice--;
        }
    }
}

template<typename T>
void GrafoPesado<T>::insertarArista(const T &origen, const T &destino, double distancia, double tiempo) {
    validarVertice(origen);
    validarVertice(destino);

    int nroDeOrigen = getNroVertice(origen);
    int nroDeDestino = getNroVertice(destino);

    listaAdyacencia[nroDeOrigen].emplace_back(nroDeDestino, distancia, tiempo);
    std::sort(listaAdyacencia[nroDeOrigen].begin(), listaAdyacencia[nroDeOrigen].end());

    if (nroDeOrigen != nroDeDestino) {
        listaAdyacencia[nroDeDestino].emplace_back(nroDeOrigen, distancia, tiempo);
        std::sort(listaAdyacencia[nroDeDestino].begin(), listaAdyacencia[nroDeDestino].end());
    }
}

template<typename T>
void GrafoPesado<T>::eliminarArista(const T &origen, const T &destino) {
    validarVertice(origen);
    validarVertice(destino);

    if (!existeAdyacencia(origen, destino))
        throw std::invalid_argument("La arista no existe");

    int nroDeOrigen = getNroVertice(origen);
    int nroDeDestino = getNroVertice(destino);

    listaAdyacencia[nroDeOrigen].erase(std::remove_if(listaAdyacencia[nroDeOrigen].begin(),
                                                      listaAdyacencia[nroDeOrigen].end(),
                                                      [nroDeDestino](const AdyacenteConPeso &adyacente) {
                                                          return adyacente.indiceVertice == nroDeDestino;
                                                      }),
                                       listaAdyacencia[nroDeOrigen].end());

    if (nroDeOrigen != nroDeDestino) {
        listaAdyacencia[nroDeDestino].erase(std::remove_if(listaAdyacencia[nroDeDestino].begin(),
                                                           listaAdyacencia[nroDeDestino].end(),
                                                           [nroDeOrigen](const AdyacenteConPeso &adyacente) {
                                                               return adyacente.indiceVertice == nroDeOrigen;
                                                           }),
                                            listaAdyacencia[nroDeDestino].end());
    }
}

template<typename T>
double GrafoPesado<T>::getPesoArista(const T &origen, const T &destino) {
    validarVertice(origen);
    validarVertice(destino);

    if (!existeAdyacencia(origen, destino))
        throw std::invalid_argument("La arista no existe");

    int nroDeOrigen = getNroVertice(origen);
    int nroDeDestino = getNroVertice(destino);

    for (const auto &adyacente: listaAdyacencia[nroDeOrigen]) {
        if (adyacente.indiceVertice == nroDeDestino)
            return adyacente.distancia;
    }
    return 0;
}

template<typename T>
void GrafoPesado<T>::actualizarPesoArista(const T &origen, const T &destino, double distancia, double tiempo) {
    validarVertice(origen);
    validarVertice(destino);

    if (!existeAdyacencia(origen, destino))
        throw std::invalid_argument("La arista no existe");

    int nroDeOrigen = getNroVertice(origen);
    int nroDeDestino = getNroVertice(destino);

    for (auto &adyacente: listaAdyacencia[nroDeOrigen]) {
        if (adyacente.indiceVertice == nroDeDestino) {
            adyacente.distancia = distancia;
            adyacente.tiempo = tiempo;
            break;
        }
    }

    if (nroDeOrigen != nroDeDestino) {
        for (auto &adyacente: listaAdyacencia[nroDeDestino]) {
            if (adyacente.indiceVertice == nroDeOrigen) {
                adyacente.distancia = distancia;
                adyacente.tiempo = tiempo;
                break;
            }
        }
    }
}


template<typename T>
int GrafoPesado<T>::grado(const T &vertice) {
    validarVertice(vertice);
    return listaAdyacencia[getNroVertice(vertice)].size();
}

template<typename T>
bool GrafoPesado<T>::existeAdyacencia(const T &origen, const T &destino) const {
    int nroOrigen = getNroVertice(origen);
    int nroDestino = getNroVertice(destino);

    if (nroOrigen == NRO_VERTICE_INVALIDO || nroDestino == NRO_VERTICE_INVALIDO)
        return false;

    for (const auto &adyacente: listaAdyacencia[nroOrigen]) {
        if (adyacente.indiceVertice == nroDestino)
            return true;
    }

    return false;
}

template<typename T>
void GrafoPesado<T>::validarVertice(const T &vertice) const {
    if (getNroVertice(vertice) == NRO_VERTICE_INVALIDO)
        throw std::invalid_argument("El vertice no existe");
}

template<typename T>
int GrafoPesado<T>::getNroVertice(const T &vertice) const {
    auto it = std::find(listaVertices.begin(), listaVertices.end(), vertice);
    if (it != listaVertices.end())
        return std::distance(listaVertices.begin(), it);
    return NRO_VERTICE_INVALIDO;
}

template<typename T>
const std::vector<T> &GrafoPesado<T>::getVertices() const {
    return listaVertices;
}


template<typename T>
T GrafoPesado<T>::getVertice(int pos) const {
    if (pos < 0 || pos >= listaVertices.size())
        throw std::out_of_range("Posición invalida");
    return listaVertices[pos];
}

template<typename T>
int GrafoPesado<T>::cantidadVertices() const {
    return listaVertices.size();
}

template<typename T>
int GrafoPesado<T>::cantidadAristas() const {
    int cantidadAristas = 0;
    for (const auto &adyacentes: listaAdyacencia) {
        cantidadAristas += adyacentes.size();
    }
    return cantidadAristas % 2 == 0 ? cantidadAristas / 2 : cantidadAristas / 2 + 1;
}

template<typename T>
double GrafoPesado<T>::pesoTotal() const {
    double total = 0.0;
    for (int i = 0; i < listaVertices.size(); i++)
        for (const auto &adyacente: listaAdyacencia[i])
            if (i <= adyacente.indiceVertice)
                total += adyacente.distancia;
    return total;
}

template<typename T>
bool GrafoPesado<T>::esConexo() {
    return false;
}

template<typename T>
std::string GrafoPesado<T>::toString() const {
    std::stringstream ss;
    for (int i = 0; i < listaVertices.size(); i++) {
        ss << i << " " << listaVertices[i].toStdString() << " -> ";

        const auto &adyacentes = listaAdyacencia[i];
        ss << "[";
        for (int j = 0; j < adyacentes.size(); j++) {
            const auto &adyacente = adyacentes[j];
            ss << "(" << adyacente.indiceVertice << "," << adyacente.distancia << ")";
            if (j < adyacentes.size() - 1) {
                ss << ", ";
            }
        }
        ss << "]";
        ss << "\n";
    }
    return ss.str();
}

template<typename T>
std::vector<T> GrafoPesado<T>::obtenerCaminoDijkstra(const T &origen, const T &destino) const {
    auto resultado = dijkstraConCamino(origen, destino);
    return resultado.camino;
}

template<typename T>
double GrafoPesado<T>::obtenerTiempoDijkstra(const T &origen, const T &destino) const {
    auto resultado = dijkstraConCamino(origen, destino);
    return resultado.tiempoTotal;
}


template<typename T>
double GrafoPesado<T>::getTiempoArista(const T &origen, const T &destino) const {
    validarVertice(origen);
    validarVertice(destino);

    if (!existeAdyacencia(origen, destino))
        throw std::invalid_argument("La arista no existe");

    int nroDeOrigen = getNroVertice(origen);
    int nroDeDestino = getNroVertice(destino);

    for (const auto &adyacente: listaAdyacencia[nroDeOrigen]) {
        if (adyacente.indiceVertice == nroDeDestino)
            return adyacente.tiempo;
    }
    return 0;
}


template<typename T>
std::vector<T> GrafoPesado<T>::getAdyacentesDelVertice(const T &vertice) const {
    validarVertice(vertice);
    std::vector<T> adyacentes;

    for (const auto &adyacente: listaAdyacencia[getNroVertice(vertice)]) {
        adyacentes.push_back(listaVertices[adyacente.indiceVertice]);
    }
    return adyacentes;
}

template<typename T>
std::vector<AdyacenteConPeso> GrafoPesado<T>::getAdyacentesDelVerticeConPeso(const T &vertice) const {
    validarVertice(vertice);
    return listaAdyacencia[getNroVertice(vertice)];
}

template<typename T>
std::vector<double> GrafoPesado<T>::dijkstra(const T &origen) const {
    validarVertice(origen);

    int n = cantidadVertices();
    int indiceOrigen = getNroVertice(origen);

    std::vector costos(n, 1e10);
    std::vector predecesores(n, -1);
    Marcado marcados(n);

    costos[indiceOrigen] = 0;

    // Algoritmo principal
    for (int i = 0; i < n; i++) {
        // Encontrar vértice no marcado con menor costo
        int verticeActual = -1;
        double menorCosto = 1e10;

        for (int j = 0; j < n; j++) {
            if (!marcados.estaMarcado(j) && costos[j] < menorCosto) {
                menorCosto = costos[j];
                verticeActual = j;
            }
        }

        // Si no encontramos vértice válido, terminamos
        if (verticeActual == -1) break;

        // Marcar el vértice actual
        marcados.marcar(verticeActual);

        // Actualizar costos de los adyacentes
        for (const auto &adyacente : listaAdyacencia[verticeActual]) {
            int vecino = adyacente.indiceVertice;
            double peso = adyacente.distancia;

            if (!marcados.estaMarcado(vecino)) {
                double nuevoCosto = costos[verticeActual] + peso;
                if (nuevoCosto < costos[vecino]) {
                    costos[vecino] = nuevoCosto;
                    predecesores[vecino] = verticeActual;
                }
            }
        }
    }

    return costos;
}


template<typename T>
std::vector<T> GrafoPesado<T>::obtenerCaminoTradicional(const T &origen, const T &destino) const {
    validarVertice(origen);
    validarVertice(destino);

    int n = cantidadVertices();
    int indiceOrigen = getNroVertice(origen);
    int indiceDestino = getNroVertice(destino);

    // Inicializar estructuras
    std::vector costos(n, 1e10);
    std::vector predecesores(n, -1);
    Marcado marcados(n);

    costos[indiceOrigen] = 0;

    for (int i = 0; i < n; i++) {
        int verticeActual = -1;
        double menorCosto = 1e10;

        for (int j = 0; j < n; j++) {
            if (!marcados.estaMarcado(j) && costos[j] < menorCosto) {
                menorCosto = costos[j];
                verticeActual = j;
            }
        }

        if (verticeActual == -1) break;
        marcados.marcar(verticeActual);

        for (const auto &adyacente : listaAdyacencia[verticeActual]) {
            int vecino = adyacente.indiceVertice;
            double peso = adyacente.distancia;

            if (!marcados.estaMarcado(vecino)) {
                double nuevoCosto = costos[verticeActual] + peso;
                if (nuevoCosto < costos[vecino]) {
                    costos[vecino] = nuevoCosto;
                    predecesores[vecino] = verticeActual;
                }
            }
        }
    }


    std::vector<T> camino;
    if (costos[indiceDestino] >= 1e10) {
        return camino;
    }

    int actual = indiceDestino;
    while (actual != -1) {
        camino.push_back(listaVertices[actual]);
        actual = predecesores[actual];
    }

    std::reverse(camino.begin(), camino.end());
    return camino;
}


template<typename T>
double GrafoPesado<T>::obtenerCostoMinimo(const T &origen, const T &destino) const {
    validarVertice(origen);
    validarVertice(destino);

    std::vector<double> costos = dijkstra(origen);
    int indiceDestino = getNroVertice(destino);

    return costos[indiceDestino] >= 1e10 ? -1 : costos[indiceDestino];
}


template<typename T>
void GrafoPesado<T>::mostrarProcesoDijkstra(const T &origen) const {
    validarVertice(origen);

    int n = cantidadVertices();
    int indiceOrigen = getNroVertice(origen);

    std::vector costos(n, 1e10);
    std::vector predecesores(n, -1);
    Marcado marcados(n);

    costos[indiceOrigen] = 0;

    std::cout << "Estado inicial:" << std::endl;
    std::cout << "Vértices: ";
    for (int i = 0; i < n; i++) {
        std::cout << i << " ";
    }
    std::cout << std::endl;

    std::cout << "Costos: ";
    for (int i = 0; i < n; i++) {
        if (costos[i] >= 1e10) {
            std::cout << "∞ ";
        } else {
            std::cout << costos[i] << " ";
        }
    }
    std::cout << std::endl << std::endl;

    for (int iteracion = 0; iteracion < n; iteracion++) {
        // Encontrar vértice no marcado con menor costo
        int verticeActual = -1;
        double menorCosto = 1e10;

        for (int j = 0; j < n; j++) {
            if (!marcados.estaMarcado(j) && costos[j] < menorCosto) {
                menorCosto = costos[j];
                verticeActual = j;
            }
        }

        if (verticeActual == -1) break;

        std::cout << "Iteración " << iteracion + 1 << ":" << std::endl;
        std::cout << "Vértice seleccionado: " << verticeActual << " (costo: " << costos[verticeActual] << ")" << std::endl;

        marcados.marcar(verticeActual);

        // Actualizar costos de adyacentes
        for (const auto &adyacente : listaAdyacencia[verticeActual]) {
            int vecino = adyacente.indiceVertice;
            double peso = adyacente.distancia;

            if (!marcados.estaMarcado(vecino)) {
                double nuevoCosto = costos[verticeActual] + peso;
                if (nuevoCosto < costos[vecino]) {
                    std::cout << "  Actualizando vértice " << vecino << ": " << costos[vecino] << " -> " << nuevoCosto << std::endl;
                    costos[vecino] = nuevoCosto;
                    predecesores[vecino] = verticeActual;
                }
            }
        }

        std::cout << "Costos actuales: ";
        for (int i = 0; i < n; i++) {
            if (costos[i] >= 1e10) {
                std::cout << "∞ ";
            } else {
                std::cout << costos[i] << " ";
            }
        }
        std::cout << std::endl;

        std::cout << "Marcados: ";
        for (int i = 0; i < n; i++) {
            std::cout << (marcados.estaMarcado(i) ? "✓" : "✗") << " ";
        }
        std::cout << std::endl << std::endl;
    }
}


template<typename T>
double GrafoPesado<T>::obtenerTiempoTradicional(const T &origen, const T &destino) const {
    validarVertice(origen);
    validarVertice(destino);

    int n = cantidadVertices();
    int indiceOrigen = getNroVertice(origen);
    int indiceDestino = getNroVertice(destino);

    // Inicializar estructuras
    std::vector tiempos(n, 1e10);  // Array de tiempos en lugar de distancias
    std::vector predecesores(n, -1);
    Marcado marcados(n);

    tiempos[indiceOrigen] = 0;

    // Ejecutar algoritmo usando tiempo como peso
    for (int i = 0; i < n; i++) {
        int verticeActual = -1;
        double menorTiempo = 1e10;

        for (int j = 0; j < n; j++) {
            if (!marcados.estaMarcado(j) && tiempos[j] < menorTiempo) {
                menorTiempo = tiempos[j];
                verticeActual = j;
            }
        }

        if (verticeActual == -1) break;
        marcados.marcar(verticeActual);

        for (const auto &adyacente : listaAdyacencia[verticeActual]) {
            int vecino = adyacente.indiceVertice;
            double peso = adyacente.tiempo;  // Usar tiempo en lugar de distancia

            if (!marcados.estaMarcado(vecino)) {
                double nuevoTiempo = tiempos[verticeActual] + peso;
                if (nuevoTiempo < tiempos[vecino]) {
                    tiempos[vecino] = nuevoTiempo;
                    predecesores[vecino] = verticeActual;
                }
            }
        }
    }

    return tiempos[indiceDestino] >= 1e10 ? -1 : tiempos[indiceDestino];
}
