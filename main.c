#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "math.h"
#include <ctype.h>
#include "list.h"
#include "treemap.h"
#define MAXLIBROS 101
#define MAXCONTENIDO 1000000
#define MAXLINEA 256
#define MAXPALABRA 10000

//================================================
//Estructuras ocupadas para nuestro programa
//================================================
typedef struct {
    int ID;
    char titulo[MAXLINEA];
    char nombreArchivo[MAXLIBROS];
    int numPalabras;
    int numCaracteres;
    char contenido[MAXCONTENIDO];
} Libro; 

typedef struct {
    char palabra[MAXPALABRA];
    float tfidf;
} PalabraTFIDF;

typedef struct {
    int contador;
    char palabra[MAXPALABRA];
} Palabra;

typedef struct {
    int ID;
    char titulo[MAXLINEA];
    float relevancia;
} LibroConPalabra;

//================================================
//Función de lectura de carácteres.
//================================================
void leer(char cadena[MAXLIBROS]){
  scanf("%100[^\n]s",cadena);
  int largo=strlen(cadena);
  if(largo>100){
    printf("No se ha podido registrar");
  }
  getchar();
}

//================================================
//Funciones para poder comparar según corresponda en cada función
//================================================
int compararPorTitulo(const void *libro1, const void *libro2) {
    const Libro *l1 = (const Libro *)libro1;
    const Libro *l2 = (const Libro *)libro2;
    return strcmp(l1->titulo, l2->titulo);
}

int compararPalabrasPorContador(const void *a, const void *b) {
    const Palabra *palabraA = (const Palabra *)a;
    const Palabra *palabraB = (const Palabra *)b;

    if (palabraA->contador < palabraB->contador) {
        return 1;  // Cambiar a -1 para orden ascendente
    } else if (palabraA->contador > palabraB->contador) {
        return -1;  // Cambiar a 1 para orden ascendente
    } else {
        return 0;
    }
}

int compararPalabrasPorTFIDF(const void *a, const void *b) {
  const PalabraTFIDF *palabraA = (const PalabraTFIDF *)a;
  const PalabraTFIDF *palabraB = (const PalabraTFIDF *)b;

  if (palabraA->tfidf < palabraB->tfidf) {
      return 1;  // Cambiar a -1 para orden descendente
  } else if (palabraA->tfidf > palabraB->tfidf) {
      return -1;  // Cambiar a 1 para orden ascendente
  } else {
      return 0;
  }
}

int compararLibrosPorRelevancia(const void *libro1, const void *libro2) {
    const LibroConPalabra *l1 = (const LibroConPalabra *)libro1;
    const LibroConPalabra *l2 = (const LibroConPalabra *)libro2;
    if (l1->relevancia < l2->relevancia) {
        return 1;
    } else if (l1->relevancia > l2->relevancia) {
        return -1;
    } else {
        return 0;
    }
}

