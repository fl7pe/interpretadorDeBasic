#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <string>
#include <algorithm>

class InterpretadorDeBasic {
public:
    
    void executar(const std::string& caminhoArquivo) {
        carregarCodigo(caminhoArquivo); // Carrega o programa BASIC do arquivo
        rodarCodigo(); 
    }

private:
    // Mapa para armazenar as linhas do código BASIC, onde a chave é o número da linha
    std::map<int, std::string> linhasCodigo;
    // Mapa para armazenar as variáveis e seus valores
    std::map<std::string, int> variaveis;
    int linhaAtual = 10;


    void carregarCodigo(const std::string& caminhoArquivo) {
    // Abre o arquivo especificado pelo caminho
    std::ifstream arquivo(caminhoArquivo);

    if (!arquivo.is_open()) {
        std::cerr << "Erro ao abrir o arquivo!" << std::endl;
        return;
    }

    std::string linha;
    int numeroLinha;

    // Lê o arquivo linha por linha
    while (std::getline(arquivo, linha)) {
        std::cout << linha << std::endl; // Exibe a linha lida

        // Converte a linha para um stream de string 
        std::istringstream iss(linha);

        // extrai o número da linha
        if (!(iss >> numeroLinha)) {
            std::cerr << "Erro de sintaxe na linha: " << linha << std::endl;
            continue; // Pula para a próxima linha em caso de erro
        }

        // Encontra a posição do primeiro espaço na linha
        size_t posEspaco = iss.str().find(' ');

        // Verifica se há um espaço na linha (indicando separação entre número da linha e comando)
        if (posEspaco == std::string::npos) {
            std::cerr << "Erro de sintaxe na linha: " << linha << std::endl;
            continue; 
        }

        // Armazena o comando associado ao número da linha no mapa linhasCodigo
        linhasCodigo[numeroLinha] = linha.substr(posEspaco + 1);
    }
}


