#ifndef BILLETERA_H
#define BILLETERA_H

#include <string>
#include <vector>
#include "lib.h"
#include "blockchain.h"

using namespace std;


/**
 * INVARIANTE DE REPRESENTACIÓN:
 * _saldo:
 *  - Es la suma del monto de todas las transacciones donde Billetera fue destino 
 *  menos la suma del monto de todas las transacciones donde Billetera fue origen.
 * 
 * _billeteras_por_cantidad_de_transacciones:
 *  - La suma de las claves es la máxima cantidad de veces que Billetera envió dinero.
 *  - La suma de las longitudes de las valores es la máxima cantidad de destinatarios a los que Billetera envió dinero.
 *  - Dada cualquier clave del map, para toda billetera del vector en su valor, se cumple que la suma de transacciones
 *  donde esta fue destino es igual a la clave.
 *  - La longitud del map es a lo sumo la máxima cantidad de destinatarios totales a los que una billetera envió dinero.
 * 
 * _saldo_por_dia:
 *  - Las claves del map son finales de días.
 *  - La cantidad de claves en el map es la cantidad de días entre que se abrió Billetera y su última transacción.
 *  - Para cada clave, el valor es la suma del monto de todas las transacciones donde Billetera fue destino 
 *  menos la suma del monto de todas las transacciones donde Billetera fue origen hasta el día de la clave.
 * 
 * _transacciones:
 *  - Listado de todas las transacciones realizadas que involucran a la billetera.
 */
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

    /** Saldo actual de la billetera */
    monto _saldo;

    /** Mapa de cantidad de interacciones y billeteras asociadas destinatarias */
    map<int, vector<id_billetera>> _billeteras_por_cantidad_de_transacciones;

    /** Saldos por dia */
    map<timestamp, monto> _saldo_por_dia;
    
    /** Listado de todas las transacciones realizadas que involucran a la billetera*/
    list<Transaccion> _transacciones;

    /** Métodos auxiliares */

    id_billetera _conseguir_billetera_amigo(Transaccion t);
    
    void _actualizar_saldo(Transaccion t);

    void _actualizar_saldo_por_dia(Transaccion t);

    void _actualizar_billeteras_por_cantidad_de_transacciones(Transaccion t);

    void _actualizar_cantidad_transacciones_billetera_amigo(map<int, vector<id_billetera>>::iterator it, id_billetera billetera_amigo, int i);
};

#endif
