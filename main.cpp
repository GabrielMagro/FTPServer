#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include <cstring>
#include <ostream>
#include <locale>
#include <iomanip>

using namespace std;


class DataHora {
private:
    time_t timestamp;
public:
    DataHora(string timestamp) {
        istringstream issTimestamp(timestamp);
        issTimestamp >> this->timestamp;
    }

    string getDataHora() {
        char buff[20];
        time_t now = this->timestamp;
        strftime(buff, 20, "%Y-%m-%dT%H:%M:%S", localtime(&now));
        return buff;
    }

    time_t getDataHoraTimeT() {
        return this->timestamp;
    }


    bool operator==(const DataHora &dados) const {
        return this->timestamp == dados.timestamp;
    }

    bool operator>(const DataHora &dados) const {
        return this->timestamp > dados.timestamp;
    }

    bool operator<(const DataHora &dados) const {
        return this->timestamp < dados.timestamp;
    }

    bool operator>=(const DataHora &dados) const {
        return this->timestamp >= dados.timestamp;
    }

    bool operator<=(const DataHora &dados) const {
        return this->timestamp <= dados.timestamp;
    }
};

class Registro {
private:
    DataHora *dataHora;
    string origemIP;
    int origemPorta;
    string comando;
    string mimeType;
    int fileSize;
    int replyCode;
    string replyMsg;
    bool filtro;
public:
    Registro(string linha) {
        string dataHoraStr;
        string origemPortaStr;
        string fileSizeStr;
        string replyCodeStr;
        istringstream issLinha(linha);

        getline(issLinha, dataHoraStr, '\t');
        this->dataHora = new DataHora(dataHoraStr);

        getline(issLinha, origemIP, '\t');

        getline(issLinha, origemPortaStr, '\t');
        this->origemPorta = atoi(origemPortaStr.c_str());

        getline(issLinha, comando, '\t');

        getline(issLinha, mimeType, '\t');
        this->mimeType = validaVazio(this->mimeType);

        getline(issLinha, fileSizeStr, '\t');
        fileSizeStr = validaVazio(fileSizeStr);
        this->fileSize = atoi(fileSizeStr.c_str());

        getline(issLinha, replyCodeStr, '\t');
        replyCodeStr = validaVazio(replyCodeStr);
        this->replyCode = atoi(replyCodeStr.c_str());

        getline(issLinha, replyMsg, '\t');
        this->replyMsg = validaVazio(this->replyMsg);

        this->filtro = true;
    }

    string validaVazio(string atributo) {
        if (atributo == "-") {
            atributo = "";
        }
        return atributo;
    }

    DataHora *getDataHoraSistema() {
        return dataHora;
    }

    string getOrigemIp() {
        return this->origemIP;
    }

    int getOrigemPorta() {
        return this->origemPorta;
    }

    string getComando() {
        return this->comando;
    }

    string getMimeType() {
        return this->mimeType;
    }

    int getFileSize() {
        return this->fileSize;
    }

    int getReplyCode() {
        return this->replyCode;
    }

    string getReplyMsg() {
        return this->replyMsg;
    }

    void setFiltro(bool status) {
        this->filtro = status;
    }

    bool getFiltro() {
        return this->filtro;
    }

};

class Sistema {
private:
    vector<Registro *> logs;
    //filtros
    int port1 = -1, port2 = -1, size1 = -1, size2 = -1, replyCod = -1;
    string dataHoraInicial = "", dataHoraFinal = "", ip = "", comando = "", mime = "", replyMsg = "";

public:
    Sistema(string arqLog) {
        fstream arq;
        arq.open(arqLog.c_str(), fstream::in);
        string linha, textTop;
        if (arq.is_open()) {
            getline(arq, textTop);
            if (textTop != "timestamp\tip\tport\tcommand\tmime_type\tfile_size\treply_code\treply_msg");
            while (!arq.eof()) {
                getline(arq, linha, '\n');
                Registro *registro = new Registro(linha);
                logs.push_back(registro);
            }
        } else {
            cout << "Erro ao abrir" << endl;
        }
    }

    vector<Registro *> getLogsValidos() {
        vector<Registro *> logsValidos;
        for (vector<Registro *>::iterator it = this->logs.begin(); it != this->logs.end(); ++it) {
            if ((*it)->getFiltro()) {
                logsValidos.push_back(*it);
            }
        }
        return logsValidos;
    }

