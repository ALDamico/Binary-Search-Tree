/*---------------------------------------------------------------------------
	Autore: Andrea Luciano Damico
	Data: 15/07/2018
	email: aldamico90@gmail.com
---------------------------------------------------------------------------*/

/*
ISTRUZIONI DI COMPILAZIONE
Per poter compilare correttamente questo programma, occorre usare il comando
g++ main.cpp -o bstree -std=c++11 -g

La compilazione con uno standard C++ precedente al C++11 fallirà perché questa
implementazione fa uso di alcune caratteristiche presenti negli standard 2011 e
successivi.
*/

#include <iostream> //Operazioni di input/output su stdin e stdout
#include <fstream> //Operazioni di input/output su file
#include <string> //Per il parametro della funzione print()
#include <algorithm> //Per funzione std::max()

/*
Classe nodo "standard" per albero binario di ricerca.
È possibile scriverla in modi diversi, per esempio usando una struct in modo da
non dover usare i metodi getter e setter (ma si rompe l'incapsulamento), oppure
scrivendo una classe interamente privata (anche il costruttore!) e friend della 
classe BSTree (ma in questo caso) si hanno problemi con l'ereditarietà, in 
quanto la friendship non è né transitiva, né reciproca, né ereditaria. Se si 
eredita da BSTree, bisogna modificare BTNode per renderla friend della classe 
figlia.
*/
template <typename T>
class BTNode {
public:
	/*
	Costruttore che inizializza il nodo con il parametro data e setta tutti i 
	puntatori a nullptr.

	D: Perché non un costruttore telescopico?
	R: Il costruttore telescopico è un modo di scrivere i costruttori molto 
	comune nel Java. Consiste nello scrivere diversi costruttori con un numero
	variabile di parametri che richiamano un costruttore predefinito.
	Non ha senso usare questo tipo di costruttore in C++ per tre motivi:
	1) Il costruttore telescopico aggiunge complessità al codice e non è rapido
	   da scrivere.
	2) Il C++ supporta i parametri di default, che ci consentono di scrivere un
	   unico costruttore in cui alcuni parametri sono opzionali.
	3) Le operazioni di assegnazione dei puntatori vengono fatte all'interno
	   della classe BSTree.
	*/
	BTNode(T data = 0) {
		this->data = data;
		parent = left = right = nullptr;
	}

	/*
	Distruttore della classe BTNode

	D: Perché un distruttore?
	R: Perché ci consente di scrivere un distruttore per la classe BSTree in
	maniera molto semplice. Ci basterà infatti effettuare la delete di root per
	cancellare tutto l'albero.

	NOTA: l'operazione di delete di un nodo all'interno del metodo di
	cancellazione della classe BSTree avviene SOLO su nodi che non hanno figli.
	*/
	~BTNode() {
		delete left;
		delete right;
	}

	//Metodi getter per accedere ai dati del nodo
	T get_data() { return data; }
	BTNode * get_parent() { return parent; }
	BTNode * get_left() { return left; }
	BTNode * get_right() { return right; }

	//Metodi setter per modificare i dati del nodo
	void set_data (T data) { this->data = data; }
	void set_parent(BTNode *parent) { this->parent = parent; }
	void set_left(BTNode *left) { this->left = left; }
	void set_right(BTNode *right) { this->right = right; }

	/*
	Metodo che distacca completamente un nodo da tutti i nodi adiacenti.
	Usare con cautela *SOLO* quando si effettua la delete di un nodo
	all'interno della procedura del() del BSTree.

	D: Ma Faro non fa la delete!
	R: Quindi se Faro si butta dal balcone tu fai lo stesso?
	*/
	void make_orphan() {
		set_parent(nullptr);
		set_left(nullptr);
		set_right(nullptr);
	}
private:
	//Dato da contenere all'interno del nodo
	T data;
	BTNode 	*parent, //Puntatore al genitore
			*left, //Puntatore al figlio sinistro
			*right; //Puntatore al figlio destro
};

