#define TIME 1000


int main() {
  pid1 = fork()
  if (pid1>0) {
    fork()
    >0
      fork()
      >0
        leercantidadtareas;
        while(strcmp) && <=6) {
            write(pipe,leercantidadtareas);
            read(pipe,"termine");
          }
        }
        exit(0)

  } else if(pid==0) {
    ejecutarProceso4()
  }

}

ejecutarProceso4() {
  taskA();
  tasktA();
  tasktB();
  tastkB();
  write(pipe,"termine")
}

ejecutarProces5() {
  taskA();
  tasktA();
  tasktB();
  tasktC();
  tasktC();
  write(pipe,"termine")
}

ejecutarProceso6() {
  pthread_t hilos[];
  taskA();
  tasktA();
  tasktB();
  tasktB();
  tasktC();
  tasktC();
  write(pipe,"termine")
}
Tarea A: esta tarea se encarga de realizar trabajos de pintura sobre un vehı́culo.
Cuando se requiere una tarea de este tipo se le informa el color y el tipo de
trabajo. El tipo de trabajo puede ser parcial o total. Un trabajo de tipo parcial
requiere una unidad de tiempo para realizarse y en un tipo total requiere tres
unidades de tiempo.

Algoritmo:
DE: color: String, tipoDeTrabajo: boolean -> false = parcial, true = total

void taskA(args) {
  char* color = args.c;
  int type = args.t;
  if (type)
    sleep(TIME*3);
  else
    sleep(TIME;
}



Tarea B: esta tarea se encarga de realizar trabajos en los frenos de un vehı́culo.
Cuando se requiere una tarea de este tipo se le informa el tipo de trabajo. El
tipo puede ser verificación o reparación. En el caso de la verificación requiere de
una unidad de tiempo para realizarse y una reparación requiere 2 unidades de
tiempo.

void taskB(args) {
    int type= args.t;
    if(type)
      sleep(TIME);
    else
      sleep(TIME*2);

}



Tarea C: esta tarea se encarga de realizar trabajos sobre las ruedas de un
vehı́culo. Cuando se requiere una tarea de este tipo se le informa el tipo de
trabajo. El tipo puede ser reparación de una llanta o rotación y balanceo. En
el caso de la reparación requiere de una unidad de tiempo para cada una de
las ruedas que deba reparar y una rotación y balanceo requiere 3 unidades de
tiempo.

void taskC() {

}





-----------------------------------------------------------------------------
-----------------------------------------------------------------------------
