//
// Created by evert on 10-07-25.
//

#ifndef MARCADO_H
#define MARCADO_H
#include <vector>


class Marcado {
private:
    std::vector<bool> marcados;

public:
    explicit Marcado(int cantidadVertices) : marcados(cantidadVertices, false) {
    }

    void marcar(int vertice) {
        if (vertice >= 0 && vertice < marcados.size()) {
            marcados[vertice] = true;
        }
    }

    bool estaMarcado(int vertice) const {
        if (vertice >= 0 && vertice < marcados.size()) {
            return marcados[vertice];
        }
        return false;
    }

    void desmarcar(int vertice) {
        if (vertice >= 0 && vertice < marcados.size()) {
            marcados[vertice] = false;
        }
    }

    void reiniciar() {
        std::fill(marcados.begin(), marcados.end(), false);
    }
};

#endif //MARCADO_H
