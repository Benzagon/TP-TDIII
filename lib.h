#ifndef LIB_H_
#define LIB_H_

typedef unsigned int id_billetera;
typedef unsigned int id_transaccion;
typedef unsigned int timestamp;
typedef unsigned int monto;

struct Transaccion {
    id_billetera origen;
    id_billetera destino;
    double monto;
    timestamp _timestamp;
};

#endif // LIB_H_
