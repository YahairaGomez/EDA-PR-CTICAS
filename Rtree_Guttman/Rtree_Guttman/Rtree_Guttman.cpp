#include <iostream>
#include <utility>
#include <stdio.h>
using namespace std;
typedef int RTdimension;
typedef int dimIdx;
typedef int childIdx;

#define RTn 2     
#define RTPS 200 
#define m 2                        
#define M 4
#define IS_BRANCH(N) ((N)->Child && (N)->Child[0].Child && (N)->Child[0].Tuple == NULL)
#define IS_LEAF(N)   ((N)->Child && (N)->Child[0].Tuple && (N)->Child[0].Child == NULL)
#define IS_TUPLE(N)  ((N)->Tuple && (N)->Child == NULL)
#define IS_EMPTY(N)  ((N).Parent == NULL) 
#define LEVEL_TOP -1
#define LEVEL_LEAF 1
#define LEVEL_TUPLE 0

struct RTNodeList {
    struct RTNodeList* Next;
    void* Tuple;
    RTdimension I[RTn * 2];
};
struct Node;
typedef struct Node* RTreePtr;

static struct Node {

    struct Node* Parent;
    struct Node* Child;
    void* Tuple;
    RTdimension I[RTn * 2];
    Node empty() {
        Parent = Child = NULL,
            Tuple = NULL,
            I[4] = { 0 };
    }


    bool ConsRtree(struct Node* ptr, childIdx size) {
        childIdx i;

        for (i = 0; i < size; ++i) {
            ptr[i] = empty();
        }

        return true;
    }

    void* mem_alloc(size_t size) {
        void* mem = malloc(size);
        memset(mem, 0, size);
        return mem;
    }
    bool Overlap(RTdimension* S1, RTdimension* S2) {
        dimIdx j = 0, k = 0;
        for (j = 0, k = RTn; j < RTn; ++j, ++k)
            if (S1[j] > S2[k] || S2[j] > S1[k])
                return false;

        return true;
    };

    bool cover(RTdimension* S1, RTdimension* S2) {
        dimIdx j = 0, k = 0;

        for (j = 0, k = RTn; j < RTn; ++j, ++k)
            if (S1[j] < S2[j] || S2[k] < S1[k])
                return false;

        return true;
    };
    long double safe_multiply(long double left, long double right) {
        int sign = 1;
        if (left == 0 || right == 0) return 0;
        if (left < 0) { left = -left; sign = -sign; }
        if (right < 0) { right = -right; sign = -sign; }
        return sign * left * right;
    }

    long double areaRec(RTdimension* S) {
        dimIdx j = 0, k = 0;
        long double volume = 1;

        for (j = 0, k = RTn; j < RTn; ++j, ++k)

            volume = safe_multiply(volume, (long double)S[k] - S[j] + 1);

        return volume;
    }
    bool updateData(RTreePtr* T, RTdimension I[], void* Tuple, void* New) {
        struct Node* L = NULL;
        childIdx pos = 0;

        FindLeaf(*T, I, Tuple, &L, &pos);

        if (!L)
            return false;

        ((L)->Child + pos)->Tuple = New;
        return true;
    }
    bool updateDim(RTreePtr* T, RTdimension I[], void* Tuple, RTdimension New[]) {
        struct Node* L = NULL;
        childIdx pos = 0;

        FindLeaf(*T, I, Tuple, &L, &pos);

        if (!L)
            return false;

        if (cover((RTdimension*)New, (L)->I))
            memcpy(((L)->Child + pos)->I, New, sizeof(((L)->Child + pos)->I));
        else {
            Delete(T, I, Tuple);
            Insert(T, LEVEL_LEAF, New, Tuple, NULL);
        }

        return true;
    }

