#include <vector>
#include "lib.h"
#include "calendario.h"
#include "billetera.h"
#include "blockchain.h"

using namespace std;

Billetera::Billetera(const id_billetera id, Blockchain* blockchain)
  : _id(id)
  , _blockchain(blockchain)
  , _saldo(0) {
}

id_billetera Billetera::id() const {
  return _id;
}


void Billetera::notificar_transaccion(Transaccion t) { // O(C)
  _transacciones.push_back(t);

  _actualizar_saldo(t);
  _actualizar_saldo_por_dia(t);

  id_billetera billetera_amigo = _conseguir_billetera_amigo(t);
  if(billetera_amigo != 0 && t.destino == billetera_amigo) { // Si no es a la semilla y envié dinero
    _actualizar_billeteras_por_cantidad_de_transacciones(t);
  }
}

monto Billetera::saldo() const {
  return _saldo; // O(1)

  // COMPLEJIDAD TOTAL DEL MÉTODO: O(1)
}

monto Billetera::saldo_al_fin_del_dia(timestamp t) const {
  timestamp dia = Calendario::fin_del_dia(t); // O(1)
  
  // Nos fijamos si el dia pedido es "en el futuro"
  auto it = _saldo_por_dia.rbegin(); // O(1)
  timestamp ultimo_dia_guardado = it->first; // O(1)
  if(ultimo_dia_guardado < dia) { // O(1)
    return _saldo; // O(1)
  }
  
  return _saldo_por_dia.at(dia); // O(log D)

  // COMPLEJIDAD TOTAL DEL MÉTODO: O(log D)
  //   - 5 operaciones O(1) + O(log D)
  //   - 5*O(1) + O(log D) = O(1) + O(log D) = O(log D)
}

vector<Transaccion> Billetera::ultimas_transacciones(int k) const { // O(k)
  vector<Transaccion> ret; // O(1)
  
  // Notar que `rbegin` y `rend` recorren la lista en orden inverso.
  auto it = _transacciones.rbegin(); // O(1)
  
  // Complejidad total del ciclo: O(k)
  //   - O(k) iteraciones
  //   - O(1) operaciones en cada iteración
  while(it != _transacciones.rend() && ret.size() < k) { // O(k) iteraciones. Con 0 <= k <= _transacciones.size() (cantidad de trx que hizo Billetera).
    ret.push_back(*it); // O(1)
    ++it; // O(1)
  }

  return ret; // O(1)

  // COMPLEJIDAD TOTAL DEL MÉTODO: O(k)
  //   - 3 operaciones O(1) + O(k)
  //   - 3*O(1) + O(k) = O(1) + O(k) = O(k)
}

vector<id_billetera> Billetera::detinatarios_mas_frecuentes(int k) const { // O(k)
  // recorro el map usando un iterador en orden inverso, que me lleva por todos
  // los pares de entradas desde la mayor clave hasta la menor.
  vector<id_billetera> ret = {};  // O(1)
  auto it = _billeteras_por_cantidad_de_transacciones.rbegin(); // O(1)

  // PREGUNTAR MAÑANA A JUAN ¿O(k/min(i,k))?
  while (it != _billeteras_por_cantidad_de_transacciones.rend() && ret.size() < k) { // O(k/i) donde i = it->second.size() en cada ciclo. En el peor caso, i = C.
    vector<id_billetera> siguiente_grupo = it->second; // O(1)
    int i = 0; // O(1)

    // Complejidad total del ciclo: O(min(i,k))
    //   - O(min(i,k)) iteraciones
    //   - O(1) operaciones en cada iteración
    while (i < siguiente_grupo.size() && ret.size() < k) { // O(min(i,k))
      ret.push_back(siguiente_grupo[i]); // O(1)
      i++; // O(1)
    }

    ++it; // O(1)
  }

  return ret; // O(1)

  // COMPLEJIDAD TOTAL DEL MÉTODO: O(k)
  //   - 3 operaciones O(1) + O(k)
  //   - 3*O(1) + O(k) = O(1) + O(k) = O(k)
}


/** Métodos privados auxiliares */

id_billetera Billetera::_conseguir_billetera_amigo(Transaccion t) {
  if(t.origen == _id) {
    return t.destino;
  }
  return t.origen;
}

void Billetera::_actualizar_saldo(Transaccion t) {
  // Si envié dinero
  if(t.origen == _id) {
    _saldo -= t.monto;
    return;
  }
  // Si recibí
  _saldo += t.monto;
}

void Billetera::_actualizar_saldo_por_dia(Transaccion t) {
  const timestamp fin_del_dia = Calendario::fin_del_dia(t._timestamp); // O(1)

  id_billetera billetera_amigo = _conseguir_billetera_amigo(t); // O(1)

  // Si es la trx semilla.
  if (billetera_amigo == 0) { // O(1)
    _saldo_por_dia[fin_del_dia] = _saldo; // O(1) (esta vacio)
    return; // O(1)
  }

  auto it_saldos = _saldo_por_dia.rbegin(); // O(1)
  const timestamp ultimo_dia_con_saldo = it_saldos->first; // O(1)

  const monto ultimo_saldo = it_saldos->second; // O(1)

  // Complejidad total del ciclo: O((D-1) * log D)
  //   - O(D-1) iteraciones
  //   - O(log D) operaciones en cada iteración
  for (timestamp i = Calendario::dia_siguiente(ultimo_dia_con_saldo); i < fin_del_dia; i = Calendario::dia_siguiente(i)) { // O(D-1) iteraciones.
    _saldo_por_dia[i] = ultimo_saldo; // O(log D)
  }
  
  _saldo_por_dia[fin_del_dia] = _saldo; // O(log D)

  // COMPLEJIDAD TOTAL DEL MÉTODO: O(D log(D))
  //   - 6 operaciones O(1) + O((D-1) * log D) + O(log D)
  //   - 6*O(1) + O((D-1) * log D) + O(log D) = O(1) + O(D*log(D)) = O(D*log(D))
}

void Billetera::_actualizar_billeteras_por_cantidad_de_transacciones(Transaccion t) {
  id_billetera billetera_amigo = _conseguir_billetera_amigo(t);

  bool encontrado = false;
  auto it = _billeteras_por_cantidad_de_transacciones.begin();  

  while (it != _billeteras_por_cantidad_de_transacciones.end() && !encontrado) { // O(C/X)
    for (int i = 0; i < it->second.size() && !encontrado; i++) { // O(X)
      if (billetera_amigo == it->second[i]) {
        _actualizar_cantidad_transacciones_billetera_amigo(it, billetera_amigo, i);
        encontrado = true;
      }
    }
    ++it;
  }

  if(!encontrado) {
    _billeteras_por_cantidad_de_transacciones[1].push_back(billetera_amigo); // O(1)
  }
}

void Billetera::_actualizar_cantidad_transacciones_billetera_amigo(map<int, vector<id_billetera>>::iterator it, id_billetera billetera_amigo, int i) {
  // BORRAR AL AMIGO
  it->second[i] = it->second[it->second.size() - 1]; // O(1)
  it->second.pop_back(); // O(1)
  
  // AGREGAR AMIGO AL SIGUIENTE
  auto it2 = ++it;
  if(it2 != _billeteras_por_cantidad_de_transacciones.end()) {
    it2->second.push_back(billetera_amigo); // O(1)
    return;
  }
  _billeteras_por_cantidad_de_transacciones[it->first + 1].push_back(billetera_amigo); // O(log n)
}

//////////////////////////////////