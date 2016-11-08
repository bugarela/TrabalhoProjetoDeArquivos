#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <set>
#include <ostream>
#include "ServerSocket.h"
#include "SocketException.h"
#include <string>
using namespace std;

#define NUMERO_DE_COLUNAS 21

struct dado{
	int RowID, OrderID, tamanho;
};

// sobrecarregando o operador < para fazer map com id composto =)
bool operator< (dado d1, dado d2){
	if(d1.OrderID != d2.OrderID || d1.RowID == -1 || d2.RowID == -1)
    	return (d1.OrderID < d2.OrderID);
    else
    	return (d1.RowID < d2.RowID);
}

bool operator> (dado d1, dado d2){
	if(d1.OrderID != d2.OrderID || d1.RowID == -1 || d2.RowID == -1)
    	return (d1.OrderID > d2.OrderID);
    else
    	return (d1.RowID > d2.RowID);
}

bool operator== (dado d1, dado d2){
	if(d1.RowID == -1 || d2.RowID == -1)
    	return (d1.OrderID == d2.OrderID);
    else
    	return (d1.OrderID == d2.OrderID && d1.RowID == d2.RowID);
}

// vê se o separador ';' está entre aspas
bool verificaAspas(string a){
	int c=0;
	for(int i=0;i<a.size();i++)
		if(a[i]=='"')
			c++;
	return (c%2==0);
}

dado criaDado(string linhaRegistros){
	dado busca;
	
	if(linhaRegistros.find(';') > linhaRegistros.size()){
		busca.OrderID = stoi(linhaRegistros.substr(0,linhaRegistros.find(',')));
		busca.RowID = stoi(linhaRegistros.substr(linhaRegistros.find(',')+1,linhaRegistros.size()));
		busca.tamanho = -1;
	} else {
		busca.RowID = stoi(linhaRegistros.substr(0,linhaRegistros.find(';')));
		busca.OrderID = stoi(linhaRegistros.substr(linhaRegistros.find(';')+1,linhaRegistros.find(';')+1));
		busca.tamanho = linhaRegistros.size();
	}
	
	return busca;
}

vector<string> criaRegistro (string linhaRegistros, int tamanhos[]){
	
	vector <string> vetorRegistros;
	vetorRegistros.resize(NUMERO_DE_COLUNAS);
	
	int inicioRegistroAnterior = -1, contadorRegistros = 0, inicioRegistro;
	
	bool linhaVazia = linhaRegistros.find(';',0) > linhaRegistros.size();
	
	string registro;
	
	while(contadorRegistros < NUMERO_DE_COLUNAS && !(linhaVazia)){
		
		if(contadorRegistros < NUMERO_DE_COLUNAS - 1)
			inicioRegistro = linhaRegistros.find(';',inicioRegistroAnterior+1);
		else
			inicioRegistro = linhaRegistros.size() + inicioRegistroAnterior + 1;
			
		registro = linhaRegistros.substr(inicioRegistroAnterior + 1, inicioRegistro - inicioRegistroAnterior - 1);
		inicioRegistroAnterior = inicioRegistro;
		
		if(!verificaAspas(registro)){
			
			inicioRegistro = linhaRegistros.find(';',inicioRegistroAnterior + 1);
			registro += linhaRegistros.substr(inicioRegistroAnterior + 1,inicioRegistro - inicioRegistroAnterior - 1);
			inicioRegistroAnterior = inicioRegistro;
			
		}
		
		if(registro.size() > tamanhos[contadorRegistros])
				tamanhos[contadorRegistros] = registro.size();
		
		vetorRegistros[contadorRegistros] = registro;		
		contadorRegistros++;
	}
						
	return vetorRegistros;
}

map <dado,int> insereIndice (string linhaRegistros, map <dado,int> arvore, vector <int> tamanhos){
	
	dado novoDado;
	novoDado = criaDado(linhaRegistros);	
	arvore[novoDado] = arvore.size();
	tamanhos.push_back(novoDado.tamanho);
	cout << novoDado.OrderID << " id" << endl;
	return arvore;
}

void inicializaArquivo (){
	int tamanhos[NUMERO_DE_COLUNAS];
	bool naoPrimeira = false;
	string linhaRegistros;
	dado dadoDoRegistro;
	
	ifstream arquivoRegistros ("Sample - Superstore Sales.csv");
	ofstream indices ("indices.txt");
	
    if (arquivoRegistros.is_open() && indices.is_open()){
        for(int i=0;!arquivoRegistros.eof();i++){
			
			getline (arquivoRegistros,linhaRegistros);
						
			if(naoPrimeira && linhaRegistros.size() > 0){
				dadoDoRegistro = criaDado(linhaRegistros);
				indices << dadoDoRegistro.OrderID << " " << dadoDoRegistro.RowID << " " << dadoDoRegistro.tamanho << " " << i << endl;         
			} else { cout << "a" << linhaRegistros.size() << endl; naoPrimeira=true;}
        }
        arquivoRegistros.close();
        indices.close();
    }
}

