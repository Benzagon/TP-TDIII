#ifndef CALENDARIO_H_
#define CALENDARIO_H_

#include "lib.h"
#include <ctime>
#include <cstdlib>

class Calendario {
  public:
    /*
     * Retorna el tiempo actual. Si `fijar_tiempo_actual` fue llamado, se
     retorna el valor con el que se usó.
     *
     * Complejidad: O(1)
     */
    static timestamp tiempo_actual() {
      if (valor_fijado != -1) {
        return valor_fijado;
      }

      return static_cast<unsigned int>(std::time(nullptr));
    }

    /*
     * Retorna el timestamp asociado al principio del día del timestamp que se
     provee.
     *
     * Complejidad: O(1)
     */
    static timestamp principio_del_dia(timestamp t) {
      return t - (t % DURACION_DIA);
    }

    /*
     * Retorna el timestamp asociado al fin del día del timestamp que se
     provee.
     *
     * Complejidad: O(1)
     */
    static timestamp fin_del_dia(timestamp t) {
      return dia_siguiente(principio_del_dia(t));
    }

    /*
    * Retorna el timestamp asociado al mismo horario de `t`, pero del día
    * siguiente.
    *
    * A modo de ejemplo, las dos expresiones siguientes deberían ser equivalentes:
    *   - fin_del_dia(t)
    *   - dia_siguiente(principio_del_dia(t)).
    *
    * Complejidad: O(1)
    */
    static timestamp dia_siguiente(timestamp t) {
      return t + DURACION_DIA;
    }

    //--------------------------------------------------------------------------
    // Las funciones de aquí en más se utilizan para proveer una forma sencilla
    // de controlar el tiempo actual en los tests.
    //
    // No deberían usar ni modificar esto.
    //--------------------------------------------------------------------------

    // Se usa para poder controlar el tiempo en tests.
    static void fijar(timestamp t) {
      valor_fijado = t;
    }

    static timestamp dia(unsigned int n) {
      return n * DURACION_DIA;
    }

    static void avanzar_un_dia() {
      if (valor_fijado != -1) {
        valor_fijado += DURACION_DIA;
      }
    }

    static void avanzar_un_minuto() {
      if (valor_fijado != -1) {
        valor_fijado += 60;
      }
    }

    // Restaura el comportamiento normal
    static void restaurar() {
      valor_fijado = -1;
    }

  private:
    static const timestamp DURACION_DIA = 86400;

    static timestamp valor_fijado;

    static void init() {
      valor_fijado = -1;
      srand(static_cast<unsigned int>(time(nullptr)));
    }
};

#endif // CALENDARIO_H_