//================================================
//Función para cargar documentos desde una carpeta
//================================================
int cargarDocumentos(const char *carpeta,Libro **libros, int *numLibros){
    DIR *dir;
    struct dirent *entry;
    dir = opendir(carpeta);
    if (dir == NULL) {
        printf("No se pudo abrir la carpeta.\n");
        return 0;
    }

    int capacidad = 10;
    *libros = (Libro *)malloc(capacidad * sizeof(Libro));
    *numLibros = 0;
    while ((entry = readdir(dir)) != NULL) {
      if (entry->d_type == DT_REG && strstr(entry->d_name, ".txt") != NULL) 
      {
        if (*numLibros >= capacidad) 
        {
          capacidad *= 2;
          *libros = (Libro *)realloc(*libros, capacidad * sizeof(Libro));
        }

        char nombreArchivo[MAXLINEA];
        sprintf(nombreArchivo, "%s/%s", carpeta, entry->d_name);

        FILE *archivo = fopen(nombreArchivo, "r");

        if (archivo == NULL) 
        {
          printf("No se pudo abrir el archivo %s\n", entry->d_name);
          continue;
        }

        int id = atoi(entry->d_name);
        (*libros)[*numLibros].ID = id;
        (*libros)[*numLibros].numPalabras = 0;
        (*libros)[*numLibros].numCaracteres = 0;

        //Buscar el título en la línea 10 u 11
        char linea[MAXLINEA];
        char titulo[MAXLINEA] = "Título desconocido";

        while (fgets(linea, sizeof(linea), archivo) != NULL) {
          char *tituloInicio = strstr(linea, "Title: ");
          if (tituloInicio != NULL) {
            tituloInicio += strlen("Title: ");
            char *finTitulo = strchr(tituloInicio, '\n');
            if (finTitulo != NULL) {
                *finTitulo = '\0';  // Elimina el salto de línea
            }
            strncpy(titulo, tituloInicio, sizeof(titulo));
            break;  // Sal del bucle cuando se encuentra el título
          }
        }

        strcpy((*libros)[*numLibros].titulo, titulo);

        int i = 0;
        while (i < MAXCONTENIDO && fscanf(archivo, "%c", &(*libros)[*numLibros].contenido[i]) != EOF) 
        {
          (*libros)[*numLibros].numCaracteres++;
          if (isalnum((*libros)[*numLibros].contenido[i]) && (i == 0 || !isalnum((*libros)[*numLibros].contenido[i - 1]))) 
          {
            (*libros)[*numLibros].numPalabras++;
          }
          i++;
        }
        (*libros)[*numLibros].contenido[i] = '\0';

        fclose(archivo);
        (*numLibros)++;
      }
  }

  closedir(dir);
  printf("Se han cargado los documentos!!\n");
  return 1;
}

//================================================
//Función para mostrar los libros ordenados por título
//================================================
void mostrarLibros(Libro *libros, int numLibros) {
  qsort(libros,numLibros,sizeof(Libro),compararPorTitulo);
  printf("Documentos cargados:\n\n");
  for (int i = 0; i < numLibros; i++) 
  {
    printf("========================================\n");
    printf("ID: %d\n", libros[i].ID);
    printf("Título: %s\n", libros[i].titulo);
    printf("Número de palabras: %d\n", libros[i].numPalabras);
    printf("Número de caracteres: %d\n", libros[i].numCaracteres);
  }
}

//================================================
//Función para buscar un libro según su título
//================================================
void buscarLibrosPorTitulo(const char *tituloBusqueda,Libro *libros, int numLibros) {
    int encontrados = 0;
    for (int i = 0; i < numLibros; i++) {
        if (strstr(libros[i].titulo, tituloBusqueda) != NULL) {
          printf("========================================\n");
          printf("Se encontró el libro!!\n\n");
          printf("ID: %d\n", libros[i].ID);
          printf("Título: %s\n", libros[i].titulo);
          printf("Palabras: %d\n", libros[i].numPalabras);
          printf("Caracteres: %d\n", libros[i].numCaracteres);
          encontrados = 1;
        }
    }
    if (!encontrados) {
        printf("No se encontraron libros con el título que contiene '%s'\n", tituloBusqueda);
    }
}

