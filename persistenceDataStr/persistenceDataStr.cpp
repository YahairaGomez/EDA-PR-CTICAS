#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <utility>
#include <iostream>
#include <vector>
using namespace std;

template <typename Type>
class nodo {
public:
	int* valor;
	size_t ptr_size;
	nodo** next;

	nodo(int data, std::size_t const& ptr_size) {
		this->valor = new int(data);
		this->ptr_size = ptr_size;
		next= new nodo<int>*[ptr_size]();
	}
	virtual ~nodo() {}
	nodo& operator[](std::size_t const& id) const {
		return *next[id];
	}
};

template <typename Type>
class PartialNode : public nodo<int> {
public:
	int modificaciones;
	vector<nodo<int>> Table;
	vector<int> version;
	PartialNode** prev;
	PartialNode(int data, size_t const& size_ptr, size_t const& size_val)
		: nodo<int>(data, size_ptr), modificaciones(0), prev(new PartialNode* [size_val]())
	{
		version = std::vector<int>(size_val * 2, 0);
		for (int i = 0; i < size_val * 2; ++i) {
			Table.emplace_back(data, size_ptr);
		}
	}
	
	PartialNode& operator[](
		std::pair<std::size_t, unsigned int> id_version) const {
		int i = 0;
		while (i < modificaciones && version[i] <= id_version.second)i++;
		i--;
		return *(PartialNode*)(Table[i].next[id_version.first]);
	}
};

template <typename Type, typename Node>
class Lista {
public:
	Node* head;
	size_t ptrs_size;

	Lista(const int data, size_t const& valores, size_t const& ptrs) {
		head = new Node(data, valores, ptrs);
		ptrs_size = valores;
	}
};
	
template <typename Type, typename Node = PartialNode<int>>
class Partial_list : public Lista<int, Node> {
public:
	unsigned int version_actual;
	std::size_t ptr_size;
	std::size_t ptrSize;
		typedef Type data_type;

			Partial_list(int const data, std::size_t const& ptrSize, size_t const& in_ptrs_size) : Lista<int, Node>(data, ptrSize, in_ptrs_size),
				ptr_size(in_ptrs_size),
				version_actual(0) {}

			Node* insert_node(int const data, Node* nodito, std::size_t position) {
				
				version_actual++;
				if (nodito->modificaciones < ptr_size * 2)
				{

					Node* new_node = new Node(data, Lista<int, Node>::ptrs_size, ptr_size);
					nodito->version[nodito->modificaciones] = version_actual;
					if (nodito->modificaciones > 0)
						nodito->Table[(nodito->modificaciones)] = nodito->Table[(nodito->modificaciones) - 1];
					new_node->prev[position] = nodito;
					nodito->Table[(nodito->modificaciones)].next[position] = new_node;
					(nodito->modificaciones)++;
					return (Node*)(nodito->Table[nodito->modificaciones - 1].next[position]);
				}
				else
				{
					Node* new_u = new Node(*(nodito->valor), Lista<int, Node>::ptrs_size, ptr_size);
					for (int i = 0; i<int(ptr_size); ++i)
					{
						if (nodito->prev[i] != nullptr)
						{
							++version_actual;
							con_node(nodito->prev[i], new_u, position, version_actual);
						}
					}
					Node* new_node = new Node(data, Lista<int, Node>::ptrs_size, ptr_size);
					nodito->version[nodito->modificaciones] = version_actual;
					if (nodito->modificaciones > 0)
						nodito->Table[(nodito->modificaciones)] = nodito->Table[(nodito->modificaciones) - 1];
					new_node->prev[position] = nodito;
					nodito->Table[(nodito->modificaciones)].next[position] = new_node;
					(nodito->modificaciones)++;
					return (Node*)(nodito->Table[nodito->modificaciones - 1].next[position]);
				}
			}
			void con_node(Node* nodito, Node* v, std::size_t position, unsigned int vers) {
				if ((nodito->modificaciones) < ptr_size * 2)
				{
					nodito->version[nodito->modificaciones] = vers;
					nodito->Table[nodito->modificaciones].next[position] = v;
					v->prev[position] = nodito;
					(nodito->modificaciones)++;
				}
				else
				{
					Node* new_nod = new Node(*(nodito->valor), Lista<int, Node>::ptrs_size, ptr_size);
					for (int i = 0; i<int(ptr_size); ++i)
					{
						if (nodito->prev[i] != nullptr)
							con_node(nodito->prev[i], new_nod, i, version_actual);
						
					}
					new_nod->version[nodito->modificaciones] = vers;
					new_nod->Table[nodito->modificaciones].next[position] = v;
					v->prev[vers] = new_nod;
					(new_nod->modificaciones)++;
				}
			
			}

		};


int main() {
	Partial_list<int> nodito(38, 35, 7);
	std::cout << "Head: " << *nodito.head->valor << std::endl;

	//The new one
	PartialNode<int>* new_nodo = nodito.insert_node(2, nodito.head, 0);

	cout << "Head next: " << *new_nodo->valor << endl;
	PartialNode<int>* new_nodo2 = nodito.insert_node(6, nodito.head, 0);


	std::cout << "version 2 valor de valor " << *(*nodito.head)[{1, 1}].valor << std::endl;

	return 0;
}
