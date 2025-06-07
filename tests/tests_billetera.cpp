#include <string>
#include <cassert>
#include <gtest/gtest.h>

#include "../calendario.h"
#include "../lib.h"
#include "../billetera.h"
#include "tests_lib.h"

using namespace std;

class test_billetera : public ::testing::Test {
protected:
    void SetUp() override    { Calendario::restaurar(); }
    void TearDown() override { Calendario::restaurar(); }
};

TEST_F(test_billetera, permite_consultar_el_saldo) {
  Blockchain blockchain;

  Billetera* billetera1 = blockchain.abrir_billetera();
  Billetera* billetera2 = blockchain.abrir_billetera();
  Billetera* billetera3 = blockchain.abrir_billetera();

  EXPECT_EQ(billetera1->saldo(), 100);
  EXPECT_EQ(billetera2->saldo(), 100);
  EXPECT_EQ(billetera3->saldo(), 100);

  agregar_transaccion(blockchain, billetera1, billetera2, 10);

  EXPECT_EQ(billetera1->saldo(), 90);
  EXPECT_EQ(billetera2->saldo(), 110);
  EXPECT_EQ(billetera3->saldo(), 100);

  agregar_transaccion(blockchain, billetera2, billetera3, 50);

  EXPECT_EQ(billetera1->saldo(), 90);
  EXPECT_EQ(billetera2->saldo(), 60);
  EXPECT_EQ(billetera3->saldo(), 150);

  agregar_transaccion(blockchain, billetera3, billetera1, 15);

  EXPECT_EQ(billetera1->saldo(), 105);
  EXPECT_EQ(billetera2->saldo(), 60);
  EXPECT_EQ(billetera3->saldo(), 135);
}

TEST_F(test_billetera, permite_listar_ultimas_transacciones) {
  Blockchain blockchain;

  Billetera* billetera1 = blockchain.abrir_billetera();
  Billetera* billetera2 = blockchain.abrir_billetera();

  agregar_transaccion(blockchain, billetera1, billetera2, 10);
  agregar_transaccion(blockchain, billetera2, billetera1, 30);
  agregar_transaccion(blockchain, billetera1, billetera2, 5);

  vector<Transaccion> ultimas2 = billetera1->ultimas_transacciones(2);

  EXPECT_EQ(ultimas2.size(), 2);
  chequear_transaccion(ultimas2[0], billetera1->id(), billetera2->id(), 5);
  chequear_transaccion(ultimas2[1], billetera2->id(), billetera1->id(), 30);

  vector<Transaccion> ultimas3 = billetera1->ultimas_transacciones(3);
  EXPECT_EQ(ultimas3.size(), 3);
  chequear_transaccion(ultimas3[0], billetera1->id(), billetera2->id(), 5);
  chequear_transaccion(ultimas3[1], billetera2->id(), billetera1->id(), 30);
  chequear_transaccion(ultimas3[2], billetera1->id(), billetera2->id(), 10);

  vector<Transaccion> ultimas4 = billetera1->ultimas_transacciones(4);
  EXPECT_EQ(ultimas4.size(), 4);
  chequear_transaccion(ultimas3[0], billetera1->id(), billetera2->id(), 5);
  chequear_transaccion(ultimas3[1], billetera2->id(), billetera1->id(), 30);
  chequear_transaccion(ultimas3[2], billetera1->id(), billetera2->id(), 10);
  chequear_transaccion(ultimas4[3], 0, billetera1->id(), 100); // transacción semilla
}

TEST_F(test_billetera, ultimas_transacciones_devuelve_todas_las_transacciones_si_se_pide_mas_del_total) {
  Blockchain blockchain;

  Billetera* billetera = blockchain.abrir_billetera();
  EXPECT_EQ(billetera->ultimas_transacciones(10).size(), 1); // sólo transacción semilla
}

TEST_F(test_billetera, ultimas_transacciones_no_lista_operaciones_entre_otras_billeteras) {
  Blockchain blockchain;

  Billetera* billetera1 = blockchain.abrir_billetera();
  Billetera* billetera2 = blockchain.abrir_billetera();
  Billetera* billetera3 = blockchain.abrir_billetera();

  agregar_transaccion(blockchain, billetera2, billetera3, 10);

  EXPECT_EQ(billetera1->ultimas_transacciones(2).size(), 1); // sólo transacción semilla
  EXPECT_EQ(billetera2->ultimas_transacciones(2).size(), 2);
  EXPECT_EQ(billetera3->ultimas_transacciones(2).size(), 2);
}