    bool createRtree(struct Node** T, struct RTNodeList* list) {
        childIdx h;
        size_t i, j, k;
        struct RTNodeList* nodelist;
        struct Node* node;

        struct Node* stack, * stack_next, * branch, * branch_next;

        stack = NULL;
        nodelist = list;
        while (nodelist != NULL) {
            node = (struct Node*)mem_alloc(sizeof(struct Node));
            ConsRtree(node, 1);
            node->Child = (struct Node*)mem_alloc(M * sizeof(node->Child[0]));
            ConsRtree(node->Child, M);

            memcpy(node->I, nodelist->I, sizeof(node->I));
            for (i = 0; nodelist != NULL && i < M; ++i) {
                memcpy(node->Child[i].I, nodelist->I, sizeof(node->Child[i].I));
                node->Child[i].Tuple = nodelist->Tuple;
                node->Child[i].Parent = node;

                for (j = 0, k = RTn; j < RTn; ++j, ++k) {
                    node->I[j] = node->I[j] < nodelist->I[j] ? node->I[j] : nodelist->I[j];
                    node->I[k] = node->I[k] > nodelist->I[k] ? node->I[k] : nodelist->I[k];
                }

                nodelist = nodelist->Next;
            }

            node->Parent = stack;
            stack = node;
        }

        while (stack && stack->Parent != NULL) {
            branch = stack;
            stack_next = NULL;
            while (branch != NULL) {
                node = (struct Node*)mem_alloc(sizeof(struct Node));
                ConsRtree(node, 1);
                node->Child = (struct Node*)mem_alloc(M * sizeof((*T)->Child[0]));
                ConsRtree(node->Child, M);

                memcpy(node->I, branch->I, sizeof(node->I));
                for (i = 0; branch != NULL && i < M; ++i) {
                    branch_next = branch->Parent;

                    memcpy(node->Child + i, branch, sizeof(node->Child[i]));
                    node->Child[i].Parent = node;

                    if (node->Child[i].Child != NULL) {
                        for (h = 0; h < M && !IS_EMPTY(node->Child[i].Child[h]); ++h)
                            node->Child[i].Child[h].Parent = node->Child + i;
                    }


                    for (j = 0, k = RTn; j < RTn; ++j, ++k) {
                        node->I[j] = node->I[j] < branch->I[j] ? node->I[j] : branch->I[j];
                        node->I[k] = node->I[k] > branch->I[k] ? node->I[k] : branch->I[k];
                    }

                    free(branch);
                    branch = branch_next;
                }
                node->Parent = stack_next;
                stack_next = node;
            }
            stack = stack_next;
        }

        (*T) = stack;

        if ((*T) == NULL) {
            (*T) = (struct Node*)mem_alloc(sizeof(struct Node));
            ConsRtree(*T, 1);
            (*T)->Child = (struct Node*)mem_alloc(M * sizeof((*T)->Child[0]));
            ConsRtree((*T)->Child, M);
        }

        return true;
    }


    bool Search(struct Node* T, RTdimension S[], struct RTNodeList** list, size_t* count) {
        struct RTNodeList* curr = NULL;
        size_t cnt = 0;
        childIdx i = 0;

        if (count) *count = 0;

        //Busca subarboles
        if (IS_BRANCH(T)) {
            for (i = 0; i < M && !IS_EMPTY(T->Child[i]); ++i) {
                if (Overlap(T->Child[i].I, S)) {
                    if (!Search(T->Child + i, S, list, &cnt)) {
                        if (list) *list = NULL;
                        if (count) *count = 0;
                        return false;
                    }
                    if (count) *count += cnt;
                }
            }


            return true;

            //Buscas a la hoja
        }
        else if (IS_LEAF(T) || ((T->Child) && IS_EMPTY(T->Child[0]))) {
            for (i = 0; i < M && !IS_EMPTY(T->Child[i]); ++i) {
                if (Overlap(T->Child[i].I, S)) {
                    if (list) {
                        curr = (struct RTNodeList*)mem_alloc(sizeof(struct RTNodeList));
                        memcpy(curr->I, T->Child[i].I, sizeof(curr->I));
                        curr->Tuple = (T->Child + i)->Tuple;
                        curr->Next = *list;
                        *list = curr;
                    }
                    if (count) (*count)++;
                }
            }


            return true;
        }


        if (list) *list = NULL;
        if (count) *count = 0;
        return false;
    }
    //Insertas nueva entrada llamando al insert
    bool RTInsertTuple(RTreePtr* N, RTdimension I[], void* Tuple) {
        if (!N || !*N) {

            return false;
        }

        if (!I || !Tuple) {

            return false;
        }


        return Insert(N, LEVEL_LEAF, I, Tuple, NULL);
    }

