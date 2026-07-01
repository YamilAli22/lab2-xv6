#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"

#define SEM_A 0
#define SEM_B 1

/* para este caso estamos usando los semaforos para ordenacion como señales de turno, donde queremos que un thread haga su trabajo y luego despierte a otro thread indicandole que ahora el puede
hacer su trabajo, y este proceso se repite.
otro caso seria usar los semaforos como contadores de recursos utilizados/utilizables, pej si tengo 3 recursos, maximo 3 threads pueden usarlo, no podria permitir que otro thread quiera usarlo
*/ 

int 
main(int argc, char **argv)
{
  if (argc != 2) {
    fprintf(2, "ERROR: se debe pasar un solo argumento, la cantidad de rondas del pingpong\n");
    return 0;
  }

  int rounds = atoi(argv[1]);

  if (rounds <= 1) {
    fprintf(2, "ERROR: debe haber 2 o mas rondas\n");
    return 0;
  }

  sem_open(SEM_A, 1);
  sem_open(SEM_B, 0);

  int rc = fork();
  if (rc == -1) {
    fprintf(2, "ERROR: fallo el fork\n");
    return 0;
  }

  else if (rc == 0) {
    for (unsigned int i = 0; i < rounds; ++i) {
      sem_down(SEM_B);
      printf("\tPONG\n");
      sem_up(SEM_A); // signal for SEM_A (PING)
    }
    return 1;
  } else {
    for (unsigned int i = 0; i < rounds; ++i) {
      sem_down(SEM_A);
      printf("PING\n");
      sem_up(SEM_B); // signal for SEM_B (PONG)
    }
  }
  wait(0);
  sem_close(SEM_A);
  sem_close(SEM_B);

  return 1;
}
