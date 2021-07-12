# Estructura de datos de recuperación de documentos

La recuperación de datos es un conjunto de técnicas utilizadas para acceder y extraer la información almacenada en diferentes tipos de bases de datos o medios digitales.
El siguiente trabajo hace uso de la estructura Suffix tree para la recuperación de datos y calcula una puntuación de ocurrencias de una palabra en todos los documentos que la contengan.
Implementación en C ++ de un suffix tree.

## Pre-requisitos 📋

Descargue el conjunto de datos de la siguiente página https://drive.google.com/drive/folders/1jAmpwspXkdzLJHMEhM3JPVuHd7-JiwYL

## Comenzando ⚙️

La implementación está en el documento FinalProject.cpp.
Explicación del formato de entrada:

Actualmente el programa se encuentra indexado con 500 000 abstracts, así que puede que demore cierto tiempo indexar todas las palabras. 
1. Una vez culminado esto, aparecerá la frase "Ingresa la palabra a buscar"
2. Escriba la palabra que desee buscar.
3. El programa imprimirá todas las ocurrencias de la palabra en los abstracts que la contengan. En caso la palabra no esté en algún documento, imprimirá la frase "No existe esta palabra"

NOTA: Puede modificar la cantidad de abstracts reemplazando la línea 130 del programa por la cantidad que desee (ejemplo: filename = "allabstracts100.txt"), tomando el cuenta que dicho archivo se encuentre en el conjunto de datos brindado en la página de drive (https://drive.google.com/drive/folders/1jAmpwspXkdzLJHMEhM3JPVuHd7-JiwYL)

Ejemplo de conjunto de datos:
INPUT: describe
El algoritmo se detiene cuando ya se analizó todos los abstracts o si no hay alguna coincidencia con la palabra buscada.

### Instalación 🔧
Para instalar y correr el proyecto descargue la carpeta la carpeta completa y ejecute el programa el cualquier entorno de desarrollo o también puede compilarlo desde su consola con el siguiente comando:

g++ -std=c++0x -o FinalProject.exe FinalProject.cpp 

Luego ejecute el programa con:

./FinalProject.exe

## Autor ✒️

* **Yahaira Gomez Sucasaca** - *Documentación* - [YahairaGomez](https://github.com/YahairaGomez)


## Expresiones de Gratitud 🎁

* Agradecimiento especiales a YouTube y a la cafeína ☕