//================================================
//Función para mostrar las palabras con mayor frecuencia según el ID de un libro
//================================================
void mostrarPalabrasMayorFrecuencia(Libro *libros, int numLibros, int libroID) {
    // Encuentra el libro por su ID
    int libroEncontrado = -1;
    for (int i = 0; i < numLibros; i++) {
        if (libros[i].ID == libroID) {
            libroEncontrado = i;
            break;
        }
    }

    if (libroEncontrado == -1) {
        printf("No se encontró un libro con el ID especificado.\n");
        return;
    }

    Libro *libro = &libros[libroEncontrado];
    
    Palabra palabras[MAXPALABRA];
    int numPalabras = 0;

    // Tokeniza el contenido en palabras y calcula las frecuencias
    char contenidoCopia[MAXCONTENIDO];
    strcpy(contenidoCopia, libro->contenido);
    char *token = strtok(contenidoCopia, " ");
    while (token != NULL) {
        // Limpia la palabra eliminando signos de puntuación y conviértela a minúsculas
        char palabraLimpia[MAXPALABRA];
        int j = 0;
        for (int i = 0; i < strlen(token); i++) {
            if (isalnum(token[i])) {
                palabraLimpia[j] = tolower(token[i]);
                j++;
            }
        }
        palabraLimpia[j] = '\0';

        // Busca la palabra en la estructura Palabra
        int encontrado = -1;
        for (int i = 0; i < numPalabras; i++) {
            if (strcmp(palabraLimpia, palabras[i].palabra) == 0) {
                encontrado = i;
                break;
            }
        }

        // Si la palabra no se encontró, se agrega a la estructura Palabra
        if (encontrado == -1) {
            strcpy(palabras[numPalabras].palabra, palabraLimpia);
            palabras[numPalabras].contador = 1;
            numPalabras++;
        } else {
            palabras[encontrado].contador++;
        }

        token = strtok(NULL, " ");
    }

    // Ordenar las palabras por contador (en orden descendente)
    qsort(palabras, numPalabras, sizeof(Palabra), compararPalabrasPorContador);

    // Muestra las palabras con mayor frecuencia
    int maxPalabrasAMostrar = numPalabras < 10 ? numPalabras : 10;
    printf("Las 10 palabras con mayor frecuencia en el libro con ID %d son:\n", libro->ID);
    for (int i = 0; i < maxPalabrasAMostrar; i++) {
        printf("%s: %d veces\n", palabras[i].palabra, palabras[i].contador);
    }
}

//================================================
//Función ocupada para calcular palabras más relevantes
//================================================
float calcularTFIDF(const char *palabra, Libro *libro, Libro *libros, int numLibros) {
    // Calcular el TF de la palabra en el libro actual
    int contador = 0;
    char *texto = libro->contenido;
    while ((texto = strstr(texto, palabra)) != NULL) {
        contador++;
        texto += strlen(palabra);
    }

    float tf = (float)contador / libro->numPalabras;

    // Calcular el IDF de la palabra en todos los libros
    int documentosQueContienenPalabra = 0;
    for (int i = 0; i < numLibros; i++) {
        char *contenido = libros[i].contenido;
        char *found = strstr(contenido, palabra);
        if (found != NULL) {
            documentosQueContienenPalabra++;
        }
    }
    float idf = log((float)numLibros / documentosQueContienenPalabra);

    // Calcular el TF-IDF
    float tfidf = tf * idf;

    return tfidf;
}

//================================================
//Función para mostrar las palabras más relevantes
//================================================
void mostrarPalabrasRelevantes(Libro *libros, int numLibros, char tituloBuscado[MAXLINEA]) {
    // Buscar el libro por título
    int libroIndex = -1;
    for (int i = 0; i < numLibros; i++) {
        if (strcmp(libros[i].titulo, tituloBuscado) == 0) {
            libroIndex = i;
            break;
        }
    }

    if (libroIndex == -1) {
        printf("No se encontró un libro con el título especificado: %s\n", tituloBuscado);
        return;
    }

    Libro *libro = &libros[libroIndex];

    // Crear una estructura para almacenar palabras y sus valores TF-IDF
    

    PalabraTFIDF palabrasRelevantes[MAXPALABRA];
    int numPalabrasRelevantes = 0;

    // Tokeniza el contenido en palabras y calcula el TF-IDF para cada palabra
    char *token = strtok(libro->contenido, " ");
    while (token != NULL) {
        // Limpia la palabra eliminando signos de puntuación y conviértela a minúsculas
        char palabraLimpia[MAXPALABRA];
        int j = 0;
        for (int i = 0; i < strlen(token); i++) {
            if (isalnum(token[i])) {
                palabraLimpia[j] = tolower(token[i]);
                j++;
            }
        }
        palabraLimpia[j] = '\0';

        // Calcular el TF-IDF utilizando la función calcularTFIDF
        float tfidf = calcularTFIDF(palabraLimpia, libro, libros, numLibros);

        // Agregar la palabra y su TF-IDF a la lista
        strcpy(palabrasRelevantes[numPalabrasRelevantes].palabra, palabraLimpia);
        palabrasRelevantes[numPalabrasRelevantes].tfidf = tfidf;
        numPalabrasRelevantes++;

        token = strtok(NULL, " ");
    }
  
    qsort(palabrasRelevantes, numPalabrasRelevantes, sizeof(PalabraTFIDF), compararPalabrasPorTFIDF);

    // Muestra las 10 palabras con mayor relevancia
    int maxPalabrasAMostrar = numPalabrasRelevantes < 10 ? numPalabrasRelevantes : 10;
    printf("Las 10 palabras más relevantes en el libro con título '%s' son:\n", libro->titulo);
    for (int i = 0; i < maxPalabrasAMostrar; i++) {
        printf("%s: TF-IDF = %.4f\n", palabrasRelevantes[i].palabra, palabrasRelevantes[i].tfidf);
    }
}

