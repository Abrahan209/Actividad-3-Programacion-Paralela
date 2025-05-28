#include <stdio.h>
#include <windows.h>

#define NUM_HILOS 5
#define ITERACIONES 1000

volatile LONG variable_compartida = 0; // volatile para evitar optimizaciones

HANDLE semaforo;

DWORD WINAPI incrementar(LPVOID arg) {
    for (int i = 0; i < ITERACIONES; i++) {
        WaitForSingleObject(semaforo, INFINITE); // Espera para entrar a la sección crítica
        variable_compartida++;                    // Sección crítica
        ReleaseSemaphore(semaforo, 1, NULL);     // Libera el semáforo
    }
    return 0;
}

int main() {
    HANDLE hilos[NUM_HILOS];

    // Crear semáforo con valor inicial 1 (solo un hilo entra a la vez)
    semaforo = CreateSemaphore(NULL, 1, 1, NULL);
    if (semaforo == NULL) {
        printf("Error al crear el semáforo: %lu\n", GetLastError());
        return 1;
    }

    // Crear hilos
    for (int i = 0; i < NUM_HILOS; i++) {
        hilos[i] = CreateThread(NULL, 0, incrementar, NULL, 0, NULL);
        if (hilos[i] == NULL) {
            printf("Error al crear hilo %d: %lu\n", i, GetLastError());
            return 1;
        }
    }

    // Esperar a que todos los hilos terminen
    WaitForMultipleObjects(NUM_HILOS, hilos, TRUE, INFINITE);

    // Mostrar resultado
    printf("Valor final de la variable compartida: %ld\n", variable_compartida);

    // Cerrar handles
    for (int i = 0; i < NUM_HILOS; i++) {
        CloseHandle(hilos[i]);
    }
    CloseHandle(semaforo);

    return 0;
}