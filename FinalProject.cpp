#include<iostream>
#include<map>
#include<string>
#include<algorithm>
#include<vector>
#include<string>
#include <unordered_map>
#include <chrono>
#include <fstream>

using namespace std;
class suffixTree
{
public:

    suffixTree* children[26];
    //CONTENEDOR QUE CONTIENE LISTAS DE OCURRENCIAS 
    //(ID DOCUMENTO, # OCURRENCIAS)
    //CUANDO INSERTO LAS PALABRAS A MI ÁRBOL VOY HACIENDO DEL NÚMERO DE OCURRENCIAS DE PALABRAS QUE HAY EN EL DOCUMENTO Y LAS GUARDO SEGÚN SU ID.
    //USO UN MAP PORQUE NO SÉ LA CANTIDAD DE NÚMEROS QUE VOY A INGRESAR (MAP YA SABE CÓMO MANEJAR ESTA CONDICIÓN)
    map<int, int>ocurren;

    //CONSTRUCTOR DE SUFFIX TREE
    suffixTree() {
        //INICIALIZO LOS HIJOS EN NULL
        for (int i = 0; i < 26; ++i)
        {
            children[i] = nullptr;
        }
    };
    //DESTRUCTOR
    //IMPLEMENTADO PORQUE A VECES DABA ERROR 
    ~suffixTree() {
        for (int i = 0; i < 26; ++i)
        {
            delete children[i];
        }
    }

    //FUNCIÓN QUE INSERTA LA PALABRA AL ÁRBOL, RECIBE DOS ENTRADAS: LA PALABRA QUE QUIERO INSERTAR Y SU ID
    void insert(const string& word_to_be_inserted, int id)
    {
        //PUNTERO AL NODO EN EL CUAL VA A SER INSERTADO
        suffixTree* p = this;
        for (char letter : word_to_be_inserted)
        {

            //INSERTO SI ES DIFERENTE DE _ 
            //ESTO DEBIDO A QUE EN LA ETAPA DE PROCESAMIENTO SE ELIMINAN LOS SIGNOS DE PUNTUACIÓN, Y POR ALGUNA RAZÓN, 
            //NO SE ELIMINA EL "_", ENTONCES AQUÍ TENGO CONTROL SOBRE ESA CONDICIÓN.
            //SI NO TUVIERA ESTO DARÍA ERROR YA QUE NO LO CONSIDERARÍA EN LA ETAPA DE NORMALIZACIÓN.
            
            if (letter != '_')
            {
                //USANDO TABLA ASCII, SI RESTO "a" SE NORMALIZA, REDUCE LOS VALORES DE 0 a 26, HAY QUE TOMAR EN CUENTA QUE ESTAMOS TRABAJANDO CON LETRAS MINÚSCULAS
                //a VENDRÍA A SER COMO UN MÍNIMO
                //ENTONCES, SI MI PUNTERO APUNTA A NULL (O SEA SI ESTÁ INICIALIZADO, ES NUEVO), CREO UN NUEVO HIJO
                //SI NO LE PONGO LA RESTA, PODEMOS SALIRNOS DEL RANGO DEL ARRAY
                if ((p->children[letter - 'a']) == nullptr)
                {
                    (p->children[letter - 'a']) = new suffixTree();
                }
                //AHORA MI PUNTERO P APUNTARÁ A ESE HIJO
                p = (p->children[letter - 'a']);
            }
        }
        // UNA VEZ QUE TERMINE ESTO, HAGO QUE LAS OCURRENCIAS DE ESE PUNTERO P AUMENTEN EN 1
        //SI LA PALABRA EXISTE ENTONCES OCURRENCIAS++
        (p->ocurren[id])++;
    }

