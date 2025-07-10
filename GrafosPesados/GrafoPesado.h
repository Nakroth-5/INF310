#ifndef GRAFOPESADO_H
#define GRAFOPESADO_H
#include <queue>
#include <vector>
#include <string>
#include <map>
#include "Marcado.h"

struct AdyacenteConPeso {
    int indiceVertice;
    double distancia;
    double tiempo;

    explicit AdyacenteConPeso(int vertice) : indiceVertice(vertice), distancia(0), tiempo(0) {
    }

    AdyacenteConPeso(int vertice, double distancia, double tiempo) : indiceVertice(vertice), distancia(distancia),
                                                                     tiempo(tiempo) {
    }

    bool operator<(const AdyacenteConPeso &otro) const {
        return this->indiceVertice < otro.indiceVertice;
    }

    bool operator==(const AdyacenteConPeso &otro) const {
        return this->indiceVertice == otro.indiceVertice;
    }

    bool operator!=(const AdyacenteConPeso &otro) const {
        return !(*this == otro);
    }
};



template<typename T>
class GrafoPesado {
    struct CaminoResultado {
        std::vector<T> camino;
        double distanciaTotal;
        double tiempoTotal;
        bool existeCamino;

        CaminoResultado() : distanciaTotal(0), tiempoTotal(0), existeCamino(false) {}
    };
protected:
    std::vector<T> listaVertices;
    std::vector<std::vector<AdyacenteConPeso> > listaAdyacencia;

public:
    static constexpr int NRO_VERTICE_INVALIDO = -1;

    GrafoPesado();

    explicit GrafoPesado(const std::vector<T> &vertices);

    virtual ~GrafoPesado() = default;

    void insertarVertice(const T &vertice);

    void eliminarVertice(const T &vertice);

    virtual void insertarArista(const T &origen, const T &destino) { insertarArista(origen, destino, 0, 0); }

    virtual void insertarArista(const T &origen, const T &destino, double distancia, double tiempo);

    virtual void eliminarArista(const T &origen, const T &destino);

    double getPesoArista(const T &origen, const T &destino);

    void actualizarPesoArista(const T &origen, const T &destino, double distancia, double tiempo);

    virtual int grado(const T &vertice);

    bool existeAdyacencia(const T &origen, const T &destino) const;

    void validarVertice(const T &vertice) const;

    int getNroVertice(const T &vertice) const;

    const std::vector<T> &getVertices() const;

    double getTiempoArista(const T &origen, const T &destino) const;

    std::vector<T> getAdyacentesDelVertice(const T &vertice) const;

    std::vector<AdyacenteConPeso> getAdyacentesDelVerticeConPeso(const T &vertice) const;

    T getVertice(int pos) const;

    // Métodos de información
    int cantidadVertices() const;

    virtual int cantidadAristas() const;

    double pesoTotal() const;

    static bool esConexo();

    std::string toString() const;


    std::vector<T> obtenerCaminoDijkstra(const T &origen, const T &destino) const;

    double obtenerTiempoDijkstra(const T &origen, const T &destino) const;

    std::vector<double> dijkstra(const T &origen) const;

    std::vector<T> obtenerCaminoTradicional(const T &origen, const T &destino) const;

    double obtenerCostoMinimo(const T &origen, const T &destino) const;

    void mostrarProcesoDijkstra(const T &origen) const;

    double obtenerTiempoTradicional(const T &origen, const T &destino) const;

};

#endif
#include "GrafoPesado.tpp"