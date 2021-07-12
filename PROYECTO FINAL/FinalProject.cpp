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
    //CUANDO INSERTO LAS PALABRAS A MI �RBOL VOY HACIENDO DEL N�MERO DE OCURRENCIAS DE PALABRAS QUE HAY EN EL DOCUMENTO Y LAS GUARDO SEG�N SU ID.
    //USO UN MAP PORQUE NO S� LA CANTIDAD DE N�MEROS QUE VOY A INGRESAR (MAP YA SABE C�MO MANEJAR ESTA CONDICI�N)
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

    //FUNCI�N QUE INSERTA LA PALABRA AL �RBOL, RECIBE DOS ENTRADAS: LA PALABRA QUE QUIERO INSERTAR Y SU ID
    void insert(const string& word_to_be_inserted, int id)
    {
        //PUNTERO AL NODO EN EL CUAL VA A SER INSERTADO
        suffixTree* p = this;
        for (char letter : word_to_be_inserted)
        {

            //INSERTO SI ES DIFERENTE DE _ 
            //ESTO DEBIDO A QUE EN LA ETAPA DE PROCESAMIENTO SE ELIMINAN LOS SIGNOS DE PUNTUACI�N, Y POR ALGUNA RAZ�N, 
            //NO SE ELIMINA EL "_", ENTONCES AQU� TENGO CONTROL SOBRE ESA CONDICI�N.
            //SI NO TUVIERA ESTO DAR�A ERROR YA QUE NO LO CONSIDERAR�A EN LA ETAPA DE NORMALIZACI�N.
            
            if (letter != '_')
            {
                //USANDO TABLA ASCII, SI RESTO "a" SE NORMALIZA, REDUCE LOS VALORES DE 0 a 26, HAY QUE TOMAR EN CUENTA QUE ESTAMOS TRABAJANDO CON LETRAS MIN�SCULAS
                //a VENDR�A A SER COMO UN M�NIMO
                //ENTONCES, SI MI PUNTERO APUNTA A NULL (O SEA SI EST� INICIALIZADO, ES NUEVO), CREO UN NUEVO HIJO
                //SI NO LE PONGO LA RESTA, PODEMOS SALIRNOS DEL RANGO DEL ARRAY
                if ((p->children[letter - 'a']) == nullptr)
                {
                    (p->children[letter - 'a']) = new suffixTree();
                }
                //AHORA MI PUNTERO P APUNTAR� A ESE HIJO
                p = (p->children[letter - 'a']);
            }
        }
        // UNA VEZ QUE TERMINE ESTO, HAGO QUE LAS OCURRENCIAS DE ESE PUNTERO P AUMENTEN EN 1
        //SI LA PALABRA EXISTE ENTONCES OCURRENCIAS++
        (p->ocurren[id])++;
    }

    //FUNCI�N PARA BUSCAR UNA PALABRA 
    int _find(const string& word_I_wanna_find, int N) {
        suffixTree* p = this;
        //EMPIEZO MI B�SQUEDA DESDE EL PRIMER CARACTER HASTA EL �LTIMO DE LA PALABRA A BUSCAR
        for (char letter : word_I_wanna_find)
        {
            //P SE QUEDA EN LA POSICI�N DONDE FUE ENCONTRADA 
            //SI EN CASO NO LO ENCUENTRA (SI NO EXISTE UNA FORMA DE LLEGAR AL NODO), RETORNA FALSE
            //SINO SIMPLEMENTE SE QUEDA EN LA POSICI�N DEL NODO DE LA PALABRA 
            if (p->children[letter - 'a'] == nullptr)
            {
                cout << "No existe esta palabra" << endl;
                return -1;
            }
            //GUARDO A P
            p = p->children[letter - 'a'];
        }


        //PUEDE QUE P EST� APUNTANDO A UNA PALABRA Y QUE ESTA SEA EL PREFIJO DE OTRA, PERO NO ES LA PALABRA QUE ESTAMOS BUSCANDO.
        //ESTO LO VERIFICAMOS CON NUESTRO MAP
        //LA PALABRA ES PREFIJO DE OTRA, PERO NO LA PALABRA EN S�
        //EJM: SUP. QUE INSERTAMOS LA PALABRA "YAHAIRA", Y QUEREMOS BUSCAR LA PALABRA "YA", NUESTRO PUNTERO P VA A RECORRER EL �RBOL Y S� VA A ENCONTRAR EL 
        //CAMINO QUE ME LLEVE DESDE LA RAIZ HASTA LA PALABRA "YA". SIN EMBARGO, ESTA PALABRA NO EXISTE EN S�, ES SOLO UN PREFIJO DE OTRA, ENTONCES
        //AC� SE VERIFICA QUE: SI NUNCA SE HA AGREGADO UNA OCURRENCIA, SIGNIFICA QUE NUNCA EXISTI� ESA PALABRA.

        if (p->ocurren.empty()) {
            cout << "No existe esta palabra" << endl;
            return -1;
        }

        //CANTIDAD DE DOCUMENTOS DONDE APARECE LA PALABRA 
        int df = ocurren.size();

        //POR �LTIMO RECORRO MI MAP 
        for (auto i : (p->ocurren))
        {

            //i.second ES DONDE SE GUARDA LA CANTIDAD DE OCURRENCIAS EN UN DOCUMENTO
            int TF = i.second;

            //F�RMULA PARA CALCULAR EL SCORE DE OCURRENCIAS DEL DOCUMENTO
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
        //DETECTA EL S�MBOLO $ Y SEPARA LOS ABSTRACTS
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