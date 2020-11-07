SEMAFOROS NECESARIOS
colaDeComidaLlena = 0
colaDeComidaVacia = 10
mesaLimpia = 30
comidaPedida = 0
comidaEntregada = 0
mesaSucia = 0
clientes=50

cocinero {
  repetir:
    espero que se libere un espacio de comida
    preparo comida
    brindo una nueva comida a la cola
}

/*
 * COCINERO IMPLEMENTACION
 */
void* cocinero(void* args) {
  while(1) {
    wait(colaDeComidaVacia); // Espero a que se libere un espacio de comida
    sleep(tiempoDePreparacion); // Preparo comida
    signal(colaDeComidaLlena); // Brindo una nueva comida a la cola
  }
}

cliente {
  repetir:
    espero mesa limpia disponible
    ir a mesa
    pedir comida
    esperar comida
    comer
    libero la mesa
}

/*
 * CLIENTE IMPLEMENTACION
 */

void* cliente(void* args) {
  while (1) {
    wait(mesaLimpia); // Espero mesa limpia disponible
    signal(comidaPedida); // Me senté y pedí la comida
    wait(comidaEntregada); // Esperar comida
    sleep(tiempoComiendo); // Comer
    signal(mesaSucia); // Libero la mesa
  }
}

colaDeComidaLlena = 0
colaDeComidaVacia = 10
mesaLimpia = 29
comidaPedida = 0
comidaEntregada = 0
mesaSucia = 0

camarero {
  repetir:
    espero a que haya un pedido
    espero a que haya una comida
    saco una comida de la cola de comidas
    llevo la comida a un cliente
}

void* camarero(void* args) {
  while(1) {
    wait(comidaPedida); // Espero a que haya un pedido
    wait(colaDeComidaLlena); // Espero a que haya una comida
    signal(colaDeComidaVacia); // Saco una comida de la cola de comidas
    signal(comidaEntregada); // Llevo la comida a un cliente
  }
}

limpiador {
  repetir:
    espero a que una mesa sucia
    limpio mesa mesa
    nueva mesa limpia libre
}

void* limpiador(void* args) {
  wait(mesaSucia); // Espero a que haya una mesa sucia
  sleep(tiempoDeLimpieza); // Limpio la mesa
  signal(mesaLimpia); // Nueva mesa limpia libre
}

Un posible problema es que haya un único cocinero haciendo todas las comidas