    //FUNCIÓN PARA BUSCAR UNA PALABRA 
    int _find(const string& word_I_wanna_find, int N) {
        suffixTree* p = this;
        //EMPIEZO MI BÚSQUEDA DESDE EL PRIMER CARACTER HASTA EL ÚLTIMO DE LA PALABRA A BUSCAR
        for (char letter : word_I_wanna_find)
        {
            //P SE QUEDA EN LA POSICIÓN DONDE FUE ENCONTRADA 
            //SI EN CASO NO LO ENCUENTRA (SI NO EXISTE UNA FORMA DE LLEGAR AL NODO), RETORNA FALSE
            //SINO SIMPLEMENTE SE QUEDA EN LA POSICIÓN DEL NODO DE LA PALABRA 
            if (p->children[letter - 'a'] == nullptr)
            {
                cout << "No existe esta palabra" << endl;
                return -1;
            }
            //GUARDO A P
            p = p->children[letter - 'a'];
        }


        //PUEDE QUE P ESTÉ APUNTANDO A UNA PALABRA Y QUE ESTA SEA EL PREFIJO DE OTRA, PERO NO ES LA PALABRA QUE ESTAMOS BUSCANDO.
        //ESTO LO VERIFICAMOS CON NUESTRO MAP
        //LA PALABRA ES PREFIJO DE OTRA, PERO NO LA PALABRA EN SÍ
        //EJM: SUP. QUE INSERTAMOS LA PALABRA "YAHAIRA", Y QUEREMOS BUSCAR LA PALABRA "YA", NUESTRO PUNTERO P VA A RECORRER EL ÁRBOL Y SÍ VA A ENCONTRAR EL 
        //CAMINO QUE ME LLEVE DESDE LA RAIZ HASTA LA PALABRA "YA". SIN EMBARGO, ESTA PALABRA NO EXISTE EN SÍ, ES SOLO UN PREFIJO DE OTRA, ENTONCES
        //ACÁ SE VERIFICA QUE: SI NUNCA SE HA AGREGADO UNA OCURRENCIA, SIGNIFICA QUE NUNCA EXISTIÓ ESA PALABRA.

        if (p->ocurren.empty()) {
            cout << "No existe esta palabra" << endl;
            return -1;
        }

        //CANTIDAD DE DOCUMENTOS DONDE APARECE LA PALABRA 
        int df = ocurren.size();

        //POR ÚLTIMO RECORRO MI MAP 
        for (auto i : (p->ocurren))
        {

            //i.second ES DONDE SE GUARDA LA CANTIDAD DE OCURRENCIAS EN UN DOCUMENTO
            int TF = i.second;

            //FÓRMULA PARA CALCULAR EL SCORE DE OCURRENCIAS DEL DOCUMENTO
            float f = 1.0f * TF * log10(N / df);
            //i.first ES EL ID DEL DOCUMENTO, CANTIDAD DE OCURRENCIAS EN UN DOCUMENTO, SCORE CALCULADO
            cout << "Documento: " << i.first << " ,  ocurrencias: " << i.second << ",  f: " << f << endl;
            
            
        }
        return 1;
    }
};
int main() {

    int id_file = 1;
    fstream file;
    string word, t, q, filename;

    // filename of the file
    filename = "allabstracts500000.txt";

    // opening file
    file.open(filename.c_str());

    // extracting words from the file

    suffixTree tree;
    std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
    start = std::chrono::high_resolution_clock::now();
    while (file >> word)
    {
        //DETECTA EL SÍMBOLO $ Y SEPARA LOS ABSTRACTS
        if (word[0] == '$'){
            id_file++;
        }
        else if (word.size() > 1)
        {
            // cout<<"word: "<<word<<endl;
            
            tree.insert(word, id_file);
            
        }
    }
    end = std::chrono::high_resolution_clock::now(); int64_t duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    cout << "Time inserting " + std::to_string(duration) + "ns\n";

    cout << "Ingresa la palabra a buscar " << endl;
    string pattern; cin >> pattern;

    std::chrono::time_point<std::chrono::high_resolution_clock> start2, end2;
    start2 = std::chrono::high_resolution_clock::now();
    cout << tree._find(pattern, id_file - 1) << endl;
    end2 = std::chrono::high_resolution_clock::now(); int64_t duration2 = std::chrono::duration_cast<std::chrono::nanoseconds>(end2 - start2).count();
    cout << "Time searching " + std::to_string(duration2) + "ns\n";



   
}