    void limpaFiltros() {
        for (vector<Registro *>::iterator it = this->logs.begin(); it != this->logs.end(); ++it) {
            (*it)->setFiltro(true);
        }
        //valores padrao
        this->port1 = -1;
        this->port2 = -1;
        this->size1 = -1;
        this->size2 = -1;
        this->replyCod = -1;
        this->dataHoraInicial = "";
        this->dataHoraFinal = "";
        this->ip = "";
        this->comando = "";
        this->mime = "";
        this->replyMsg = "";
    }

    void filtroData(string data1, string data2) {
        struct tm date1 = {};
        struct tm date2 = {};

        stringstream issData1(data1);
        stringstream issData2(data2);

        bool valido = true;

        time_t dt1;
        time_t dt2;

        if (issData1 >> get_time(&date1, "%Y-%m-%dT%H:%M:%S")) {
            dt1 = mktime(&date1);
        } else {
            valido = false;
        }
        if (issData2 >> get_time(&date2, "%Y-%m-%dT%H:%M:%S")) {
            dt2 = mktime(&date2);
        } else {
            valido = false;
        }

        if (!valido) {
            cout << "Datas invalidas" << endl;
            this->dataHoraInicial = "";
            this->dataHoraFinal = "";
        } else if (dt1 == dt2) {
            cout << "Datas iguais, filtro nao aplicado" << endl;
            this->dataHoraInicial = "";
            this->dataHoraFinal = "";
        } else if (dt1 > dt2) {
            this->dataHoraInicial = "";
            this->dataHoraFinal = "";
            cout << "Data Inicial nao pode ser maior que data final, filtro nao aplicado" << endl;
        } else {
            vector<Registro *> logsValidos = this->getLogsValidos();
            int dataFiltro1 = dt1;
            int dataFiltro2 = dt2;
            for (vector<Registro *>::iterator it = logsValidos.begin(); it != logsValidos.end(); ++it) {
                int dataRegistro = (*it)->getDataHoraSistema()->getDataHoraTimeT();
                if (dataRegistro < dt1 || dataRegistro > dt2) {
                    (*it)->setFiltro(false);
                }
            }
        }
    }

    void filtroIp(string ip) {
        vector<Registro *> logsValidos = this->getLogsValidos();
        for (vector<Registro *>::iterator it = logsValidos.begin(); it != logsValidos.end(); ++it) {
            if ((*it)->getOrigemIp() != ip) {
                (*it)->setFiltro(false);
            }
        }
    }

    void filtroPortas(int port1, int port2) {
        vector<Registro *> logsValidos = this->getLogsValidos();
        for (vector<Registro *>::iterator it = logsValidos.begin(); it != logsValidos.end(); ++it) {
            if ((*it)->getOrigemPorta() < port1 || (*it)->getOrigemPorta() > port2) {
                (*it)->setFiltro(false);
            }
        }
    }

    void filtroComando(string comando) {
        comando = this->toUpper(comando);
        vector<Registro *> logsValidos = this->getLogsValidos();
        for (vector<Registro *>::iterator it = logsValidos.begin(); it != logsValidos.end(); ++it) {
            if (this->toUpper((*it)->getComando()) != comando) {
                (*it)->setFiltro(false);
            }
        }
    }

    void filtroMimeType(string mime) {
        mime = this->toUpper(mime);
        vector<Registro *> logsValidos = this->getLogsValidos();
        for (vector<Registro *>::iterator it = logsValidos.begin(); it != logsValidos.end(); ++it) {
            string str = this->toUpper((*it)->getMimeType());
            size_t found = str.find(mime);
            if (!(found != string::npos)) {
                (*it)->setFiltro(false);
            }
        }
    }

    void filtroTamanho(int size1, int size2) {
        vector<Registro *> logsValidos = this->getLogsValidos();
        for (vector<Registro *>::iterator it = logsValidos.begin(); it != logsValidos.end(); ++it) {
            if ((*it)->getFileSize() < size1 || (*it)->getFileSize() > size2) {
                (*it)->setFiltro(false);
            }
        }
    }

    void filtroReplyCode(int codigo) {
        vector<Registro *> logsValidos = this->getLogsValidos();
        for (vector<Registro *>::iterator it = logsValidos.begin(); it != logsValidos.end(); ++it) {
            if ((*it)->getReplyCode() != codigo) {
                (*it)->setFiltro(false);
            }
        }
    }