/*
Classe BSTree che implementa un albero binario di ricerca.
*/
template <typename T>
class BSTree {
public:
	//Costruttore che crea un albero vuoto
	BSTree() { root = nullptr; }

	//Distruttore che elimina ricorsivamente un albero
	~BSTree() { delete root; }

	//Funzione di inserimento di elementi all'interno dell'albero
	void insert(T data) {
		//Creiamo un nuovo nodo
		BTNode<T> *leaf = new BTNode<T>(data);
		/*
		Due puntatori che ci servono per esplorare l'albero. 
		Alla fine della ricerca, y punterà al genitore del nodo che vogliamo 
		inserire.
		*/
		BTNode<T> 	*x = root,
					*y = nullptr;
		while (x) {
			y = x;
			if (data <= x->get_data())
				x = x->get_left();
			else
				x = x->get_right();
		}
		//Fine dell'iterazione: y è *sicuramente* il genitore di leaf...
		leaf->set_parent(y);
		/* 
		...ma potrebbe essere un puntatore nullo! In questo caso il nuovo 
		   elemento leaf è la radice
		*/
		if (!y)
			root = leaf;
		/* 
		Altrimenti, controlliamo il valore del dato di leaf (che è anche il 
		parametro della funzione) e settiamo leaf come figlio sinistro se il 
		suo valore è <= a quello di y, altrimenti lo settiamo come figlio 
		destro.
		*/
		else if (data <= y->get_data())
			y->set_left(leaf);
		else
			y->set_right(leaf);
	}

	/*
	Metodo pubblico che utilizza l'omonima funzione privata per cercare il
	valore minimo presente all'interno dell'albero, se presente.
	Se non presente, restituisce -1 (valore scelto arbitrariamente)
	*/
	T min() {
		//Se il minimo della radice esiste == se la radice esiste...
		if (min(root))
			return min(root)->get_data();
		//Altrimenti restituiamo -1
		else 
			return -1;
	}

	/*
	Come sopra, ma restituisce il massimo valore contenuto nell'albero, se
	presente.
	*/
	T max() {
		if (max(root))
			return max(root)->get_data();
		else
			return -1;
	}

	/*
	Funzione pubblica di stampa dell'albero. Consente di scegliere tra i tre
	diversi tipi di visita (inorder, postorder e preorder), oltre che su quale
	output stream scrivere.
	Se l'output stream non viene specificato, il parametro di default è 
	std::cout
	*/
	void print(std::string method, std::ostream &os = std::cout) {
		if (method == "inorder")
			inorder(root, os);
		else if (method == "preorder")
			preorder(root, os);
		else if (method == "postorder")
			postorder(root, os);
		else
			std::cerr << "ERROR! Invalid traversal method!";
		/*
		Istruzione richiesta per tornare a capo dopo aver stampato tutto
		l'albero.
		*/
		os << std::endl;
	}

	/*
	Funzione pubblica di ricerca di un valore all'interno dell'albero.
	Utilizza l'omonima funzione privata di ricerca ricorsiva.
	*/
	bool search(T data) {
		//Eseguiamo la ricerca a partire dalla radice.
		return search(root, data);
	}

	/*
	Funzione pubblica che restituisce il valore del successore di un certo nodo.
	*/
	T successor(T value) {
		BTNode<T> *s = successor(root, value);
		if (!s)
			return -1;
		return s->get_data();
	}

	/*
	Funzione pubblica che restituisce il valore del predecessore di un certo
	nodo.
	*/
	T predecessor(T value) {
		BTNode<T> *s = predecessor(root, value);
		if (!s)
			return -1;
		return s->get_data();
	}

	/*
	Funzione pubblica di cancellazione.

	D: Perché chiamiamo del su root?
	R: Perché la funzione privata esegue una ricerca *a partire* dal nodo che
	   gli viene passato come parametro. Il nodo r non è il nodo da cancellare,
	   ma la radice del sottoalbero in cui si cerca.
	*/
	void del(T value) { del(root, value); }