TEST_F(test_billetera, permite_consultar_el_saldo_al_final_de_un_dia) {
  Blockchain blockchain;
  Calendario::fijar(0);

  Billetera* billetera1 = blockchain.abrir_billetera();
  Billetera* billetera2 = blockchain.abrir_billetera();

  // --------------------

  agregar_transaccion(blockchain, billetera1, billetera2, 10);

  Calendario::avanzar_un_minuto();
  agregar_transaccion(blockchain, billetera2, billetera1, 70);

  EXPECT_EQ(billetera1->saldo(), 160);
  EXPECT_EQ(billetera2->saldo(), 40);

  // --------------------

  Calendario::avanzar_un_dia();
  agregar_transaccion(blockchain, billetera1, billetera2, 5);

  Calendario::avanzar_un_minuto();
  agregar_transaccion(blockchain, billetera2, billetera1, 20);

  EXPECT_EQ(billetera1->saldo(), 175);
  EXPECT_EQ(billetera2->saldo(), 25);

  // --------------------

  Calendario::avanzar_un_dia();
  Calendario::avanzar_un_minuto();
  agregar_transaccion(blockchain, billetera1, billetera2, 100);

  EXPECT_EQ(billetera1->saldo(), 75);
  EXPECT_EQ(billetera2->saldo(), 125);

  // --------------------

  EXPECT_EQ(billetera1->saldo_al_fin_del_dia(Calendario::dia(0)), 160);
  EXPECT_EQ(billetera2->saldo_al_fin_del_dia(Calendario::dia(0)), 40);

  EXPECT_EQ(billetera1->saldo_al_fin_del_dia(Calendario::dia(1)), 175);
  EXPECT_EQ(billetera2->saldo_al_fin_del_dia(Calendario::dia(1)), 25);

  EXPECT_EQ(billetera1->saldo_al_fin_del_dia(Calendario::dia(2)), 75);
  EXPECT_EQ(billetera2->saldo_al_fin_del_dia(Calendario::dia(2)), 125);
}

TEST_F(test_billetera, si_consulto_saldo_entre_creacion_y_primera_transaccion_real_retorna_el_saldo_inicial) {
  // IMPORTANTE: No se testea el caso en que consultamos por una fecha previa a
  // la creación de la billetera ya que este caso no está admitido por la
  // precondición de la función.

  Blockchain blockchain;

  Calendario::fijar(0);
  Billetera* billetera1 = blockchain.abrir_billetera();
  Billetera* billetera2 = blockchain.abrir_billetera();

  Calendario::avanzar_un_dia();
  Calendario::avanzar_un_dia();
  agregar_transaccion(blockchain, billetera1, billetera2, 10);

  // primera transaccion en el día 2, consulto por día 1
  EXPECT_EQ(billetera1->saldo_al_fin_del_dia(Calendario::dia(1)), 100);
}

TEST_F(test_billetera, si_consulto_saldo_después_de_la_ultima_transaccion_devuelve_saldo_actual) {
  Blockchain blockchain;

  Calendario::fijar(0);

  Billetera* billetera1 = blockchain.abrir_billetera();
  Billetera* billetera2 = blockchain.abrir_billetera();

  agregar_transaccion(blockchain, billetera1, billetera2, 10);

  EXPECT_EQ(billetera1->saldo_al_fin_del_dia(Calendario::dia(3)), 90);
}

TEST_F(test_billetera, permite_consultar_los_destinatarios_mas_frecuentes) {
  Blockchain blockchain;

  Billetera* billetera1 = blockchain.abrir_billetera();
  Billetera* billetera2 = blockchain.abrir_billetera();
  Billetera* billetera3 = blockchain.abrir_billetera();
  Billetera* billetera4 = blockchain.abrir_billetera();

  // 20 transacciones a billetera2
  for(int i = 0; i < 20; i++) {
    agregar_transaccion(blockchain, billetera1, billetera2, 1);
  }

  // 10 transacciones a billetera3
  for(int i = 0; i < 10; i++) {
    agregar_transaccion(blockchain, billetera1, billetera3, 1);
  }

  // 30 transacciones a billetera4
  for(int i = 0; i < 30; i++) {
    agregar_transaccion(blockchain, billetera1, billetera4, 1);
  }

  chequear_ids_billeteras(billetera1->detinatarios_mas_frecuentes(1), { billetera4->id() });
  chequear_ids_billeteras(billetera1->detinatarios_mas_frecuentes(2), { billetera4->id(), billetera2->id() });
  chequear_ids_billeteras(billetera1->detinatarios_mas_frecuentes(3), { billetera4->id(), billetera2->id(), billetera3->id() });
  chequear_ids_billeteras(billetera1->detinatarios_mas_frecuentes(4), { billetera4->id(), billetera2->id(), billetera3->id() });
}

TEST_F(test_billetera, destinatarios_mas_frecuentes_solo_cuenta_transacciones_salientes) {
  Blockchain blockchain;

  Billetera* billetera1 = blockchain.abrir_billetera();
  Billetera* billetera2 = blockchain.abrir_billetera();
  Billetera* billetera3 = blockchain.abrir_billetera();

  // 2 transacciones salientes con billetera 2
  agregar_transaccion(blockchain, billetera1, billetera2, 1);
  agregar_transaccion(blockchain, billetera1, billetera2, 1);

  // 1 transacciones saliente + 3 entrantes con billetera3
  agregar_transaccion(blockchain, billetera1, billetera3, 1);
  agregar_transaccion(blockchain, billetera3, billetera1, 1);
  agregar_transaccion(blockchain, billetera3, billetera1, 1);
  agregar_transaccion(blockchain, billetera3, billetera1, 1);

  chequear_ids_billeteras(
    billetera1->detinatarios_mas_frecuentes(2),
    { billetera2->id(), billetera3->id() }
  );
}