   void rodarCodigo() {
    
    while (linhasCodigo.find(linhaAtual) != linhasCodigo.end()) {
        // Obtém o comando da linha atual
        std::string comando = linhasCodigo[linhaAtual];
        
        // Processa o comando obtido
        processarComando(comando);
        
        
        if (linhaAtual != -1) {
            linhaAtual = obterProximaLinha(); 
        }
    }
}
    
int obterProximaLinha() {
    // Encontra o próximo elemento no mapa 'linhasCodigo' que é maior que 'linhaAtual'
    auto it = linhasCodigo.upper_bound(linhaAtual);
    
    // Se encontrou um elemento válido, retorna a chave (número da linha)
    if (it != linhasCodigo.end()) {
        return it->first;
    }
    
    // Se não encontrou, retorna -1 indicando que não há mais linhas a serem executadas
    return -1; 
}


void processarComando(const std::string& comando) {
    std::string cmd = comando; 
    size_t pos;

    
    while ((pos = cmd.find(';')) != std::string::npos) {
        // Extrai o comando até o próximo ';'
        std::string comandoUnico = cmd.substr(0, pos);
        // Remove espaços em branco do comando
        comandoUnico = removerEspacos(comandoUnico);  
        executarComandoUnico(comandoUnico);
        // Remove o comando processado da string original
        cmd.erase(0, pos + 1);
    }

    // Remove espaços em branco do último comando (ou do comando inteiro se não houver ';')
    cmd = removerEspacos(cmd);
    // Executa o último comando
    executarComandoUnico(cmd);
}


void executarComandoUnico(const std::string& comando) {
    // Ignora comentários (comandos que começam com "REM")
    if (comando.substr(0, 3) == "REM") return; 

    // Trata comandos de atribuição (que começam com "LET")
    if (comando.substr(0, 3) == "LET") {
        tratarAtribuicao(comando);
    }
    // Trata comandos de entrada (que começam com "INPUT")
    else if (comando.substr(0, 5) == "INPUT") {
        tratarEntrada(comando);
    }
    // Trata comandos de impressão (que começam com "PRINT")
    else if (comando.substr(0, 5) == "PRINT") {
        tratarImpressao(comando);
    }
    // Trata comando de parada (que começam com "HALT")
    else if (comando.substr(0, 4) == "HALT") {
        std::cout << "Execução encerrada." << std::endl;
        exit(0); 
    }
    // Trata comandos condicionais (que começam com "IF")
    else if (comando.substr(0, 2) == "IF") {
        tratarCondicional(comando);
    }
    // Se o comando não for reconhecido, exibe uma mensagem de erro
    else {
        std::cerr << "Comando desconhecido: " << comando << std::endl;
    }
}

void tratarImpressao(const std::string& comando) {
    // Extrai a parte do comando a ser impressa, removendo espaços em branco
    std::string aImprimir = comando.substr(5);
    aImprimir = removerEspacos(aImprimir);

    // Verifica se a parte a ser impressa é uma string entre aspas
    size_t posAspas = aImprimir.find('"');
    if (posAspas != std::string::npos) {
        // Encontra a posição da segunda aspa
        size_t posFimAspas = aImprimir.find('"', posAspas + 1);
        if (posFimAspas != std::string::npos) {
            // Extrai a string entre aspas e a imprime
            std::string strAImprimir = aImprimir.substr(posAspas + 1, posFimAspas - posAspas - 1);
            std::cout << strAImprimir << std::endl;
        } else {
            // Se a string não estiver terminada corretamente, exibe um erro de sintaxe e encerra o programa
            std::cerr << "Erro de sintaxe: string não terminada corretamente." << std::endl;
            exit(1);
        }
    } else {
        // Se não for uma string, trata como nome de variável
        std::string nomeVariavel = aImprimir;
        nomeVariavel.erase(std::remove_if(nomeVariavel.begin(), nomeVariavel.end(), ::isspace), nomeVariavel.end());
        
        // Verifica se a variável existe e imprime seu valor
        if (variaveis.find(nomeVariavel) != variaveis.end()) {
            std::cout << variaveis[nomeVariavel] << std::endl;
        } else {
            // Se a variável não for encontrada, exibe um erro e encerra o programa
            std::cerr << "Variável não encontrada: " << nomeVariavel << std::endl;
            exit(1);
        }
    }
}
    
void tratarAtribuicao(const std::string& comando) {
    // Encontra a posição do sinal de igual '=' no comando
    size_t posIgual = comando.find('=');
    
    // Se não encontrar o sinal, exibe um erro de sintaxe e encerra o programa
    if (posIgual == std::string::npos) {
        std::cerr << "Erro de sintaxe no comando LET: " << comando << std::endl;
        exit(1);
    }
    
    // Extrai o nome da variável do comando, removendo espaços em branco
    std::string nomeVariavel = comando.substr(4, posIgual - 4);
    nomeVariavel.erase(std::remove_if(nomeVariavel.begin(), nomeVariavel.end(), ::isspace), nomeVariavel.end());

    // Extrai a expressão a ser atribuída à variável, removendo espaços em branco
    std::string expressao = comando.substr(posIgual + 1);
    expressao = removerEspacos(expressao);
    
    variaveis[nomeVariavel] = avaliarExpressao(expressao);
}


void tratarEntrada(const std::string& comando) {
    // Extrai o nome da variável do comando, removendo espaços em branco
    std::string nomeVariavel = comando.substr(6);
    nomeVariavel = removerEspacos(nomeVariavel);
    nomeVariavel.erase(std::remove_if(nomeVariavel.begin(), nomeVariavel.end(), ::isspace), nomeVariavel.end());
    
    // Se o nome da variável estiver vazio, exibe um erro de sintaxe e encerra o programa
    if (nomeVariavel.empty()) {
        std::cerr << "Erro de sintaxe no comando INPUT: " << comando << std::endl;
        exit(1);
    }
    
    // Solicita ao usuário que digite um valor para a variável
    int valor;
    std::cout << "Digite um valor para " << nomeVariavel << ": ";
    std::cin >> valor;
    
    // Armazena o valor digitado na variável correspondente
    variaveis[nomeVariavel] = valor;
}

void tratarCondicional(const std::string& comando) {
    // Encontra a posição dos dois pontos ':' no comando
    size_t posDoisPontos = comando.find(':');
    
    // Se não encontrar os dois pontos, exibe um erro de sintaxe e encerra o programa
    if (posDoisPontos == std::string::npos) {
        std::cerr << "Erro de sintaxe: Falta ':' no comando IF." << std::endl;
        exit(1);
    }
    
    // Extrai a condição e os comandos a serem executados após a condição
    std::string condicao = comando.substr(2, posDoisPontos - 2);
    std::string comandosAposCondicional = comando.substr(posDoisPontos + 1); 

    // Remove espaços em branco da condição e dos comandos
    condicao = removerEspacos(condicao);
    comandosAposCondicional = removerEspacos(comandosAposCondicional);

    // Avalia a condição e, se verdadeira, processa os comandos subsequentes
    if (avaliarCondicao(condicao)) {
        processarComando(comandosAposCondicional); 
    }
}

std::string removerEspacos(const std::string& s) {
    size_t inicio = s.find_first_not_of(" \t"); // Encontra o primeiro caractere não branco
    size_t fim = s.find_last_not_of(" \t"); // Encontra o último não branco
    if (inicio == std::string::npos) return ""; // Se a string for vazia ou só tiver espaços, retorna uma string vazia
    return s.substr(inicio, fim - inicio + 1); // Retorna a substring sem espaços em branco
}

   
bool avaliarCondicao(const std::string& condicao) {
    std::istringstream iss(condicao); // Cria um stream de entrada a partir da condição
    int valorEsquerda = obterValor(iss); // Obtém o valor à esquerda do operador
    std::string operador;
    iss >> operador; // Lê o operador da condição
    int valorDireita = obterValor(iss); // Obtém o valor à direita do operador

    // Compara os valores com base no operador e retorna o resultado da comparação
    if (operador == "=") return valorEsquerda == valorDireita;
    else if (operador == "<") return valorEsquerda < valorDireita;
    else if (operador == ">") return valorEsquerda > valorDireita;
    else if (operador == "<=") return valorEsquerda <= valorDireita;
    else if (operador == ">=") return valorEsquerda >= valorDireita;
    else if (operador == "<>") return valorEsquerda != valorDireita;

    std::cerr << "Operador desconhecido na condição: " << operador << std::endl;
    exit(1);
}


int avaliarExpressao(const std::string& expressao) {
    std::istringstream iss(expressao); // Cria um stream de entrada a partir da expressão
    int resultado = obterValor(iss); // Obtém o primeiro valor da expressão

    char operador;
    
    while (iss >> operador) {
        if (iss.peek() == ' ') iss.ignore(); // Ignora espaços em branco
        int proximoValor = obterValor(iss); // Obtém o próximo valor da expressão

        // Realiza a operação aritmética correspondente
        if (operador == '+') resultado += proximoValor;
        else if (operador == '-') resultado -= proximoValor;
        else if (operador == '*') resultado *= proximoValor;
        else if (operador == '/') {
            if (proximoValor != 0) resultado /= proximoValor;
            else {
                std::cerr << "Erro: Divisão por zero." << std::endl;
                exit(1);
            }
        }
        else {
            std::cerr << "Operador desconhecido na expressão: " << operador << std::endl;
            exit(1);
        }
    }
    return resultado;
}


int obterValor(std::istringstream& iss) {
    std::string token;
    iss >> token; // Lê o próximo token do stream

    // Se o token for uma variável
    if (isalpha(token[0])) {
        token.erase(std::remove_if(token.begin(), token.end(), ::isspace), token.end()); // Remove espaços em branco
        if (variaveis.find(token) != variaveis.end()) {
            return variaveis[token]; 
        }
        else {
            std::cerr << "Variável não encontrada: " << token << std::endl;
            exit(1);
        }
    }
    else {
        // Se o token for um número literal, converte para inteiro e retorna
        try {
            return std::stoi(token); 
        }
        catch (std::invalid_argument&) {
            std::cerr << "Erro de sintaxe na expressão: " << token << std::endl;
            exit(1);
        }
    }
}


