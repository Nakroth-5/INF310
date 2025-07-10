//
// Created by evert on 09-07-25.
//

#ifndef DIGRAFOPESADO_H
#define DIGRAFOPESADO_H
#include "GrafoPesado.h"

template<typename T>
class DiGrafoPesado : public GrafoPesado<T> {
public:
    DiGrafoPesado();

    explicit DiGrafoPesado(const std::vector<T> &vertices);

    void insertarArista(const T &origen, const T &destino) override;

    void insertarArista(const T &origen, const T &destino, double distancia, double tiempo) override;

    void eliminarArista(const T &origen, const T &destino) override;

    int grado(const T &vertice) override;

    [[nodiscard]] int cantidadAristas() const override;

    int gradoEntrada(const T &vertice) const;

    int gradoSalida(const T &vertice) const;
};


#endif
#include "DiGrafoPesado.tpp"