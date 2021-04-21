#include <iostream>
#include <cmath>
#include <random>
using namespace std;
int randomNumbers(int range) {
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<> dis(1, range);
    int num = dis(gen);
    return num;
}

// Clase punto
class Punto
{
public:
    int x, y;
    Punto() {
        this->x = 0;
        this->y = 0;
    }
    Punto(int x, int y) {
        this->x = x;
        this->y = y;
    }

};

class Nodo
{
public:
    Punto pos;
    int data;
    Nodo(Punto pos, int data) {
        this->pos = pos;
        this->data = data;
    }
    Nodo() {
        pos = Punto(0, 0);
        data = 0;
    }
};

class Quad
{
public:
    Punto nw, se;
    int capacity = 1;
    Nodo* n = nullptr;
    Quad* northWest = nullptr, * northEast = nullptr, * SouthWest = nullptr, * SouthEast = nullptr;

    Quad()
    {
        nw = Punto(0, 0);
        se = Punto(0, 0);
    }
    Quad(Punto topL, Punto botR)
    {
        this->nw = topL;
        this->se = botR;
    }
    // Función que verifica que no pase del límte
    bool container(Punto p)
    {
        return (p.x >= nw.x && p.x <= se.x && p.y >= nw.y && p.y <= se.y);
    }

    void insert(Nodo* node)
    {
        if (node == nullptr)
            return;

        if (!container(node->pos))
            return;

        //Verifica que no pase de la capacidad
        if (abs(nw.x - se.x) <= capacity && abs(nw.y - se.y) <= capacity)
        {
            if (n == nullptr)
                n = node;
            return;
        }

        if ((nw.x + se.x) / 2 >= node->pos.x)
        {
            // NORTHWEST
            if ((nw.y + se.y) / 2 >= node->pos.y)
            {
                if (northWest == NULL)
                    northWest = new Quad(Punto(nw.x, nw.y), Punto((nw.x + se.x) / 2, (nw.y + se.y) / 2));
                northWest->insert(node);
            }

            // SOUTHWEST
            else
            {
                if (SouthWest == NULL)
                    SouthWest = new Quad(Punto(nw.x, (nw.y + se.y) / 2), Punto((nw.x + se.x) / 2, se.y));
                SouthWest->insert(node);
            }
        }
        else
        {
            // NORTHEAST
            if ((nw.y + se.y) / 2 >= node->pos.y)
            {
                if (northEast == NULL)
                    northEast = new Quad(Punto((nw.x + se.x) / 2, nw.y), Punto(se.x, (nw.y + se.y) / 2));
                northEast->insert(node);
            }

            //  SOUTHEAST
            else
            {
                if (SouthEast == NULL)
                    SouthEast = new Quad(Punto((nw.x + se.x) / 2, (nw.y + se.y) / 2), Punto(se.x, se.y));
                SouthEast->insert(node);
            }
        }
    }


    Nodo* search(Punto p)
    {

        if (!container(p) == 1) {
            return nullptr;
        }

        if (n != NULL) {
            return n;
        }


        if ((nw.x + se.x) / 2 >= p.x)
        {
            // NORTHWEST
            if ((nw.y + se.y) / 2 >= p.y)
            {
                if (northWest == NULL)
                    return NULL;
                return northWest->search(p);
            }

            // SOUTHWEST
            else
            {
                if (SouthWest == NULL)
                    return NULL;
                return SouthWest->search(p);
            }
        }
        else
        {
            // NORTHEAST
            if ((nw.y + se.y) / 2 >= p.y)
            {
                if (northEast == NULL)
                    return NULL;
                return northEast->search(p);
            }

            // SOUTHEAST
            else
            {
                if (SouthEast == NULL)
                    return NULL;
                return SouthEast->search(p);
            }
        }
    };



};

int main()
{
    int max = 20;
    Quad centro(Punto(0, 0), Punto(max, max));
    Nodo n1(Punto(1, 1), randomNumbers(max));
    Nodo n2(Punto(2, 5), randomNumbers(max));
    Nodo n3(Punto(7, 6), randomNumbers(max));
    centro.insert(&n1); centro.insert(&n2); centro.insert(&n3);
    cout << "Node a: " << centro.search(Punto(1, 1))->data << "\n";
    cout << "Node b: " << centro.search(Punto(2, 5))->data << "\n";
    cout << "Node c: " << centro.search(Punto(7, 6))->data << "\n";


    return 0;
}