    bool Insert(struct Node** N, size_t Level, RTdimension I[], void* Tuple, struct Node* TupleNode) {
        struct Node* L = NULL, * LL = NULL, * splitL = NULL, * splitR = NULL, * newRoot = NULL;
        RTdimension* Size;
        size_t Start;
        childIdx h, i;

        Size = I;
        if (TupleNode)
            Size = TupleNode->I;
        else if (!Tuple) {

            *N = NULL;
            return false;
        }
        for (Start = 0, L = *N; L->Child; ++Start, L = L->Child);


        if (Level == LEVEL_TOP)
            Level = Start;

        //Encuentra posicion para el nuevo index record
        if (!ChooseLeaf(*N, Start, Level, Size, &L)) {
            *N = NULL;
            return false;
        }

        //Añade a la hoja
        for (i = 0; i < M; ++i) {
            if (IS_EMPTY(L->Child[i])) {
                if (Tuple) {
                    memcpy(L->Child[i].I, I, sizeof(L->Child[i].I));
                    L->Child[i].Child = NULL;
                    L->Child[i].Tuple = Tuple;
                    memcpy(L->Child + i, TupleNode, sizeof(L->Child[i]));
                }

                L->Child[i].Parent = L;

                // Actualizamos data
                if (L->Child[i].Child != NULL) {
                    for (h = 0; h < M && !IS_EMPTY(L->Child[i].Child[h]); ++h) {
                        L->Child[i].Child[h].Parent = L->Child + i;
                    }
                    break;
                }
            }
            if (i == M) {
                if (!LinearSplit(L, I, Tuple, TupleNode, &LL)) {
                    *N = NULL;
                    return false;
                }
            }
            //Propagar haci arriba
            if (!AdjustTree(L, LL, &splitL, &splitR)) {
                *N = NULL;
                return false;
            }

            //Hacemos crecer el árbol
            if (splitR) {
                newRoot = (struct Node*)mem_alloc(sizeof(struct Node));
                ConsRtree(newRoot, 1);
                newRoot->Child = (struct Node*)mem_alloc(M * sizeof(struct Node));
                ConsRtree(newRoot->Child, M);

                if (!Insert(&newRoot, LEVEL_TOP, NULL, NULL, splitL) || !Insert(&newRoot, LEVEL_TOP, NULL, NULL, splitR)) {
                    *N = NULL;
                    return false;
                }
                free(splitL);
                free(splitR);
                *N = newRoot;
                return true;
            }

            *N = splitL;
            return true;
        }

    }
    bool ChooseLeaf(struct Node* N, size_t Start, size_t Stop, RTdimension* I, struct Node** leaf) {
        childIdx i = 0;
        dimIdx j = 0, k = 0;
        long double area = 0, increase = 0, min = 0, minarea = 0;
        RTdimension expanded[RTn * 2];
        struct Node* F = NULL;


        //Revisa hoja
        if (Start == Stop) {
            *leaf = N;
            return true;

            /*CL3 [Choose subtree]*/
        }
        else if (IS_BRANCH(N)) {
            min = N->I[RTn];
            minarea = areaRec(N->Child[0].I);
            F = N->Child;

            for (i = 0; i < M && !IS_EMPTY(N->Child[i]); ++i) {
                area = areaRec(N->Child[i].I);
                for (j = 0, k = RTn; j < RTn; ++j, ++k) {
                    expanded[j] = (I[j] < N->Child[i].I[j]) ? I[j] : N->Child[i].I[j];
                    expanded[k] = (I[k] > N->Child[i].I[k]) ? I[k] : N->Child[i].I[k];
                }
                increase = areaRec(expanded) - area;

                if (increase < min || (increase == min && area < minarea)) {
                    min = increase;
                    minarea = area;
                    F = N->Child + i;
                }
            }

            /*CL4 [Descend until a leaf is reached]*/
            return ChooseLeaf(F, Start - 1, Stop, I, leaf);
        }


        *leaf = NULL;
        return false;
    }
    bool AdjustTree(struct Node* N, struct Node* NN, struct Node** root, struct Node** split) {
        struct Node* P = NULL, * LS = NULL;
        childIdx h, i;
        dimIdx j, k;

        //Revisa si ya hizo split
        if (N == NULL) {
            *split = NN;
            return true;
        }

        P = N->Parent;
        memcpy(N->I, N->Child[0].I, sizeof(N->I));
        for (i = 0; i < M && !IS_EMPTY(N->Child[i]); ++i) {
            for (j = 0, k = RTn; j < RTn; ++j, ++k) {
                if (N->I[j] > N->Child[i].I[j])
                    N->I[j] = N->Child[i].I[j];
                if (N->I[k] < N->Child[i].I[k])
                    N->I[k] = N->Child[i].I[k];
            }
        }
        if (P && NN) {
            for (i = 0; i < M; ++i) {
                if (IS_EMPTY(P->Child[i])) {
                    memcpy(P->Child + i, NN, sizeof(P->Child[i]));

                    //Actualizo al nuevo hijo del padre
                    P->Child[i].Parent = P;


                    if (P->Child[i].Child != NULL) {
                        for (h = 0; h < M && !IS_EMPTY(P->Child[i].Child[h]); ++h)
                            P->Child[i].Child[h].Parent = P->Child + i;
                    }
                    free(NN);
                    NN = NULL;
                    break;
                }
                if (i == M) {
                    if (!LinearSplit(P, NULL, NULL, NN, &LS)) {
                        *root = NULL;
                        *split = NULL;
                        return false;
                    }
                    free(NN);
                    NN = LS;
                }
                else if (P == NULL && NN) {
                    *root = N;
                    *split = NN;
                    return true;
                }
                //Ir al siguiente nivel (como un next)
                *root = N;
                return AdjustTree(P, NN, root, split);
            }
        }
    }
    bool RTDeleteTuple(RTreePtr* T, RTdimension I[], void* Tuple) {
        if (I == nullptr || Tuple == nullptr) {
            return false;
        }

        return Delete(T, I, Tuple);
    }


