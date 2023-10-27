**Descripción:**
-Programa de gestión de menú y análisis de una colección de libros. Permite cargar libros desde una carpeta, recopilando información como título, ID, número de palabras y contenido. Luego, ofrece diversas funcionalidades, incluyendo la visualización de los libros ordenados por título, la búsqueda de libros por título, la identificación de las 10 palabras más frecuentes en un libro, la determinación de las 10 palabras más relevantes en función de TF-IDF para un libro dado, la búsqueda de libros que contengan una palabra específica y la visualización del contexto de una palabra dentro de un libro.
-------------------------------------------------------------  
Funcionando correctamente:
- El programa carga los documentos y lo almacena en nuestra estructura.
- El programa ordena y muestra los documentos ordenados por título.
- El programa cumple la función para buscar por título
- El programa puede buscar por una palabra en específico.
- El programa muestra las palabras en su contexto dentro de un libro y muestra sus apariciones.
-------------------------------------------------------------
Problemas conocidos: El programa lo establecimos al usar nuestras propias estructuras, ya que al cargar desde una carpeta y ingresarlo a un mapa presentaba errores de compilación. Además, nuestro programa presenta errores en las funciones 4 y 5 donde el programa se cierra sin completar lo programado. Errores de bucle al recorrer los libros en nuestra estructura (exit status -1). Nuestros errores se deben principalmente al manejo de memoria. Además, existe un desbordamiento de memoria en función 4 cuando se define el tamaño de la cantidad de palabras en un libro y la cantidad de carácteres porque es una cantidad bastante grande y termina con problemas en su ejecución.
-------------------------------------------------------------
A mejorar: Manejar mejor el uso de la memoria. Mejorar el uso de TDAs para un programa más eficiente. 
-------------------------------------------------------------
Contribuciones:
- Álvaro Del Pino: Creación de estructura para el programa. Implementación de funciones (5,6,7). Diseño del programa. Auto-evaluación: 3 (Aporte Excelente).

- Pablo Daza: Documentación y comentarios en el código. Creación de función 4. Diseño de la interfaz del programa. Auto-evaluación: 3 (Aporte Excelente).

- Sebastián Rojas: Creación de la interfaz del programa. Implementación de las funciones (1,2,3). Diseño y reacción del readme. Creación de comentarios en el código. Auto-evaluación: 3 (Aporte Excelente).
-------------------------------------------------------------
  