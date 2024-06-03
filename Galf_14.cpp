#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_REGISTROS 3500
#define FILENAME "registros.dat"

typedef struct {
    int matricula;
    int indice;
    int status; // 0 = válido, 1 = dado de baja
} Registro;

typedef struct {
    int matricula;
    int indice;
} Indice;

Registro registros[MAX_REGISTROS];
Indice indices[MAX_REGISTROS];
int num_registros = 0;
int num_indices = 0;

void cargarArchivo();
void guardarArchivo();
void imprimirRegistros(int ordenado);
void generarArchivoTexto(int ordenado);
void empaquetar();
void agregar();
void eliminar();
void buscar();
void ordenar();
void imprimirRegistrosOriginal();
void imprimirRegistrosOrdenados();

int compararIndices(const void *a, const void *b);
int busquedaBinaria(int matricula);

int main() {
    int opcion;

    cargarArchivo();

    do {
        printf("MENU\n");
        printf("1.- AGREGAR\n");
        printf("2.- ELIMINAR\n");
        printf("3.- BUSCAR\n");
        printf("4.- ORDENAR\n");
        printf("5.- IMPRIMIR REGISTROS ARCHIVO ORIGINAL\n");
        printf("6.- IMPRIMIR REGISTROS ARCHIVO ORDENADO\n");
        printf("7.- GENERAR ARCHIVO TEXTO\n");
        printf("8.- EMPAQUETAR\n");
        printf("0.- SALIR\n");
        printf("Selecciona una opción: ");
        scanf("%d", &opcion);

        switch(opcion) {
            case 1:
                agregar();
                break;
            case 2:
                eliminar();
                break;
            case 3:
                buscar();
                break;
            case 4:
                ordenar();
                break;
            case 5:
                imprimirRegistrosOriginal();
                break;
            case 6:
                imprimirRegistrosOrdenados();
                break;
            case 7:
                generarArchivoTexto(1); // 1 para ordenado
                break;
            case 8:
                empaquetar();
                break;
            case 0:
                guardarArchivo();
                printf("Saliendo...\n");
                break;
            default:
                printf("Opción no válida\n");
        }
    } while(opcion != 0);

    return 0;
}

void cargarArchivo() {
    FILE *file = fopen(FILENAME, "rb");
    if (file == NULL) {
        printf("No se pudo abrir el archivo\n");
        return;
    }

    num_registros = fread(registros, sizeof(Registro), MAX_REGISTROS, file);
    fclose(file);

    num_indices = 0;
    for (int i = 0; i < num_registros; i++) {
        if (registros[i].status == 0) {
            indices[num_indices].matricula = registros[i].matricula;
            indices[num_indices].indice = i;
            num_indices++;
        }
    }
}

void guardarArchivo() {
    FILE *file = fopen(FILENAME, "wb");
    if (file == NULL) {
        printf("No se pudo abrir el archivo\n");
        return;
    }

    fwrite(registros, sizeof(Registro), num_registros, file);
    fclose(file);
}

void imprimirRegistros(int ordenado) {
    if (ordenado) {
        for (int i = 0; i < num_indices; i++) {
            int idx = indices[i].indice;
            printf("Matricula: %d, Indice: %d\n", registros[idx].matricula, idx);
        }
    } else {
        for (int i = 0; i < num_registros; i++) {
            if (registros[i].status == 0) {
                printf("Matricula: %d, Indice: %d\n", registros[i].matricula, i);
            }
        }
    }
}

void generarArchivoTexto(int ordenado) {
    char nombreArchivo[100];
    printf("Introduce el nombre del archivo de texto: ");
    scanf("%s", nombreArchivo);

    FILE *file = fopen(nombreArchivo, "w");
    if (file == NULL) {
        printf("No se pudo crear el archivo de texto\n");
        return;
    }

    if (ordenado) {
        for (int i = 0; i < num_indices; i++) {
            int idx = indices[i].indice;
            fprintf(file, "Matricula: %d, Indice: %d\n", registros[idx].matricula, idx);
        }
    } else {
        for (int i = 0; i < num_registros; i++) {
            if (registros[i].status == 0) {
                fprintf(file, "Matricula: %d, Indice: %d\n", registros[i].matricula, i);
            }
        }
    }

    fclose(file);
}

void empaquetar() {
    Registro nuevosRegistros[MAX_REGISTROS];
    int j = 0;

    for (int i = 0; i < num_registros; i++) {
        if (registros[i].status == 0) {
            nuevosRegistros[j] = registros[i];
            j++;
        }
    }

    num_registros = j;
    memcpy(registros, nuevosRegistros, sizeof(nuevosRegistros));

    guardarArchivo();
}

void agregar() {
    if (num_registros >= MAX_REGISTROS) {
        printf("No se pueden agregar más registros\n");
        return;
    }

    int matricula;
    printf("Introduce la matrícula: ");
    scanf("%d", &matricula);

    for (int i = 0; i < num_indices; i++) {
        if (indices[i].matricula == matricula) {
            printf("La matrícula ya existe\n");
            return;
        }
    }

    registros[num_registros].matricula = matricula;
    registros[num_registros].indice = num_registros;
    registros[num_registros].status = 0;

    indices[num_indices].matricula = matricula;
    indices[num_indices].indice = num_registros;

    num_registros++;
    num_indices++;
}

void eliminar() {
    int matricula;
    printf("Introduce la matrícula a eliminar: ");
    scanf("%d", &matricula);

    int idx = busquedaBinaria(matricula);
    if (idx == -1) {
        printf("Matrícula no encontrada\n");
        return;
    }

    printf("Matrícula encontrada. ¿Eliminar? (1=Sí, 0=No): ");
    int eliminar;
    scanf("%d", &eliminar);

    if (eliminar) {
        registros[indices[idx].indice].status = 1;
        printf("Registro eliminado\n");
    } else {
        printf("Operación cancelada\n");
    }
}

void buscar() {
    int matricula;
    printf("Introduce la matrícula a buscar: ");
    scanf("%d", &matricula);

    int idx = busquedaBinaria(matricula);
    if (idx == -1) {
        printf("Matrícula no encontrada\n");
        return;
    }

    printf("Registro encontrado: Matricula: %d, Indice: %d\n", registros[indices[idx].indice].matricula, indices[idx].indice);
}

void ordenar() {
    qsort(indices, num_indices, sizeof(Indice), compararIndices);
}

int compararIndices(const void *a, const void *b) {
    Indice *ia = (Indice *)a;
    Indice *ib = (Indice *)b;
    return ia->matricula - ib->matricula;
}

int busquedaBinaria(int matricula) {
    int izquierda = 0;
    int derecha = num_indices - 1;

    while (izquierda <= derecha) {
        int medio = (izquierda + derecha) / 2;
        if (indices[medio].matricula == matricula) {
            return medio;
        } else if (indices[medio].matricula < matricula) {
            izquierda = medio + 1;
        } else {
            derecha = medio - 1;
        }
    }

    return -1;
}

void imprimirRegistrosOriginal() {
    imprimirRegistros(0);
}

void imprimirRegistrosOrdenados() {
    imprimirRegistros(1);
}