    bool Delete(struct Node** T, RTdimension I[], void* Tuple) {
        struct Node* L = nullptr, * newRoot = nullptr;
        childIdx i, pos = 0;

        //Encuentro al nodo a eliminar
        if (!FindLeaf(*T, I, Tuple, &L, &pos)) {
            *T = nullptr;
            return false;
        }

        //Si no lo encuentro
        if (pos == M || L == NULL)
            return false;

        //Si lo encuentro, lo elimino
        memmove(L->Child + pos, L->Child + pos + 1, (M - 1 - pos) * sizeof(L->Child[pos]));
        ConsRtree(L->Child + M - 1, 1);

        //Propagar cambios
        if (!CondenseTree(L, T)) {
            *T = NULL;
            return false;
        }


        //Arbol más pequeño
        if (IS_EMPTY((*T)->Child[1]) && IS_BRANCH(*T)) {
            newRoot = (struct Node*)mem_alloc(sizeof(struct Node));
            memcpy(newRoot, (*T)->Child + 0, sizeof(*newRoot));

            //Actualizo a mi nuevo dato
            for (i = 0; i < M && !IS_EMPTY(newRoot->Child[i]); ++i)
                newRoot->Child[i].Parent = newRoot;

            free((*T)->Child);
            //Borra la info de T, antiguo
            free(*T);
            newRoot->Parent = NULL;
            *T = newRoot;
        }

        return true;
    }