void atualizaArquivos (map <dado,int> arvore){
	
	int tamanhos[NUMERO_DE_COLUNAS];
	vector < vector<string> > colunas;
	bool naoPrimeira = false;
	string linhaRegistros;
	dado dadoDoRegistro;
	
	ifstream arquivoRegistros ("Sample - Superstore Sales.csv");
	ofstream indices ("indices.txt");
	
	map<dado,int>::iterator it;
	
    if (arquivoRegistros.is_open() && indices.is_open()){
        while (! arquivoRegistros.eof() ){
			
			getline (arquivoRegistros,linhaRegistros);
			
			if(naoPrimeira){
				dadoDoRegistro = criaDado(linhaRegistros);
				it = arvore.find(dadoDoRegistro);
				if(it != arvore.end()){
					indices << dadoDoRegistro.OrderID << " " << dadoDoRegistro.RowID << " " << dadoDoRegistro.tamanho << " " << it->second << endl;
            		colunas.push_back(criaRegistro(linhaRegistros,tamanhos));
            	}
            		
			} else {
				colunas.push_back(criaRegistro(linhaRegistros,tamanhos));
           		naoPrimeira = true;
           	}   	            
        }
        arquivoRegistros.close();
        indices.close();
    }
	
	ofstream saida ("saida.pra");
	string registro;
	
    if(saida.is_open()){
		
		for(int i=0;i<colunas.size();i++){
			for(int j=0;j<NUMERO_DE_COLUNAS;j++){
				
				registro = colunas[i][j];
				
				if(j==0 || j==1 || j==4 || j==5 || j==6 || j==8 || j==9 || j==10 || j==19){
					for(int k=registro.size();k<tamanhos[j];k++)
						saida << " ";	
					saida << registro << " ";
		
				}else{
					saida << registro;
					for(int k=registro.size();k<=tamanhos[j];k++)
						saida << " ";
				}
			}
			saida << endl;
		}
		saida.close();
	}
}

map <dado,int> leIndices (vector <int> tamanhos){
	
	dado indiceTemporario;
	int posicao;
	map <dado,int> arvore;
	ifstream arquivoIndices ("indices.txt");
	if (arquivoIndices.is_open()){
        while (! arquivoIndices.eof() ){
			arquivoIndices >> indiceTemporario.OrderID >> indiceTemporario.RowID >> indiceTemporario.tamanho >> posicao;
			arvore[indiceTemporario] = posicao;
			tamanhos.push_back(indiceTemporario.tamanho);
                        
        }
        arquivoIndices.close();
	}
	
	return arvore;
}

void escreveRegistro (string linhaRegistros){
	
	ofstream saida ("Sample - Superstore Sales.csv", ios::app);
	if(saida.is_open())
		saida << linhaRegistros << endl;	
	saida.close();
}

int encontraPosicao(dado busca, map <dado,int> arvore){
	
	if(arvore.find(busca) == arvore.end())
		return -1;
	
	dado encontrado = arvore.find(busca)->first;		
	int posicao = arvore.find(busca)->second;
	
	arvore.erase(encontrado);
	if(arvore.count(busca) == 0){
		arvore[encontrado] = posicao;
		return posicao;
	}
	
	arvore[encontrado] = posicao;
	return 0;	
	
}

string pegaRegistro (int indice, vector <int> tamanhos){
	
	ifstream arquivoRegistros ("Sample - Superstore Sales.csv");
	int posicao;
	for(int i=0;i<indice;i++)
		posicao += tamanhos[i];
		
	arquivoRegistros.seekg (posicao,ios::cur);
	string linhaRegistros;
	getline (arquivoRegistros,linhaRegistros);
	return linhaRegistros;
}

int main(){
	
	cout << ">Servidor rodando..." << endl;
	
	vector <int> tamanhos;
	tamanhos.resize(0);
	
    string comando;
	int posicao;
	map <dado,int> arvore = leIndices(tamanhos);
	if(arvore.size() == 0){
		cout << ">Arquivos precisam ser inicializados. Inicializando..." << endl;
		inicializaArquivo();
		arvore = leIndices(tamanhos);
	}
	
	cout << ">Arvore carregada com " << arvore.size() << " registros." << endl;
	
	try{
		ServerSocket server ( PORTA );

		while (1){
			ServerSocket new_sock;
			server.accept ( new_sock );
			cout << ">Cliente conectado." << endl;
			while(1){
				new_sock >> comando;
				cout << ">Comando recebido: " << comando << endl;
				switch(comando[0]){
				
					case 'f':
						cout << ">Iniciando busca" << endl;
						comando.erase(0,1);
						posicao = encontraPosicao(criaDado(comando), arvore);
						cout << posicao << endl;
					
						if(posicao == -1)
							new_sock << "404";
							
						else if(posicao > 0){
							//cria json;
							new_sock << "Sucesso";
						} else
							new_sock << "RowIDNecessaria";
								
						break;
						
					case 'i': 	
					
						comando.erase(0,1);
						escreveRegistro(comando);						
						arvore = insereIndice(comando, arvore, tamanhos);
						new_sock << "Inserido com sucesso!";
						break;
					
					case 'r':
						
						comando.erase(0,1);
						posicao = encontraPosicao(criaDado(comando), arvore);
						
						if(posicao == -1)					
							new_sock << "404";
			
						else if(posicao > 0){		
							arvore.erase(criaDado(comando));
							new_sock << "Sucesso";
								
						} else
							new_sock << "RowIDNecessaria";
						
						break;
				}
			}
		}
	
	} catch ( SocketException& e ){
			cout << "Exception was caught:" << e.description() << "\nExiting.\n";
	}

    return 0;
}
