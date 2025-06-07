#ifndef TESTS_LIB_H_
#define TESTS_LIB_H_

#include <cassert>
#include <gtest/gtest.h>

void inline chequear_transaccion(Transaccion tx, id_billetera origen, id_billetera destino, monto monto) {
  EXPECT_EQ(tx.origen, origen);
  EXPECT_EQ(tx.destino, destino);
  EXPECT_EQ(tx.monto, monto);
}

void inline chequear_ids_billeteras(vector<id_billetera> v1, vector<id_billetera> v2) {
  EXPECT_EQ(v1, v2);
}

// Hace una transaccion y verifica que se ejecutó correctamente, de modo que el
// test falle si hubo un problema.
void inline agregar_transaccion(Blockchain& blockchain, Billetera* b1, Billetera* b2, monto monto) {
  EXPECT_TRUE(blockchain.agregar_transaccion(b1, b2->id(), monto));
}

#endif // TESTS_LIB_H_