	/*
	Funzione che calcola l'altezza dell'albero, cioè il percorso più lungo dalla
	radice alle foglie.
	*/
	int height() {
		/*
		Applichiamo la funzione privata di altezza alla radice del nostro albero
		*/
		return height(root);
	}

	/*
	Funzione pubblica che calcola ricorsivamente la profondità di un nodo.
	*/
	int depth(T value) {
		BTNode<T> *n = search(root, value);
		return depth(n);
	}

	/*
	Funzione pubblica che calcola iterativamente la profondità di un nodo.
	*/
	int depth_iterative(T value) {
		BTNode<T> * n = search(root, value);
		return depth_iterative(n);
	}

	/*
	Funzione che sostituisce le chiavi dell'albero con gli elementi di un array
	*ordinato*.
	*/
	void natural_fill(T *a, int size) {
		BTNode<T> *m = min(root);
		int i = 0;
		while (m) {
			m->set_data(a[i]);
			m = successor(m, m->get_data());
			i++;
		}
	}
private:
	/*
	Metodo privato di visita inorder (sinistra, radice, destra).
	Il parametro os ci consente di scrivere non solo su stdout ma anche su file
	*/
	void inorder(BTNode<T> *n, std::ostream& os) {
		if (n) {
			inorder(n->get_left(), os);
			os << n->get_data() << " ";
			inorder(n->get_right(), os);
		}
	}

	/*
	Metodo privato di visita preorder (radice, sinistra, destra)
	*/
	void preorder(BTNode<T> *n, std::ostream& os) {
		if (n) {
			os << n->get_data() << " ";
			preorder(n->get_left(), os);
			preorder(n->get_right(), os);
		}
	}

	/*
	Metodo privato di visita postorder (sinistra, destra, radice)
	*/
	void postorder(BTNode<T> *n, std::ostream &os) {
		if (n) {
			postorder(n->get_left(), os);
			postorder(n->get_right(), os);
			os << n->get_data() << " ";
		}
	}

	/*
	Metodo privato e ricorsivo che restituisce un puntatore al nodo contenente 
	il valore minimo di un sottoalbero.
	Questo metodo utilizza la proprietà degli alberi binari di ricerca secondo
	la quale ciascun nodo di un albero binario di ricerca è esso stesso un
	albero.
	*/
	BTNode<T> *min(BTNode<T> *n) {
		/*
		Caso particolare: il nodo n non esiste == albero vuoto
		Restituiamo un puntatore nullo.
		*/
		if (!n)
			return nullptr;
		/*
		Proprietà degli alberi binari di ricerca: il figlio sinistro ha *sempre*
		un valore minore o uguale a quello del padre.
		Se un nodo non ha il figlio sinistro, significa che non esistono valori
		minori di quello del nodo in questione, quindi questo nodo è il minimo.
		*/
		/*
		Se n non ha il figlio sinistro è il valore minimo: restituiamo n
		(caso base)
		*/
		if (!n->get_left())
			return n;
		/*
		Se non siamo usciti dalla funzione nell'istruzione precedente, dobbiamo
		procedere al passo induttivo. Applichiamo la funzione stessa al figlio
		sinistro di n
		*/
		return min(n->get_left());
	}

	/*
	Come sopra, ma restituisce un puntatore al nodo contenente il valore massimo
	di un albero binario di ricerca
	*/
	BTNode<T> * max(BTNode<T> *n) {
		if (!n)
			return nullptr;
		if (!n->get_right())
			return n;
		return max(n->get_right());
	}