    void filtroReplyMsg(string replyMsg) {
        replyMsg = this->toUpper(replyMsg);
        vector<Registro *> logsValidos = this->getLogsValidos();
        for (vector<Registro *>::iterator it = logsValidos.begin(); it != logsValidos.end(); ++it) {
            string str = this->toUpper((*it)->getReplyMsg());
            size_t found = str.find(replyMsg);
            if (!(found != string::npos)) {
                (*it)->setFiltro(false);
            }
        }
    }

    void mostraFiltrosAtivos() {

        if (this->dataHoraInicial == "" && this->ip == "" && this->port1 < 1 && this->comando == "" &&
            this->mime == "" && this->size1 < 1 && this->replyCod < 1 && this->replyMsg == "") {
            cout << "Sem filtros aplicados" << endl;
        } else {
            if (this->dataHoraInicial != "") {
                cout << "Data hora inicial: " << this->dataHoraInicial << endl;
                cout << "Data hora final: " << this->dataHoraFinal << endl;
            }
            if (this->ip != "") {
                cout << "IP: " << this->ip << endl;
            }
            if (this->port1 > 0) {
                cout << "Porta inicial: " << this->port1 << endl;
                cout << "Porta final: " << this->port2 << endl;
            }
            if (this->comando != "") {
                cout << "Comando: " << this->comando << endl;
            }
            if (this->mime != "") {
                cout << "Mime Type: " << this->mime << endl;
            }
            if (this->size1 > 0) {
                cout << "Tamanho inicial: " << this->size1 << endl;
                cout << "Tamanho final: " << this->size2 << endl;
            }
            if (this->replyCod > 0) {
                cout << "Reply code: " << this->replyCod << endl;
            }
            if (this->replyMsg != "") {
                cout << "Reply Message: " << this->replyMsg << endl;
            }
        }
    }

    string toUpper(string param) {
        string up = "";
        for (int i = 0; i < param.size(); i++) {
            up += toupper(param[i]);
        }
        return up;
    }

    void aplicaFiltros() {

        int submenu;
        system("cls");
        cout << "1 - Data Hora" << endl;
        cout << "2 - IP" << endl;
        cout << "3 - Porta" << endl;
        cout << "4 - Comando" << endl;
        cout << "5 - MimeType" << endl;
        cout << "6 - FileSize" << endl;
        cout << "7 - ReplyCode" << endl;
        cout << "8 - ReplyMessage" << endl;
        cin >> submenu;
        switch (submenu) {
            case 1:
                cout << "Informe data inicial " << endl;
                cin >> dataHoraInicial;
                cout << "Informe data final " << endl;
                cin >> dataHoraFinal;
                this->filtroData(dataHoraInicial, dataHoraFinal);
                break;
            case 2:
                do {
                    cout << "Informe endereco IP: " << endl;
                    cin >> ip;
                    if (ip == "") {
                        cout << "Valor invalido, insira novamente" << endl;
                    }
                } while (ip == "");
                this->filtroIp(ip);
                break;
            case 3:
                do {
                    cout << "Informe a porta inicial" << endl;
                    cin >> port1;
                    cout << "Informe a porta final" << endl;
                    cin >> port2;
                    if (port1 < 1 || port2 < 1 || port1 > port2) {
                        cout << "Valores invalidos, informe novamente!" << endl;
                    }
                } while (port1 < 1 && port2 < 1 && port1 > port2);
                this->filtroPortas(port1, port2);
                break;
            case 4:
                do {
                    cout << "Informe o comando" << endl;
                    cin >> comando;
                    if (comando == "") {
                        cout << "Valor invalido, informe novamente!" << endl;
                    }
                } while (comando == "");
                this->filtroComando(comando);
                break;
            case 5:
                do {
                    cout << "Informe o MimeType" << endl;
                    cin >> mime;
                    if (mime == "") {
                        cout << "Valor invalido, informe novamente!" << endl;
                    }
                } while (mime == "");
                this->filtroMimeType(mime);
                break;
            case 6:
                do {
                    cout << "Informe o tamanho inicial" << endl;
                    cin >> size1;
                    cout << "Informe o tamanho final" << endl;
                    cin >> size2;
                    if (size1 < 1 || size2 < 1 || size1 > size2) {
                        cout << "Valores invalidos, informe novamente!" << endl;
                    }
                } while (size1 < 1 && size2 < 1 && size1 > size2);
                this->filtroTamanho(size1, size2);
                break;
            case 7:
                do {
                    cout << "Informe codigo de resposta" << endl;
                    cin >> replyCod;
                    if (replyCod < 1) {
                        cout << "Valor invalido, informe novamente!" << endl;
                    }
                } while (replyCod < 1);
                this->filtroReplyCode(replyCod);
                break;
            case 8:
                do {
                    cout << "Informe o mensagem de resposta" << endl;
                    getline(cin, replyMsg);
                    if (replyMsg == "") {
                        cout << "Valor invalido, informe novamente!" << endl;
                    }
                } while (replyMsg == "");
                this->filtroReplyMsg(replyMsg);
                break;
            default:
                cout << "Opcao invalida" << endl;
                break;
        }
    }