    bool FindLeaf(struct Node* T, RTdimension I[], void* Tuple, struct Node** L, childIdx* position) {
        childIdx i;

        *L = NULL;
        *position = M;

        //Busco subarboles
        if (IS_BRANCH(T)) {
            for (i = 0; i < M && !IS_EMPTY(T->Child[i]); ++i)
                if (Overlap(T->Child[i].I, I)) {
                    if (!FindLeaf(T->Child + i, I, Tuple, L, position)) {
                        *position = M;
                        *L = NULL;
                        return false;
                    }
                    if (*position < M) {
                        return true;
                    }
                }
            return true;


        }
        else if (IS_LEAF(T) || ((T->Child) && IS_EMPTY(T->Child[0]))) {
            for (i = 0; i < M && !IS_EMPTY(T->Child[i]); ++i) {
                if (T->Child[i].Tuple == Tuple && !memcmp(T->Child[i].I, I, sizeof(T->Child[i].I))) {
                    *L = T;
                    *position = i;
                    return true;
                }
            }
            return true;
        }


        *L = NULL;
        *position = M;
        return false;
    }
    bool CondenseTree(struct Node* N, struct Node** root) {
        struct CTNodeList
        {
            int level;
            struct Node* Node;
            struct CTNodeList* Next;
        };
        CTNodeList* Q = NULL, * ptr = NULL, * next = NULL;
        Node* P = NULL;
        childIdx g = 0, h = 0, i = 0;
        dimIdx j = 0, k = 0;
        size_t level = 1;
        while (N->Parent) {
            P = N->Parent;


            for (i = 0; i < m && N->Child[i].Parent; ++i);

            if (i < m) {
                for (i = 0; i < m && !IS_EMPTY(N->Child[i]); ++i) {
                    ptr = (struct CTNodeList*)mem_alloc(sizeof(struct CTNodeList));
                    ptr->Node = (struct Node*)mem_alloc(sizeof(struct Node));
                    ptr->level = level;
                    memcpy(ptr->Node, N->Child + i, sizeof(*ptr->Node));
                    ptr->Next = Q;
                    Q = ptr;
                }

                for (i = 0; i < M && !IS_EMPTY(P->Child[i]); ++i) {
                    if (P->Child + i == N)
                        break;
                }
                free(P->Child[i].Child);
                memmove(P->Child + i, P->Child + i + 1, (M - 1 - i) * sizeof(P->Child[i]));
                ConsRtree(P->Child + M - 1, 1);

                //Actualiza
                for (g = 0; g < M && !IS_EMPTY(P->Child[g]); ++g) {
                    for (h = 0; h < M && !IS_EMPTY(P->Child[g].Child[h]); ++h)
                        P->Child[g].Child[h].Parent = P->Child + g;
                }
            }
            else {
                memcpy(N->I, N->Child[0].I, sizeof(N->I));
                for (i = 0; i < M && !IS_EMPTY(N->Child[i]); ++i) {
                    for (j = 0, k = RTn; j < RTn; ++j, ++k) {
                        if (N->I[j] > N->Child[i].I[j])
                            N->I[j] = N->Child[i].I[j];
                        if (N->I[k] < N->Child[i].I[k])
                            N->I[k] = N->Child[i].I[k];
                    }
                }

            }
            //NEXT!!!
            N = P;
            ++level;
        }
        //Reinsertar nodo que quedaron huérfanos
        for (; Q != NULL; Q = next) {
            if (!Insert(&N, Q->level, NULL, NULL, Q->Node)) {
                for (; Q != NULL; Q = next) {
                    next = Q->Next;
                    free(Q->Node);
                    free(Q);
                }
                return false;
            }
            next = Q->Next;
            free(Q->Node);
            free(Q);
        }
        if (root) *root = N;
        return true;
    }

