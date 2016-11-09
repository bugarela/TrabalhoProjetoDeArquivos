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

int tamanhoRegistro[NUMERO_DE_COLUNAS];
vector <int> tamanhos;

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

vector<string> criaRegistro (string linhaRegistros){
	
	vector <string> vetorRegistros;
	vetorRegistros.resize(NUMERO_DE_COLUNAS);
	
	int inicioRegistroAnterior = -1, contadorRegistros = 0, inicioRegistro;
	
	bool linhaVazia = linhaRegistros.find(';',0) > linhaRegistros.size();
	
	string registro;
	
	while(contadorRegistros < NUMERO_DE_COLUNAS && !(linhaVazia)){
		
		inicioRegistro = linhaRegistros.find(';',inicioRegistroAnterior+1);
		if(contadorRegistros == NUMERO_DE_COLUNAS - 1)
			inicioRegistro = linhaRegistros.size()-1;
			
		registro = linhaRegistros.substr(inicioRegistroAnterior + 1, inicioRegistro - inicioRegistroAnterior - 1);
		inicioRegistroAnterior = inicioRegistro;
		
		if(!verificaAspas(registro)){
			
			inicioRegistro = linhaRegistros.find(';',inicioRegistroAnterior + 1);
			registro += linhaRegistros.substr(inicioRegistroAnterior + 1,inicioRegistro - inicioRegistroAnterior - 1);
			inicioRegistroAnterior = inicioRegistro;
			
		}
		
		if(registro.size() > tamanhoRegistro[contadorRegistros])
				tamanhoRegistro[contadorRegistros] = registro.size();
		
		vetorRegistros[contadorRegistros] = registro;		
		contadorRegistros++;
	}
						
	return vetorRegistros;
}

map <dado,int> insereIndice (string linhaRegistros, map <dado,int> arvore){
	
	dado novoDado;
	novoDado = criaDado(linhaRegistros);	
	arvore[novoDado] = arvore.size();
	tamanhos.push_back(novoDado.tamanho);
	return arvore;
}

void inicializaArquivo (){

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
			} else 
				naoPrimeira=true;
        }
        arquivoRegistros.close();
        indices.close();
    }
}

void atualizaArquivos (map <dado,int> arvore){
	
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
            		colunas.push_back(criaRegistro(linhaRegistros));
            	}
            		
			} else {
				colunas.push_back(criaRegistro(linhaRegistros));
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
					for(int k=registro.size();k<tamanhoRegistro[j];k++)
						saida << " ";	
					saida << registro << " ";
		
				}else{
					saida << registro;
					for(int k=registro.size();k<=tamanhoRegistro[j];k++)
						saida << " ";
				}
			}
			saida << endl;
		}
		saida.close();
	}
}

map <dado,int> leIndices (){
	
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

string pegaRegistro (int indice){
	
	ifstream arquivoRegistros ("Sample - Superstore Sales.csv");
	int posicao = 0;
	for(int i=0;i<indice;i++)
		posicao += tamanhos[i] + 1;
		
	arquivoRegistros.seekg (posicao,ios::cur);
	string linhaRegistros;
	getline (arquivoRegistros,linhaRegistros);
	
	return linhaRegistros;
}

int main(){
	
	cout << ">Servidor rodando..." << endl;
	

	tamanhos.resize(0);
	tamanhos.push_back(260);
	
    string comando;
	int posicao;
	vector <string> campos = criaRegistro(pegaRegistro(-1));
	map <dado,int> arvore = leIndices();
	if(arvore.size() == 0){
		cout << ">Arquivos precisam ser inicializados. Inicializando..." << endl;
		inicializaArquivo();
		arvore = leIndices();
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
							vector <string> registros = criaRegistro(pegaRegistro(posicao));
							string json = "{";
							for(int i=0;i<registros.size();i++)
								json += ("{" + campos[i] + ": " + registros[i] + "},");
							json[json.size()-1] = '}';
							cout << "a: " << pegaRegistro(posicao) << endl;
							new_sock << "S" + json;
						} else
							new_sock << "RowIDNecessaria";
								
						break;
						
					case 'i': 	
					
						comando.erase(0,1);
						escreveRegistro(comando);						
						arvore = insereIndice(comando, arvore);
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
