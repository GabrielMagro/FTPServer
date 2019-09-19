#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include <cstring>
#include <ostream>

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

    DataHora *getDataHora() {
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
    // {f}+{NomeAtributo}
    int fPort1=-1, fPort2=-1, fSize1=-1, fSize2=-1, fReplyCod=-1;
    string fDataHoraInicial="", fDataHoraFinal="", fIp="", fComando="", fMime="", fReplyMsg="";
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

    vector<Registro *> getLogs() {
        return this->logs;
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
    }

    //TODO FILTRO O ESQUEMA DO CIN
    void filtroData() {}

    void filtroIp(string ip) {
        this->fIp=ip;
        vector<Registro *> logsValidos = this->getLogsValidos();
        for (vector<Registro *>::iterator it = logsValidos.begin(); it != logsValidos.end(); ++it) {
            if ((*it)->getOrigemIp() != ip) {
                (*it)->setFiltro(false);
            }
        }
    }

    void filtroPortas(int port1, int port2) {
        this->fPort1=port1;
        this->fPort2=port2;
        vector<Registro *> logsValidos = this->getLogsValidos();
        for (vector<Registro *>::iterator it = logsValidos.begin(); it != logsValidos.end(); ++it) {
            if ((*it)->getOrigemPorta() < port1 || (*it)->getOrigemPorta() > port2) {
                (*it)->setFiltro(false);
            }
        }
    }

    void filtroComando(string comando) {
        this->fComando=comando;
        comando = this->toUpper(comando);
        vector<Registro *> logsValidos = this->getLogsValidos();
        for (vector<Registro *>::iterator it = logsValidos.begin(); it != logsValidos.end(); ++it) {
            if (this->toUpper((*it)->getComando()) != comando) {
                (*it)->setFiltro(false);
            }
        }
    }

    void filtroMimeType(string mime) {
        this->fMime=mime;
        mime = this->toUpper(mime);
        vector<Registro *> logsValidos = this->getLogsValidos();
        for (vector<Registro *>::iterator it = logsValidos.begin(); it != logsValidos.end(); ++it) {
            string str = this->toUpper((*it)->getMimeType());
            string str1 = mime;
            size_t found = str.find(str1);
            if (!(found != string::npos)) {
                (*it)->setFiltro(false);
            }
        }
    }

    void filtroTamanho(int size1, int size2) {
        this->fSize1=size1;
        this->fSize2=size2;
        vector<Registro *> logsValidos = this->getLogsValidos();
        for (vector<Registro *>::iterator it = logsValidos.begin(); it != logsValidos.end(); ++it) {
            if ((*it)->getFileSize() < size1 || (*it)->getFileSize() > size2) {
                (*it)->setFiltro(false);
            }
        }
    }

    void filtroReplyCode(int codigo) {
        this->fReplyCod=codigo;
        vector<Registro *> logsValidos = this->getLogsValidos();
        for (vector<Registro *>::iterator it = logsValidos.begin(); it != logsValidos.end(); ++it) {
            if ((*it)->getReplyCode() != codigo) {
                (*it)->setFiltro(false);
            }
        }
    }

    void filtroReplyMsg(string replyMsg) {
        this->fReplyMsg=replyMsg;
        replyMsg = this->toUpper(replyMsg);
        vector<Registro *> logsValidos = this->getLogsValidos();
        for (vector<Registro *>::iterator it = logsValidos.begin(); it != logsValidos.end(); ++it) {
            string str = this->toUpper((*it)->getMimeType());
            string str1 = replyMsg;
            size_t found = str.find(str1);
            if (!(found != string::npos)) {
                (*it)->setFiltro(false);
            }
        }
    }

    void mostraFiltrosAtivos() {

        cout << "Filtros: " << endl;
        if (this->fDataHoraInicial != "") {
            cout << "Data hora inicial: " << this->fDataHoraInicial << endl;
            cout << "Data hora final: " << this->fDataHoraFinal << endl;
        }
        if (this->fIp != "") {
            cout << "IP: " << this->fIp << endl;
        }
        if (this->fPort1 > 0) {
            cout << "Porta inicial: " << this->fPort1 << endl;
            cout << "Porta final: " << this->fPort2 << endl;
        }
        if (this->fComando != "") {
            cout << "Comando: " << this->fComando << endl;
        }
        if (this->fMime != "") {
            cout << "Mime Type: " << this->fMime << endl;
        }
        if (this->fSize1 > 0) {
            cout << "Tamanho inicial: " << this->fSize1 << endl;
            cout << "Tamanho final: " << this->fSize2 << endl;
        }
        if (this->fReplyCod > 0) {
            cout << "Reply code: " << this->fReplyCod << endl;
        }
        if (this->fReplyMsg != "") {
            cout << "Reply Message: " << this->fReplyMsg << endl;
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
        int port1=-1, port2=-1, size1=-1, size2=-1, replyCod=-1;
        string dataHoraInicial="", dataHoraFinal="", ip="", comando="", mime="", replyMsg="";
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
                //TODO TRANFORMAR PARA POSIX
//            cout << "Data Incial:";
//            cin >> dataHoraInicial;
//            cout << "Data Final";
//            cin >> dataHoraFinal;
                break;
            case 2:
                do {
                    cout << "Informe endereço IP." << endl;
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
                obj->filtroPortas(port1, port2);
                break;
            case 4:
                do {
                    cout << "Informe o comando" << endl;
                    cin >> comando;
                    if (comando == "") {
                        cout << "Valor invalido, informe novamente!" << endl;
                    }
                } while (comando == "");
                obj->filtroComando(comando);
                break;
            case 5:
                do {
                    cout << "Informe o MimeType" << endl;
                    cin >> mime;
                    if (mime == "") {
                        cout << "Valor invalido, informe novamente!" << endl;
                    }
                } while (mime == "");
                obj->filtroMimeType(mime);
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
                obj->filtroTamanho(size1, size2);
                break;
            case 7:
                do {
                    cout << "Informe codigo de resposta" << endl;
                    cin >> replyCod;
                    if (replyCod < 1) {
                        cout << "Valor invalido, informe novamente!" << endl;
                    }
                } while (replyCod < 1);
                obj->filtroReplyCode(replyCod);
                break;
            case 8:
                do {
                    cout << "Informe o mensagem de resposta" << endl;
                    cin >> replyMsg;
                    if (replyMsg == "") {
                        cout << "Valor invalido, informe novamente!" << endl;
                    }
                } while (replyMsg == "");
                obj->filtroReplyMsg(replyMsg);
                break;
            default:
                cout << "Opcao invalida" << endl;
                break;
        }
    }


};


void filtros(Sistema *obj) {
}

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
                filtros(obj);
                break;
            case 2:
                obj->limpaFiltros();
                break;
            case 3:
                obj->mostraFiltrosAtivos();
                break;
            case 4:

                break;
            case 5:
                break;

        }
    }

    return 0;
}