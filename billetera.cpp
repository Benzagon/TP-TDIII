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
  id_billetera amigo;
  if(t.origen == _id) {
    _saldo -= t.monto;
    amigo = t.destino;
  }
  else if (t.destino == _id) {
    _saldo += t.monto;
    amigo = t.origen;
  }

  if (amigo == 0) {
    const timestamp fin_del_dia = Calendario::fin_del_dia(t._timestamp);
    _saldo_por_dia[fin_del_dia] = _saldo; // O(1) (esta vacio) 
  }
  else {
    const timestamp fin_del_dia = Calendario::fin_del_dia(t._timestamp);
    auto itSaldos = _saldo_por_dia.rbegin();
    const timestamp ultimo_dia_con_saldo = itSaldos->first;

    const monto ultimo_saldo = itSaldos->second;

    for (timestamp i = Calendario::dia_siguiente(ultimo_dia_con_saldo); i < fin_del_dia; i = Calendario::dia_siguiente(i)) { // O((D-1)*log(D))
      _saldo_por_dia[i] = ultimo_saldo;
    }
    _saldo_por_dia[fin_del_dia] = _saldo; // O(log(D))
  }
  
  if(amigo != 0 && t.destino == amigo) { // Si no es a la semilla    
    auto it = _billeteras_por_cantidad_de_transacciones.begin();  
    bool encontrado = false;
    while (it != _billeteras_por_cantidad_de_transacciones.end() && !encontrado) { // O(C/X)
      for (int i = 0; i < it->second.size() && !encontrado; i++) { // O(X)
        if (amigo == it->second[i]) {
          // BORRAR AL AMIGO
          it->second[i] = it->second[it->second.size() - 1]; // O(1)
          it->second.pop_back(); // O(1)
          // AGREGAR AMIGO AL SIGUIENTE
          auto it2 = ++it;
          if(it2 != _billeteras_por_cantidad_de_transacciones.end()) {
            it2->second.push_back(amigo); // O(1)
          }
          else {
            _billeteras_por_cantidad_de_transacciones[it->first + 1].push_back(amigo); // O(log n)
          }
          encontrado = true;
        }
      }
      ++it;
    }

    if(!encontrado) {
      _billeteras_por_cantidad_de_transacciones[1].push_back(amigo); // O(1)
    }
  }
}

monto Billetera::saldo() const { // O(1)
  return _saldo; // O(1)
}

monto Billetera::saldo_al_fin_del_dia(timestamp t) const {
  timestamp dia = Calendario::fin_del_dia(t);
  return _saldo_por_dia.at(dia); // O(log D)    PREGUNTAR PORQUE NO FUNCIONA _saldop_por_dia[dia]
}

vector<Transaccion> Billetera::ultimas_transacciones(int k) const { // O(k)
  vector<Transaccion> ret; // O(1)
  // Notar que `rbegin` y `rend` recorren la lista en orden inverso.
  auto it = _transacciones.rbegin(); // O(1)
  while(it != _transacciones.rend() && ret.size() < k) { // O(k)
    ret.push_back(*it); // O(1)
    ++it; // O(1)
  }

  return ret; // O(1)
}

vector<id_billetera> Billetera::detinatarios_mas_frecuentes(int k) const { // O(k)
  // recorro el map usando un iterador en orden inverso, que me lleva por todos
  // los pares de entradas desde la mayor clave hasta la menor.
  vector<id_billetera> ret = {};
  auto it = _billeteras_por_cantidad_de_transacciones.rbegin();
  while (it != _billeteras_por_cantidad_de_transacciones.rend() && ret.size() < k) {
    vector<id_billetera> siguiente_grupo = it->second;
    int i = 0;
    while (i < siguiente_grupo.size() && ret.size() < k) {
      ret.push_back(siguiente_grupo[i]);
      i++;
    }
    ++it;
  }

  return ret;
}
