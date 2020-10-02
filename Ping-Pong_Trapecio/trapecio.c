/*
 * Entrada:  Numero de trapecios
 * Salida:   Estimación de la integral de "a" a "b" de f(x)
 *           usando la regla trapezoidal y "n" trapezoides..
 *
 * Uso:    mpirun -np <numero de procesos> ./mpi_trap <numero de trapezoides>
 *
 * Algoritmo:
 *  1. Cada proceso calcula "su" intervalo de integración.
 *  2. Cada proceso estima la integral de f (x) durante su intervalo utilizando la regla trapezoidal.
 *  3.1. Cada proceso != 0 envía su integral a 0.
 *  3.2. El proceso 0 suma los cálculos recibidos del individuo procesa e imprime el resultado.
 *
 * Nota: f(x) = (0.2(x-4)^3)-(2x)+12
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

const double a = 0;
const double b = 10;

/* Declaraciones de funciones */
void Get_input(int argc, char* argv[], int my_rank, double* n_p);
double Trap(double left_endpt, double right_endpt, int trap_count, double base_len);
double f(double x);

int main(int argc, char** argv) {
  int my_rank, comm_sz, local_n;
  double n, h, local_a, local_b;
  double local_int, total_int;
  double start, finish, loc_elapsed, elapsed;

  MPI_Init(NULL, NULL);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

  /*Imprime en consola el nucleo que ejecuto el proceso*/
  printf("soy el core nro. %d de %d\n", my_rank, comm_sz);

  Get_input(argc, argv, my_rank, &n); /*Leer la entrada del usuario*/

  /*Nota: h y local_n son iguales para todos los procesos */
  h = (b - a) / n;          /* longitud de cada trapecio */
  local_n = n / comm_sz;  /* cantidad de trapecios por proceso */

  /* Duración del intervalo de integración de cada proceso = local_n * h. */
  local_a = a + my_rank * local_n * h;
  local_b = local_a + local_n * h;

  MPI_Barrier(MPI_COMM_WORLD);
  start = MPI_Wtime();

  /* Calcular la integral local de cada proceso utilizando puntos finales locales*/
  local_int = Trap(local_a, local_b, local_n, h);
  finish = MPI_Wtime();
  loc_elapsed = finish - start;
  MPI_Reduce(&loc_elapsed, &elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

  /* Suma las integrales calculadas por cada proceso */
  MPI_Reduce(&local_int, &total_int, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);


  if (my_rank == 0) {
    printf("Con n = %.0f trapezoides, el valor de la integral entre %.0f a %.0f = %f \n", n, a, b, total_int);
    printf("Tiempo transcurrido = %f millisegundos \n", elapsed * 1000);
  }

  /* Cerrar MPI */
  MPI_Finalize();

  return 0;
} /*  main  */


/*------------------------------------------------ ------------------
* Función: Get_input
* Propósito : obtener la entrada del usuario : el número de trapecios
**
* Args de entrada :
* 1. my_rank: rango de proceso en MPI_COMM_WORLD
* 2. comm_sz : número de procesos en MPI_COMM_WORLD
**
* Args de salida : 
* 1. n_p: puntero al número de trapecios
*/
void Get_input(int argc, char* argv[], int my_rank, double* n_p) {
  if (my_rank == 0) {
    if (argc != 2) {
      fprintf(stderr, "uso: mpirun -np <N> %s <numero de trapezoides> \n", argv[0]);
      fflush(stderr);
      *n_p = -1;
    }
    else {
      *n_p = atoi(argv[1]);
    }
  }
  // Transmite el valor de n a cada proceso
  MPI_Bcast(n_p, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  // n negativo termina el programa
  if (*n_p <= 0) {
    MPI_Finalize();
    exit(-1);
  }
}  /* Get_input */

/*------------------------------------------------ ------------------
* Función: Trap
* Propósito : función en serie para estimar una integral definida usando la regla trapezoidal
**
* Args de entrada : 
* -left_endpt
* -right_endpt
* -trap_count
* -base_len
** 
* Valor de retorno : estimación de la regla trapezoidal de la integral de 
* left_endpt a right_endpt usando trap_count trapecios
*/
double Trap(double left_endpt, double right_endpt, int trap_count, double base_len) {
  double estimate, x;
  int i;

  estimate = (f(left_endpt) + f(right_endpt)) / 2.0;
  for (i = 1; i <= trap_count - 1; i++) {
    x = left_endpt + i * base_len;
    estimate += f(x);
  }
  estimate = estimate * base_len;

  return estimate;
} /*  Trap  */


/*------------------------------------------------ ------------------
* Función: f
* Propósito : Calcular el valor de la función a integrar
* Args de entrada : x
*/
double f(double x) { 
    double x1;
    double x2;
    x1 = ((x-4.0)*(x-4.0)*(x-4.0));
    x2 = 2.0*x;
    return ((0.2*x1)-x2)+12.0;;
} /* f */
