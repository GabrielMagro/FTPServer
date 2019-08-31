#include <iostream>
#include <ctime>
#include <fstream>
#include <string>

using namespace std;

class Registro {
private:
    DataHora dataHora;
    string origemIP;
    int origemPorta;
    string comando;
    string mimeType;
    int fileSize;
    int replyCode;
    string replyMsg;
public:
    Registro(string linha){

    }
};

class DataHora {
private:
    time_t timestamp;
public:
    DataHora(string timestamp);
    string getDataHora();
};

class Sistema{
private:
    vector<Registro*> logs;
public:
    Sistema(string arqLog){
        fstream arq = arqLog;
        arq.open(arq.c_str, fstream::in){
            if(arq.is_open()){
                string linha;
                getline(arq, title);
                if(linha != "timestamp\tip\tport\tcommand\tmime_type\tfile_size\treply_code\treply_msg"){
                    while(!arq.eof()){
                        //atribuição das variaveis
                    }
                }
            }
        }
    }
};

int main()
{
    int opcao;
    while(1){
        system("cls");
        cout << "1 - Adicionar filtro" << endl;
        cout << "2 - Limpar filtros" << endl;
        cout << "3 - Visualizar filtros" << endl;
        cout << "4 - Visualizar dados" << endl;
        cout << "5 - Exportar dados" << endl;

        cin >> opcao;
        system("cls");
        switch (opcao) {
        case 1:
        break;

        case 2:
        break;

        case 3:
        break;

        case 4:
        break;

        case 5:
        break;

        }
    }



    return 0;
}