    bool LinearSplit(struct Node* L, RTdimension I[], void* Tuple, struct Node* Child, struct Node** split) {
        struct Node NL[M + 1], * seedA = NULL, * seedB = NULL, * LL = NULL, * Parent = NULL;
        childIdx i = 0, A = 1, B = 1;
        dimIdx j = 0, k = 0;
        long double Larea = 0, Lincrease = 0, LLarea = 0, LLincrease = 0, width[RTn];
        RTdimension Lexpanded[RTn * 2], LLexpanded[RTn * 2];

        //Copio a los hijos del nodo en un nuevo array
        memcpy(NL, L->Child, M * sizeof(NL[0]));


        if (Tuple) {
            NL[M].Parent = NULL;
            memcpy(NL[M].I, I, sizeof(NL[M].I));
            NL[M].Child = NULL;
            NL[M].Tuple = Tuple;
            memcpy(NL + M, Child, sizeof(NL[M]));
        }
        Parent = L->Parent;

        //Calculando el ancho de los hijos de L
        for (j = 0, k = RTn; j < RTn; ++j, ++k) {
            width[j] = (long double)((NL[M].I[k] > L->I[k]) ? NL[M].I[k] : L->I[k]) - ((NL[M].I[j] < L->I[j]) ? NL[M].I[j] : L->I[j]);
        }

        //Ya sabemos el ancho de los hijos y cuanta area cubren, DAMN it

        LL = (struct Node*)mem_alloc(sizeof(struct Node));
        ConsRtree(LL, 1);
        LL->Child = (struct Node*)mem_alloc(M * sizeof(struct Node));
        ConsRtree(LL->Child, M);

        //Cogemos la primera entrada del array
        if (!LinearPickSeeds(NL, width, &seedA, &seedB)) {
            *split = NULL;
            return false;
        }
        ConsRtree(L->Child, M);
        memset(L->I, 0, sizeof(L->I));
        L->Parent = NULL;
        L->Tuple = NULL;

        if (!Insert(&L, LEVEL_TOP, NULL, NULL, seedA) || !Insert(&LL, LEVEL_TOP, NULL, NULL, seedB)) {
            *split = NULL;
            return false;
        }
        //Revisamos que todo esté bien (Todas las entradas check) y hacemos el next
        for (i = 0; i < M + 1; ++i) {
            if (NL + i != seedA && NL + i != seedB) {
                if ((M + 1) - (A + B) == m - A) {
                    if (!Insert(&L, LEVEL_TOP, NULL, NULL, NL + i)) {
                        *split = NULL;
                        return false;
                    }
                }
                else if ((M + 1) - (A + B) == m - B) {
                    if (!Insert(&LL, LEVEL_TOP, NULL, NULL, NL + i)) {
                        *split = NULL;
                        return false;
                    }
                }
                else {
                    //Nueva entrada
                    Larea = areaRec(L->I);
                    LLarea = areaRec(LL->I);

                    for (j = 0, k = RTn; j < RTn; ++j, ++k) {
                        Lexpanded[j] = (NL[i].I[j] < L->I[j]) ? NL[i].I[j] : L->I[j];
                        Lexpanded[k] = (NL[i].I[k] > L->I[k]) ? NL[i].I[k] : L->I[k];
                        LLexpanded[j] = (NL[i].I[j] < LL->I[j]) ? NL[i].I[j] : LL->I[j];
                        LLexpanded[k] = (NL[i].I[k] > LL->I[k]) ? NL[i].I[k] : LL->I[k];
                    }

                    Lincrease = areaRec(Lexpanded) - Larea;
                    LLincrease = areaRec(LLexpanded) - LLarea;

                    if (Lincrease < LLincrease || (Lincrease == LLincrease && Larea < LLarea)) {
                        if (!Insert(&L, LEVEL_TOP, NULL, NULL, NL + i)) {
                            *split = NULL;
                            return false;
                        }
                        ++A;
                    }
                    else if (Lincrease > LLincrease || (Lincrease == LLincrease && Larea > LLarea)) {
                        if (!Insert(&LL, LEVEL_TOP, NULL, NULL, NL + i)) {
                            *split = NULL;
                            return false;
                        }
                        ++B;
                    }
                    else if (A < B) {
                        if (!Insert(&L, LEVEL_TOP, NULL, NULL, NL + i)) {
                            *split = NULL;
                            return false;
                        }
                        ++A;
                    }
                    else {
                        if (!Insert(&LL, LEVEL_TOP, NULL, NULL, NL + i)) {
                            *split = NULL;
                            return false;
                        }
                        ++B;
                    }
                }
            }
        }

        L->Parent = Parent;
        *split = LL;
        return true;
    }
    bool LinearPickSeeds(struct Node NL[], long double* width, struct Node** hbest, struct Node** lbest) {
        long double separation = 0.0, sbest = -1.0;
        childIdx i = 0, low = 0, high = 1;
        dimIdx j = 0, k = 0;

        //Extreme rectangulos
        for (j = 0, k = RTn; j < RTn; ++j, ++k) {
            for (i = 0; i < M; ++i) {
                if (NL[i].I[j] > NL[low].I[j])
                    if (i != high)
                        low = i;
                if (NL[i].I[k] < NL[high].I[k])
                    if (i != low)
                        high = i;
            }

            //Checa su forma, ajústala
            separation = ((long double)NL[low].I[j] - NL[high].I[k]) / width[j];

            // Pares más extremos
            if (separation > sbest) {
                *hbest = NL + high;
                *lbest = NL + low;
                sbest = separation;
            }
        }

        if (*hbest != *lbest)
            return true;


        *hbest = NULL;
        *lbest = NULL;
        return false;
    }

};
