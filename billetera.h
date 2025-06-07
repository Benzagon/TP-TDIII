#ifndef BILLETERA_H
#define BILLETERA_H

#include <string>
#include <vector>
#include "lib.h"
#include "blockchain.h"

using namespace std;

class Billetera {
  public:
    /**
     * Constructor. No se utiliza directamente, si no que se asume que será
     * llamado por la blockchain al utilizar el método `abrir_billetera`.
     */
    Billetera(const id_billetera id, Blockchain* blockchain);

    /**
     *  Retorna el id de la billetera, asignado al momento de su creación.
     */
    id_billetera id() const;

    /**
     * Método utilizado para notificar a la billetera cuando se impacta en la
     * blockchain una transacción que la implica (ya sea como origen o destino).
     *
     * Este método también es notificado al registrarse la transacción semilla.
     *
     * Complejidad esperada: O(D*log(D) + C), donde:
     *   - D es la máxima cantidad de días que una billetera estuvo activa
     *   - C es la máxima cantidad de destinatarios totales a los que una billetera envió dinero
     */
    void notificar_transaccion(Transaccion t);

    /**
     * Devuelve el saldo actual de la billetera.
     *
     * Complejidad esperada: O(1)
     */
    monto saldo() const;

    /**
     * Devuelve el saldo que tenía la billetera hacia fin del día de `t`.
     *
     * Por ejemplo, si t es el 10 de enero a las 15hs, devolveremos el saldo que
     * tenía la billetera al fin del 10 de enero.
     *
     * Se asume como precondición que t es mayor o igual al momento de la
     * creación de la billetera.
     *
     * Complejidad esperada: O(log(D)), donde D es la máxima cantidad de días
     * que una billetera estuvo activa
     */
    monto saldo_al_fin_del_dia(timestamp t) const;

    /**
     * Devuelve las últimas `k` transaccionesen las que esta billetera participó
     * (ya sea como origen o destino). Incluye la transacción semilla.
     *
     * Complejidad esperada: O(k)
     */
    vector<Transaccion> ultimas_transacciones(int k) const;

    /**
     * Devuelve los ids de las `k` billeteras a las que más transacciones le
     * realizó esta billetera.
     *
     * Complejidad esperada: O(k)
     */
    vector<id_billetera> detinatarios_mas_frecuentes(int k) const;

  private:
    /** El id de la billetera */
    const id_billetera _id;

    /** Puntero a la blockchain asociada */
    Blockchain* const _blockchain;
};

#endif