//================================================
//Función para buscar palabra en específico 
//================================================
void buscarLibrosPorPalabra(Libro *libros, int numLibros, const char *palabraBuscada) {
    LibroConPalabra librosConPalabra[MAXLIBROS];
    int numLibrosConPalabra = 0;

    // Recorre todos los libros para encontrar los que contienen la palabra
    for (int i = 0; i < numLibros; i++) {
        char *contenido = libros[i].contenido;
        char *found = strstr(contenido, palabraBuscada);
        if (found != NULL) {
            // Utiliza calcularTFIDF para obtener el valor de TF-IDF
            float tfidf = calcularTFIDF(palabraBuscada, &libros[i], libros, numLibros);
          
            // Agregar información del libro a la lista
            librosConPalabra[numLibrosConPalabra].ID = libros[i].ID;
            strcpy(librosConPalabra[numLibrosConPalabra].titulo, libros[i].titulo);
            librosConPalabra[numLibrosConPalabra].relevancia = tfidf;
            numLibrosConPalabra++;
        }
    }

    // Ordenar los libros por relevancia de la palabra buscada de mayor a menor
    qsort(librosConPalabra, numLibrosConPalabra, sizeof( LibroConPalabra), compararLibrosPorRelevancia);

    // Mostrar los libros ordenados
    printf("Libros con la palabra '%s', ordenados por relevancia:\n", palabraBuscada);
    for (int i = 0; i < numLibrosConPalabra; i++) {
      printf("========================================\n");
      printf("ID: %d\n", librosConPalabra[i].ID);
      printf("Título: %s\n", librosConPalabra[i].titulo);
      printf("Relevancia: %4f\n", librosConPalabra[i].relevancia);
    }
  printf("\n");
}

void mostrarContextoPalabra(Libro *libros, int numLibros, int libroID, const char *palabraBuscada, int palabrasAntes, int palabrasDespues) {
    int libroEncontrado = -1;
  
    // Encuentra el libro por su ID
    for (int i = 0; i < numLibros; i++) {
        if (libros[i].ID == libroID) {
            libroEncontrado = i;
            break;
        }
    }

    if (libroEncontrado == -1) {
        printf("No se encontró un libro con el ID especificado.\n");
        return;
    }

    Libro *libro = &libros[libroEncontrado];

    char *posicion = libro->contenido;
    printf("Contexto de '%s' en el libro '%s':\n", palabraBuscada, libro->titulo);

    while ((posicion = strstr(posicion, palabraBuscada)) != NULL) {
        // Capturar el contexto de la palabra
        int inicio = (posicion - libro->contenido) - palabrasAntes;
        int fin = (posicion - libro->contenido) + strlen(palabraBuscada) + palabrasDespues;
        if (inicio < 0) {
            inicio = 0;
        }
        if (fin > libro->numCaracteres) {
            fin = libro->numCaracteres;
        }

        // Mostrar el contexto
        for (int i = inicio; i < fin; i++) {
            printf("%c", libro->contenido[i]);
        }
        printf("\n");

        // Avanzar para buscar la próxima aparición de la palabra
        posicion += strlen(palabraBuscada);
    }
}

