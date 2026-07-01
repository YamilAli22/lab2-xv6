#include "types.h"
#include "spinlock.h"
#include "param.h"
#include "types.h"
#include "riscv.h"
#include "defs.h"

#define MAX_SEM 256

/*
esta es la estructura del spinlock que usa xv6, definida en spinlock.h:
struct spinlock {
  uint locked;       // Is the lock held?

  For debugging:
  char *name;        // Name of lock.
  struct cpu *cpu;   // The cpu holding the lock.
};
*/ 

struct semaphore {
  struct spinlock s_lock;
  int value;
  int max_value;
  int is_open; 
};

struct semaphore sem_table[MAX_SEM];

// esta funcion inicializa y deja los semaforos en un estado "limpio" para ser usado luego en sem_open, sem_close, etc
void 
init_sems() 
{
  for (unsigned int i = 0; i < MAX_SEM; ++i) {
    sem_table[i].is_open = 0;
    sem_table[i].max_value = 0;
    sem_table[i].value = 0;
    initlock(&(sem_table[i].s_lock), "semaphore");
  }
}

int 
sem_open(int sem, int value) 
{
    if (value < 0) {
      printf("El valor no puede ser negativo (%d)\n", sem);
      return 0;
    }
    if (sem < 0 || sem >= MAX_SEM) {
      printf("ID de semaforo inválido (%d)\n", sem);
      return 0;
    }
    acquire(&(sem_table[sem].s_lock));
    if (sem_table[sem].is_open == 1) {
      release(&(sem_table[sem].s_lock));
      printf("El semaforo con ID: %d ya está abierto.\n", sem);
      return 0;
    }
    sem_table[sem].value = value;
    sem_table[sem].max_value = value;
    sem_table[sem].is_open = 1;
    release(&(sem_table[sem].s_lock));

    return 1;
}

int 
sem_close(int sem) {
    if (sem < 0 || sem >= MAX_SEM) {
      printf("ID de sem inválido (%d)\n", sem);
      return 0;
    }
    acquire(&(sem_table[sem].s_lock));
    if (sem_table[sem].is_open == 0) {
      release(&(sem_table[sem].s_lock));
      printf("El semaforo %d no esta abierto\n", sem);
      return 0;
    }

    sem_table[sem].is_open = 0;
    sem_table[sem].value = 0;
    sem_table[sem].max_value = 0; // estos ultimos dos los seteo en 0 para dejar el sem "limpio" otra vez, aunque no se si es necesario
    release(&(sem_table[sem].s_lock));
    return 1;
}

int
sem_up(int sem) {
  if (sem < 0 || sem >= MAX_SEM) {
    printf("ID de sem invalido (%d)", sem);
    return 0;
  }

  acquire(&(sem_table[sem].s_lock));
  
  if (sem_table[sem].is_open == 0) {
    printf("Error, no se puede hacer up de un semaforo cerrado\n");
    release(&(sem_table[sem].s_lock));
    return 0;
  }
  
  if (sem_table[sem].value == 0) {
    wakeup(&(sem_table[sem])); // acá es como si avisara que hay lugar para que entren a usar X recurso, aunque podria no haber procesos esperando para escuchar ese aviso
  }
  sem_table[sem].value++;
  release(&(sem_table[sem].s_lock));

  return 1;
}

int
sem_down(int sem) {
  if (sem < 0 || sem >= MAX_SEM) {
    printf("ID de sem invalido (%d)", sem);
    return 0;
  }

  acquire(&(sem_table[sem].s_lock));
  if (sem_table[sem].is_open == 0) {
    printf("Error, no se puede hacer down de un semaforo cerrado\n");
    release(&(sem_table[sem].s_lock));
    return 0;
  }

  while (sem_table[sem].value == 0) {
    sleep(&(sem_table[sem]), &(sem_table[sem].s_lock));
  }
  sem_table[sem].value--;
  release(&(sem_table[sem].s_lock));
  return 1; 
}






