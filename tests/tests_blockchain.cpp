#include <string>
#include <cassert>
#include <gtest/gtest.h>

#include "../lib.h"
#include "../blockchain.h"
#include "../billetera.h"
#include "tests_lib.h"

using namespace std;

TEST(tests_blockchain,blockchain_no_tiene_transacciones_al_principio) {
  Blockchain blockchain;
  EXPECT_EQ(blockchain.transacciones().empty(), true);
}

TEST(tests_blockchain,las_billeteras_comienzan_con_100_unidades) {
  Blockchain blockchain;
  Billetera* billetera = blockchain.abrir_billetera();

  EXPECT_EQ(blockchain.calcular_saldo(billetera), 100);
}

TEST(tests_blockchain,permite_agregar_transaccion) {
  Blockchain blockchain;

  Billetera* billetera1 = blockchain.abrir_billetera();
  Billetera* billetera2 = blockchain.abrir_billetera();

  // transacciones semilla
  EXPECT_EQ(blockchain.transacciones().size(), 2);

  bool resultado = blockchain.agregar_transaccion(billetera1, billetera2->id(), 10);

  EXPECT_EQ(resultado, true);
  EXPECT_EQ(blockchain.transacciones().size(), 3);

  Transaccion tx = *blockchain.transacciones().rbegin();
  chequear_transaccion(tx, billetera1->id(), billetera2->id(), 10);

  EXPECT_EQ(blockchain.calcular_saldo(billetera1), 90);
  EXPECT_EQ(blockchain.calcular_saldo(billetera2), 110);
}

TEST(tests_blockchain,no_permite_hacer_transaccion_con_billetera_origen_desconocida) {
  Blockchain blockchain1;
  Blockchain blockchain2;

  Billetera* billetera1 = blockchain2.abrir_billetera();
  Billetera* billetera2 = blockchain1.abrir_billetera();

  bool resultado = blockchain1.agregar_transaccion(billetera1, billetera2->id(), 10);

  EXPECT_EQ(resultado, false);
  EXPECT_EQ(blockchain1.transacciones().size(), 1); // sólo transacción semilla
}

TEST(tests_blockchain,no_permite_hacer_transaccion_con_billetera_destino_desconocida) {
  Blockchain blockchain1;
  Blockchain blockchain2;

  Billetera* billetera1 = blockchain1.abrir_billetera();
  Billetera* billetera2 = blockchain2.abrir_billetera();

  bool resultado = blockchain1.agregar_transaccion(billetera1, billetera2->id(), 10);

  EXPECT_EQ(resultado, false);
  EXPECT_EQ(blockchain1.transacciones().size(), 1); // sólo transacción semilla
}

TEST(tests_blockchain,no_permite_hacerse_pasar_por_una_billetera_para_hacer_una_transaccion) {
  Blockchain blockchain;

  // billeteras reales creadas por la blockchain
  Billetera* billetera1 = blockchain.abrir_billetera();
  Billetera* billetera2 = blockchain.abrir_billetera();

  // billetera "falsa" que simula ser otra usando su mismo id
  Billetera billetera1_falsa(billetera1->id(), &blockchain);

  bool resultado = blockchain.agregar_transaccion(&billetera1_falsa, billetera2->id(), 10);

  EXPECT_EQ(resultado, false);
  EXPECT_EQ(blockchain.transacciones().size(), 2);
}

TEST(tests_blockchain,no_permite_hacer_transaccion_si_la_billetera_origen_no_tiene_saldo_suficiente) {
  Blockchain blockchain;

  Billetera* billetera1 = blockchain.abrir_billetera();
  Billetera* billetera2 = blockchain.abrir_billetera();

  bool resultado = blockchain.agregar_transaccion(billetera1, billetera2->id(), 101);

  EXPECT_EQ(resultado, false);
  EXPECT_EQ(blockchain.transacciones().size(), 2); // sólo transacciones semilla
}

TEST(tests_blockchain,no_permite_hacerse_transferencias_a_uno_mismo) {
  Blockchain blockchain;

  Billetera* billetera = blockchain.abrir_billetera();

  bool resultado = blockchain.agregar_transaccion(billetera, billetera->id(), 1);
  EXPECT_EQ(resultado, false);
  EXPECT_EQ(blockchain.transacciones().size(), 1); // sólo transacción semilla
}