    void visualizarDados() {
        vector<Registro *> logsValidos = this->getLogsValidos();

        if (logsValidos.size() == 0) {
            cout << "Nenhum log valido perante os filtros" << endl;
        } else {
            for (vector<Registro *>::iterator it = logsValidos.begin(); it != logsValidos.end(); ++it) {
                cout << "Data hora:             " << (*it)->getDataHoraSistema()->getDataHora() << endl;
                cout << "IP de Origem:          " << (*it)->getOrigemIp() << endl;
                cout << "Porta de Origem:       " << (*it)->getOrigemPorta() << endl;
                cout << "Comando:               " << (*it)->getComando() << endl;
                cout << "Mime Type:             " << (*it)->getMimeType() << endl;
                if ((*it)->getFileSize() == 0) {
                    cout << "Tamanho do Arquivo:    " << endl;
                } else {
                    cout << "Tamanho do Arquivo:    " << (*it)->getFileSize() << endl;
                }
                if ((*it)->getReplyCode() == 0) {
                    cout << "Codigo de Resposta:    " << endl;
                } else {
                    cout << "Codigo de Resposta:    " << (*it)->getReplyCode() << endl;
                }
                cout << "Mensagem de Resposta:  " << (*it)->getReplyMsg() << endl;
                cout << endl;
            }
        }
    }

    void exportar() {
        string arquivoDeSerializacao;
        fstream arq;
        cout << "Insira o nome do arquivo onde deseja salvar a pesquisa filtrada: ";
        cin >> arquivoDeSerializacao;
        arquivoDeSerializacao += ".txt";

        arq.open(arquivoDeSerializacao.c_str(), fstream::out);

        vector<Registro *> logsValidos = this->getLogsValidos();

        if (arq.is_open()) {
            cout << "Serializando..." << endl;
            arq << "timestamp\tip\tport\tcommand\tmime_type\tfile_size\treply_code\treply_msg\n";
            for (vector<Registro *>::iterator it = logsValidos.begin(); it != logsValidos.end(); ++it) {
                arq << (*it)->getDataHoraSistema()->getDataHora() << '\t'
                    << (*it)->getOrigemIp() << '\t'
                    << (*it)->getOrigemPorta() << '\t'
                    << (*it)->getComando() << '\t';

                if ((*it)->getMimeType() == "") {
                    arq << "-" << '\t';
                } else {
                    arq << (*it)->getMimeType() << '\t';
                }
                if ((*it)->getFileSize() == 0) {
                    arq << "-" << '\t';
                } else {
                    arq << (*it)->getFileSize() << '\t';
                }
                if ((*it)->getReplyCode() == 0) {
                    arq << "-" << '\t';
                } else {
                    arq << (*it)->getReplyCode() << '\t';
                }
                if ((*it)->getReplyMsg() == "") {
                    arq << "-" << '\t';
                } else {
                    arq << (*it)->getReplyMsg() << endl;
                }
            }
            arq.close();
        }
        cout << "--------------------------" << endl;
        cout << "Serializacao bem sucedida!" << endl;
        cout << "--------------------------" << endl;
    }
};


int main() {
    int opcao;
    string nomeArq = "ftp.log";
    Sistema *obj = new Sistema(nomeArq);

    while (1) {
        system("cls");
        cout << "1 - Adicionar filtro" << endl;
        cout << "2 - Limpar filtros" << endl;
        cout << "3 - Visualizar filtros" << endl;
        cout << "4 - Visualizar dados" << endl;
        cout << "5 - Exportar dados" << endl;

        cin >> opcao;

        switch (opcao) {
            case 1:
                obj->aplicaFiltros();
                break;
            case 2:
                obj->limpaFiltros();
                break;
            case 3:
                obj->mostraFiltrosAtivos();
                break;
            case 4:
                obj->visualizarDados();
                break;
            case 5:
                obj->exportar();
                break;
            default:
                cout << "Opcao invalida"<<endl;
                break;
        }
    }

    return 0;
}