//================================================
//Función para el interfaz del menú del programa
//================================================
void imprimirMenu(){
  printf("==================================================\n");
  printf("        ¡Bienvenido al buscador de documentos!     \n");
  printf("==================================================\n\n");
  printf("1. Cargar documentos\n");
  printf("2. Mostrar documentos ordenados\n");
  printf("3. Buscar un libro por su título\n");
  printf("4. Mostrar las palabras con mayor frecuencia\n");
  printf("5. Mostrar las palabras más relevantes\n");
  printf("6. Buscar por palabra en específico\n");
  printf("7. Mostrar palabra en su contexto dentro del libro\n");
  printf("0. Exit\n\n");
  printf("Ingrese un dígito: ");
}


//================================================
//Función principal que llama a las demás funciones según nuestro menú
//================================================
int main(){
  int digito;
  Libro *libros=NULL;
  int numLibros=0;
  while(true){
    imprimirMenu();
    scanf("%d", &digito);
    if (digito == 0){
      printf("\n¡Has finalizado tu sesión!\n");
      return 0;
    }else if(digito == 1){
      char nombreCarpeta[MAXLIBROS];
      getchar();
      printf("Ingrese nombre de la carpeta: ");
      leer(nombreCarpeta);
      cargarDocumentos(nombreCarpeta,&libros,&numLibros);
    }else if(digito == 2){
      if(numLibros > 0){
        mostrarLibros(libros,numLibros);        
      }else{
        printf("Por favor, cargue los archivos para continuar.\n");
      }
    }else if(digito == 3){      
      char tituloBuscado[MAXLINEA];
      if(numLibros > 0){
        getchar();
        printf("Ingrese del nombre del libro a buscar: ");
        leer(tituloBuscado);
        buscarLibrosPorTitulo(tituloBuscado,libros,numLibros);
      }else{
        printf("Por favor, cargue los archivos para continuar.\n");
      }
    }else if(digito == 4){
      if (numLibros > 0) {
          int libroID;
          printf("Ingrese ID del libro: ");
          scanf("%d", &libroID);
          printf("Buscando ID...\n");
          int libroEncontrado = -1;
          for (int i = 0; i < numLibros; i++) {
              if (libros[i].ID == libroID) {
                  libroEncontrado = i;
                  break;
              }
          }
          if (libroEncontrado != -1){
              mostrarPalabrasMayorFrecuencia(libros, numLibros, libroID);
          } else {
              printf("No se encontró un libro con el ID especificado.\n");
          }
      } else {
          printf("Por favor, cargue los archivos para continuar.\n");
      }
      }else if (digito == 5){
          if (numLibros > 0){
              char tituloBuscado[MAXLINEA];
              getchar();
              printf("Ingrese el título de un libro a buscar: ");
              leer(tituloBuscado);
              mostrarPalabrasRelevantes(libros, numLibros, tituloBuscado);
          }else{
              printf("Por favor, cargue los archivos para continuar.\n");
          }
      }else if (digito == 6){
          if (numLibros > 0){
              char palabraBuscada[MAXLINEA];
              getchar();
              printf("Ingrese la palabra a buscar: ");
              leer(palabraBuscada);
              buscarLibrosPorPalabra(libros, numLibros, palabraBuscada);
          } else {
              printf("Por favor, cargue los archivos para continuar.\n");
          }
      }else if(digito == 7){
      if (numLibros > 0){
        int libroID;
        printf("Ingrese el ID del libro: ");
        scanf("%d",&libroID);
        getchar();
        char palabraABuscar[MAXPALABRA];
        printf("Ingrese la palabra a buscar: ");
        leer(palabraABuscar);
        int palabrasAntes=5;
        int palabrasDespues=5;
          mostrarContextoPalabra(libros,numLibros,libroID,palabraABuscar,palabrasAntes,palabrasDespues);
      }else{
        printf("Por favor, cargue los archivos para continuar.\n");
      }
    }else{
      printf("Opción inválida.\n");
    } 
  }
}