	/*
	Funzione privata ricorsiva di ricerca di un determinato valore.
	Il nodo r punta alla radice del *sottoalbero* in cui stiamo cercando.
	*/
	BTNode<T> * search(BTNode<T> *r, T data) {
		/*
		Caso particolare: la radice che abbiamo scelto non esiste.
		Restituiamo un puntatore nullo.
		*/
		if (!r)
			return nullptr;
		/*
		Caso base: il parametro data è uguale al valore contenuto nel nodo r.
		Restituiamo r.
		*/
		if (data == r->get_data())
			return r;

		/*
		Passo induttivo. Controlliamo il valore di r e decidiamo se continuare
		la ricerca nel sottoalbero destro o in quello sinistro.

		D: Bisogna per forza usare return?
		R: Sì, altrimenti il risultato della funzione search viene ignorato.
		*/
		if (data < r->get_data())
			return search(r->get_left(), data);
		else
			return search(r->get_right(), data);
	}

	/*
	Funzione privata che restituisce il successore di un nodo n
	*/
	BTNode<T> * successor (BTNode<T> *n, T data) {
		//Cerchiamo il valore data
		BTNode<T> * s = search(n, data);
		//Se un nodo del genere non esiste, restituiamo un puntatore nullo
		if (!s)
			return nullptr;
		//Se il nodo esiste, ci sono due possibilità:

		/*
		s ha il figlio destro: in questo caso, il successore si trova nel
		sottoalbero destro e dobbiamo trovare il valore minimo di quel
		sottoalbero
		*/
		if (s->get_right())
			return min(s->get_right());

		/*
		s *NON* ha il figlio destro. Il successore si trova in uno dei genitori.
		Dobbiamo trovare un nodo che sia un figlio sinistro.
		*/
		BTNode<T> * p = s->get_parent();
		while (p && s == p->get_right()) {
			s = p;
			p = p->get_parent();
		}
		return p;
	}

	/*
	Funzione di ricerca del predecessore.
	Simile alla funzione di ricerca del successore, ma dobbiamo restituire il 
	massimo del figlio sinistro anziché il minimo del figlio destro.
	Inoltre, se il puntatore s non ha un figlio sinistro, dobbiamo tornare a
	ritroso fino a trovare un nodo che abbia un figlio *destro* anziché 
	sinistro.
	*/
	BTNode<T> * predecessor(BTNode<T> *n, T data) {
		BTNode<T> * s = search(n, data);
		if (!s)
			return nullptr;
		if (s->get_left())
			return max(s->get_left());
		BTNode<T> * p = s->get_parent();
		while (p && s == p->get_left()) {
			s = p;
			p = p->get_parent();
		}
		return p;
	}

	/*
	Funzione privata di cancellazione di un elemento dall'albero.
	r è la radice del sottoalbero in cui effettuiamo la ricerca, *NON* il nodo
	da cancellare.
	data è il valore che intendiamo cancellare
	*/
	void del(BTNode<T> *r, T data) {
		//Cerchiamo se esiste un nodo contenente il valore data
		BTNode<T> * n = search(r, data);
		//Se non esiste, usciamo dalla funzione
		if (!n)
			return;
		/*
		Se il nodo che abbiamo trovato non ha il figlio destro, il figlio
		sinistro o nessuno dei due...
		*/
		if (!n->get_left() || !n->get_right()) {
			//Genitore del nodo n
			BTNode<T> * p = n->get_parent();
			//Cerchiamo se esiste un figlio
			BTNode<T> * c = n->get_left();
			//Se c è un puntatore nullo, proviamo col figlio destro
			if (!c)
				c = n->get_right();
			//n ha un genitore? Se non lo ha, c è la nuova radice
			if (!p)
				root = c;
			else {
				//Altrimenti impostiamo i figli correttamente
				if (n == p->get_left())
					p->set_left(c);
				else
					p->set_right(c);
			}
			/*
			Se il nodo n ha un figlio, impostiamo il genitore. Altrimenti 
			(se è una foglia) non facciamo nulla.
			*/
			if (c)
				c->set_parent(p);
			/*
			n potrebbe avere un figlio. Eseguendo la delete su n, cancelleremo
			TUTTO il sottoalbero che ha n come radice.
			Per evitare questo effetto collaterale, dobbiamo staccare n da
			tutti i nodi adiacenti con la funzione make_orphan()
			*/
			n->make_orphan();
			delete n;
			return;
		}
		else { //In questo caso, il nodo ha due figli
			//Ricerchiamo il successore
			BTNode<T> * succ = successor(n, data);
			//Sostuiamo il valore di n con quello del successore
			n->set_data(succ->get_data());
			/*
			Richiamiamo la cancellazione sul sottoalbero destro per eliminare il
			duplicato che abbiamo appena inserito.
			Eseguiamo la cancellazione sul sottoalbero destro perché siamo
			sicuri che il successore si trova a destra del nodo n (se ci
			troviamo in questo caso, n ha *sicuramente* due figli)
			*/
			del(n->get_right(), succ->get_data());
		}
	}

