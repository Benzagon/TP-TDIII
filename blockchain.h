#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include <list>
#include <map>
#include <cstdlib>

#include "lib.h"

using namespace std;

class Billetera;

class Blockchain {
  public:
    /** Constructor */
    Blockchain();

    /**
     * Registra una billetera en la blockchain y devuelve un puntero a la misma.
     *
     * Todas las billeteras tienen un saldo inicial de 100 unidades.
     *
     * Complejidad: misma que agregar_transacción.
     */
    Billetera* abrir_billetera();

    /**
     * Agrega una transacción.
     *
     * Valida que...:
     *   - ambas billeteras estén registradas en la blockchain
     *   - el puntero de la billetera origen coincida con el que hay en registro
     *   - la billetera origen tenga monto suficiente
     *   - sean billeteras distintas
     *
     * Devuelve `true` si y sólo si la transacción se registró con éxito.
     *
     * Complejidad: O(log(B) + T + NT), donde NT es la complejidad del método notificar_transaccion de la clase Billetera
     */
    bool agregar_transaccion(Billetera* origen, id_billetera destino, double monto);

    /**
     * Lista de todas las transacciones registradas.
     *
     * Complejidad: O(1), usando una referencia no modificable.
     */
    const list<Transaccion>& transacciones();

    /**
     * Calcula el saldo actual de una billetera, recorriendo toda la lista de
     * transacciones.
     *
     * Complejidad: O(T)
     */
    monto calcular_saldo(const Billetera* billetera) const;

    /**
     * Destructor.
     * Libera la memoria dinámica pedida por la blockchain al crear billeteras.
     */
    ~Blockchain();

  private:
    /** Listado de todas las transacciones realizadas */
    list<Transaccion> _transacciones;

    /**
     * Registro de todas las billeteras que fueron abiertas. Se mantiene un
     * puntero a cada una para poder notificarlas cuando hay una transacción.
     */
    map<id_billetera, Billetera *> _billeteras;

    /** Lleva cuenta del siguiente id a utilizar. */
    id_billetera _siguiente_id_billetera;

    /** El saldo inicial de todas las billeteras al momento de registrarse. */
    static const monto SALDO_INICIAL = 100;
};

#endif