	/*
	Funzione ricorsiva per il calcolo dell'altezza di un sottoalbero con radice
	r.
	*/
	int height(BTNode<T> *r) {
		//Se r non esiste, l'albero ha altezza 0 (caso base)
		if (!r)
			return 0;
		//Calcoliamo l'altezza del sottoalbero sinistro
		int leftHeight = height(r->get_left());
		//Calcoliamo l'altezza del sottoalbero destro
		int rightHeight = height(r->get_right());
		//Restituiamo 1 + l'altezza più grande tra i due sottoalberi
		return 1 + std::max(leftHeight, rightHeight);
	}

	/*
	Funzione ricorsiva per il calcolo della profondità di un nodo.
	È inoltre possibile scriverne una versione iterativa.
	*/
	int depth(BTNode<T> *n) {
		//Se n non esiste restituiamo -1
		if (!n)
			return -1;
		//Se n non ha il genitore, significa che è la radice
		if (!n->get_parent())
			return 1;
		/*
		Se n ha il genitore, applichiamo il passo induttivo restituendo 1
		sommato al risultato di depth() applicato al genitore di n
		*/
		return 1 + depth(n->get_parent());
	}

	/*
	Funzione iterativa per il calcolo della profondità di un nodo.
	*/
	int depth_iterative(BTNode<T> *n) {
		if (!n)
			return -1;
		/*
		Una variabile che tiene traccia di quanti passi abbiamo fatto verso la
		radice
		*/
		int d = 1;
		//Finché n non è la radice, procediamo a ritroso e incrementiamo d
		while (n != root) {
			n = n->get_parent();
			d++;
		}
		//d sarà il valore della profondità del sottoalbero
		return d;
	}

	//Puntatore alla radice dell'albero
	BTNode<T> *root;
};

int main() {
	BSTree<int> t;
	//Test inserimento
	for (int i = 0; i < 100; i++)
		t.insert(rand() % 100);
	//Stampa inorder (ordine crescente)
	std::cout << "Stampa inorder dell'albero:" << std::endl;
	t.print("inorder");
	//Stampa preorder
	std::cout << "Stampa preorder dell'albero:" << std::endl;
	t.print("preorder");
	//Stampa postorder
	std::cout << "Stampa postorder dell'albero:" << std::endl;
	t.print("postorder");
	//Dimostrazione funzione di ricerca
	std::cout << std::boolalpha << "50 è presente: " << t.search(50) 
		<< std::endl
		<< "51 è presente: " << t.search(51) << std::endl;
	//Dimostrazione funzioni minimo e massimo
	std::cout << "Valore minimo: " << t.min() << "\tValore massimo: " 
		<< t.max() << std::endl;
	//Dimostrazione funzioni di altezza e profondità
	std::cout << "Altezza dell'albero: " << t.height() << "\tProfondità di 50: "
		<< t.depth(50) << std::endl;
	int *a = new int[100];
	for (int i = 0; i < 100; i++)
		a[i] = i + 200;
	//Dimostrazione natural fill
	std::cout << a[99] << std::endl;
	std::cout << "Riempimento naturale con i numeri da 200 a 299:" << std::endl;
	t.natural_fill(a, 100);
	t.print("inorder");
	return 0;
}