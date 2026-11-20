#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

/*
 * Controle da impressora Elgin (i8 / i9) em C, usando a DLL E1_Impressora01.dll.
 *
 * A DLL precisa estar na RAIZ do projeto (na mesma pasta de onde o programa e executado),
 * junto com os arquivos XMLSAT.xml e CANC_SAT.xml.
 *
 * IMPORTANTE: a DLL precisa ter a mesma arquitetura do compilador:
 *   - compilador de 32 bits (ex.: MinGW do Code::Blocks)  -> use a DLL da pasta x86
 *   - compilador de 64 bits (ex.: MSYS2 mingw64)          -> use a DLL da pasta x64
 */

/* ===================== Ligacao com a DLL (equivalente a interface ImpressoraDLL do Java) ===================== */

/* Cada typedef descreve a "forma" de um comando da DLL: o que devolve e quais parametros recebe */
typedef int (*FnAbreConexaoImpressora)(int tipo, const char *modelo, const char *conexao, int param);
typedef int (*FnFechaConexaoImpressora)(void);
typedef int (*FnImpressaoTexto)(const char *dados, int posicao, int estilo, int tamanho);
typedef int (*FnCorte)(int avanco);
typedef int (*FnImpressaoQRCode)(const char *dados, int tamanho, int nivelCorrecao);
typedef int (*FnImpressaoCodigoBarras)(int tipo, const char *dados, int altura, int largura, int HRI);
typedef int (*FnAvancaPapel)(int linhas);
typedef int (*FnStatusImpressora)(int param);
typedef int (*FnAbreGavetaElgin)(int pino, int ti, int tf);
typedef int (*FnAbreGaveta)(int pino, int ti, int tf);
typedef int (*FnSinalSonoro)(int qtd, int tempoInicio, int tempoFim);
typedef int (*FnModoPagina)(void);
typedef int (*FnLimpaBufferModoPagina)(void);
typedef int (*FnImprimeModoPagina)(void);
typedef int (*FnModoPadrao)(void);
typedef int (*FnPosicaoImpressaoHorizontal)(int posicao);
typedef int (*FnPosicaoImpressaoVertical)(int posicao);
typedef int (*FnImprimeXMLSAT)(const char *dados, int param);
typedef int (*FnImprimeXMLCancelamentoSAT)(const char *dados, const char *assQRCode, int param);

/* Um ponteiro para cada comando. Depois de carregarDLL(), e so chamar: AbreConexaoImpressora(...) */
static FnAbreConexaoImpressora AbreConexaoImpressora;
static FnFechaConexaoImpressora FechaConexaoImpressora;
static FnImpressaoTexto ImpressaoTexto;
static FnCorte Corte;
static FnImpressaoQRCode ImpressaoQRCode;
static FnImpressaoCodigoBarras ImpressaoCodigoBarras;
static FnAvancaPapel AvancaPapel;
static FnStatusImpressora StatusImpressora;
static FnAbreGavetaElgin AbreGavetaElgin;
static FnAbreGaveta AbreGaveta;
static FnSinalSonoro SinalSonoro;
static FnModoPagina ModoPagina;
static FnLimpaBufferModoPagina LimpaBufferModoPagina;
static FnImprimeModoPagina ImprimeModoPagina;
static FnModoPadrao ModoPadrao;
static FnPosicaoImpressaoHorizontal PosicaoImpressaoHorizontal;
static FnPosicaoImpressaoVertical PosicaoImpressaoVertical;
static FnImprimeXMLSAT ImprimeXMLSAT;
static FnImprimeXMLCancelamentoSAT ImprimeXMLCancelamentoSAT;

static HMODULE dll = NULL;

/* Procura um comando dentro da DLL. Devolve 1 se achou, 0 se nao achou. */
static int carregaFuncao(void *destino, const char *nome) {
    FARPROC f = GetProcAddress(dll, nome);
    if (f == NULL) {
        printf("Comando '%s' nao encontrado na DLL.\n", nome);
        return 0;
    }
    *(FARPROC *)destino = f;
    return 1;
}

/* Carrega a DLL da raiz do projeto e todos os comandos. Devolve 1 se deu certo. */
static int carregarDLL(void) {
    dll = LoadLibraryA("E1_Impressora01.dll");
    if (dll == NULL) {
        printf("Nao foi possivel carregar E1_Impressora01.dll (erro %lu).\n", GetLastError());
        printf("Confira: a DLL esta na raiz do projeto? Ela tem a mesma arquitetura (32/64 bits) do compilador?\n");
        return 0;
    }

    int ok = 1;
    ok &= carregaFuncao(&AbreConexaoImpressora, "AbreConexaoImpressora");
    ok &= carregaFuncao(&FechaConexaoImpressora, "FechaConexaoImpressora");
    ok &= carregaFuncao(&ImpressaoTexto, "ImpressaoTexto");
    ok &= carregaFuncao(&Corte, "Corte");
    ok &= carregaFuncao(&ImpressaoQRCode, "ImpressaoQRCode");
    ok &= carregaFuncao(&ImpressaoCodigoBarras, "ImpressaoCodigoBarras");
    ok &= carregaFuncao(&AvancaPapel, "AvancaPapel");
    ok &= carregaFuncao(&StatusImpressora, "StatusImpressora");
    ok &= carregaFuncao(&AbreGavetaElgin, "AbreGavetaElgin");
    ok &= carregaFuncao(&AbreGaveta, "AbreGaveta");
    ok &= carregaFuncao(&SinalSonoro, "SinalSonoro");
    ok &= carregaFuncao(&ModoPagina, "ModoPagina");
    ok &= carregaFuncao(&LimpaBufferModoPagina, "LimpaBufferModoPagina");
    ok &= carregaFuncao(&ImprimeModoPagina, "ImprimeModoPagina");
    ok &= carregaFuncao(&ModoPadrao, "ModoPadrao");
    ok &= carregaFuncao(&PosicaoImpressaoHorizontal, "PosicaoImpressaoHorizontal");
    ok &= carregaFuncao(&PosicaoImpressaoVertical, "PosicaoImpressaoVertical");
    ok &= carregaFuncao(&ImprimeXMLSAT, "ImprimeXMLSAT");
    ok &= carregaFuncao(&ImprimeXMLCancelamentoSAT, "ImprimeXMLCancelamentoSAT");
    return ok;
}

/* ===================== Variaveis do programa ===================== */

static int conexaoAberta = 0;       /* 0 = fechada, 1 = aberta */
static int tipo = 1;                /* 1 = USB, 2 = Serial, 3 = Rede... */
static char modelo[50] = "i9";      /* "i8" ou "i9" */
static char conexao[50] = "USB";
static int parametro = 0;

/* Chave do QR Code usada no cancelamento SAT */
static const char *ASS_QRCODE =
    "Q5DLkpdRijIRGY6YSSNsTWK1TztHL1vD0V1Jc4spo/CEUqICEb9SFy82ym8EhBRZjbh3btsZhF+sjHqEMR159i4agru9x6KsepK/"
    "q0E2e5xlU5cv3m1woYfgHyOkWDNcSdMsS6bBh2Bpq6s89yJ9Q6qh/J8YHi306ce9Tqb/drKvN2XdE5noRSS32TAWuaQEVd7u+"
    "TrvXlOQsE3fHR1D5f1saUwQLPSdIv01NF6Ny7jZwjCwv1uNDgGZONJdlTJ6p0ccqnZvuE70aHOI09elpjEO6Cd+orI7XHHrFCwhFhAcbalc+"
    "ZfO5b/+vkyAHS6CYVFCDtYR9Hi5qgdk31v23w==";

/* ===================== Funcoes auxiliares ===================== */

/* Mostra a mensagem e le uma linha de texto digitada (sem o ENTER no final) */
static void capturarEntrada(const char *mensagem, char *destino, int tamanho) {
    printf("%s", mensagem);
    if (fgets(destino, tamanho, stdin) == NULL) {
        destino[0] = '\0';
        return;
    }
    destino[strcspn(destino, "\r\n")] = '\0';
}

/* Mostra a mensagem e le um numero inteiro. Se digitar letras, pergunta de novo. */
static int lerInteiro(const char *mensagem) {
    char linha[50];
    char *fim;
    while (1) {
        capturarEntrada(mensagem, linha, sizeof(linha));
        long valor = strtol(linha, &fim, 10);
        if (linha[0] != '\0' && *fim == '\0') {
            return (int)valor;
        }
        printf("Digite apenas numeros!\n");
    }
}

/* Le um arquivo inteiro para a memoria. Devolve NULL se nao achar. Quem chama deve dar free(). */
static char *lerArquivo(const char *caminho) {
    FILE *arq = fopen(caminho, "rb");
    if (arq == NULL) {
        return NULL;
    }
    fseek(arq, 0, SEEK_END);
    long tamanho = ftell(arq);
    rewind(arq);

    char *conteudo = (char *)malloc(tamanho + 1);
    if (conteudo == NULL) {
        fclose(arq);
        return NULL;
    }
    size_t lidos = fread(conteudo, 1, tamanho, arq);
    conteudo[lidos] = '\0';
    fclose(arq);
    return conteudo;
}

/* Corta o papel (Corte(5)) e confere o retorno do comando. */
static void cortarPapel(void) {
    int retorno = Corte(5);
    if (retorno != 0) {
        printf("Erro ao cortar o papel. Codigo: %d\n", retorno);
    }
}

/* ===================== Funcoes da impressora ===================== */

/* Pede usuario e senha (3 tentativas). Devolve 1 se acertar e 0 se errar as 3 vezes. */
int fazerLogin(void){

	int tentativa;
	char login[50], senha[50];

		printf("\nELGIN\n");

	for(tentativa = 1; tentativa <= 3; tentativa++){

		capturarEntrada("\nDigite seu usuario: ", login, sizeof(login));
		capturarEntrada("Digite sua senha: ", senha, sizeof(senha));

		if(strcmp(login, "admin") == 0 && strcmp(senha, "1234") == 0){
			return 1;
		}
		else if(tentativa == 3){
			printf("\nLimite de tentativas atingido!\n");
		}
		else{
			printf("\nUsuario ou senha errada! %d Tentativas Restantes\n", 3 - tentativa);
		}

	}
	return 0;
}

void exibirMenu(void) {
    printf("\nMENU ELGIN\n\n");
    printf("1  - Configurar Conexao\n");
	printf("2  - Abrir Conexao\n");
	printf("3  - Fechar Conexao\n");
	printf("4  - Imprimir Texto\n");
	printf("5  - Imprimir Qrcode\n");
	printf("6  - Imprimir Codigo de Barras\n");
	printf("7  - Imprimir XMLSAT\n");
	printf("8  - Imprimir XMLSAT Cancelamento\n");
	printf("9  - Abrir Gaveta Elgin\n");
	printf("10 - Abrir Gaveta \n");
	printf("11 - Emitir Sinal Sonoro\n");
	printf("12 - Fechar Programa\n");
	printf("0  - Sair\n");
}

void configurarConexao(void) {
    if (!conexaoAberta) {
        tipo = lerInteiro("Digite o tipo de conexao (1 para USB, 2 para Serial, etc.): ");
        capturarEntrada("Digite o modelo da impressora: ", modelo, sizeof(modelo));
        capturarEntrada("Digite a porta de conexao (ex: USB): ", conexao, sizeof(conexao));
        parametro = lerInteiro("Digite o parametro adicional (ex: 0 para padrao): ");
        printf("Parametros de conexao configurados com sucesso.\n");
    } else {
        printf("Conexao ja configurada. Pronta para uso.\n");
    }
}

void abrirConexao(void) {
    if (!conexaoAberta) {
        int retorno = AbreConexaoImpressora(tipo, modelo, conexao, parametro);
        if (retorno == 0) {
            conexaoAberta = 1;
            printf("Conexao aberta com sucesso.\n");
        } else {
            printf("Erro ao abrir conexao. Codigo de erro: %d\n", retorno);
        }
    } else {
        printf("Conexao ja esta aberta.\n");
    }
}

void fecharConexao(void) {
    /* so fecha se estiver aberta */
    if (!conexaoAberta) {
        printf("Conexao ja esta fechada.\n");
        return;
    }

    int retorno = FechaConexaoImpressora();
    if (retorno == 0) {
        conexaoAberta = 0;
        printf("Conexao fechada com sucesso.\n");
    } else {
        printf("Erro ao fechar conexao. Codigo de erro: %d\n", retorno);
    }
}

void imprimirTexto(void) {
    char texto[200];
    int copias, i, retorno;

    /* 1) conexao aberta? Se nao, avisa e sai sem perguntar nada */
    if (!conexaoAberta) {
        printf("Conexao fechada. Abra a conexao antes (opcao 2).\n");
        return;
    }

    /* 2) pergunta o que precisa */
    capturarEntrada("Texto a imprimir: ", texto, sizeof(texto));
    copias = lerInteiro("Quantas copias? ");

    /* 3) imprime cada copia e 4) confere o retorno (ImpressaoTexto: erro = negativo) */
    for (i = 1; i <= copias; i++) {
        retorno = ImpressaoTexto(texto, 1, 4, 0);
        if (retorno >= 0) {
            printf("Copia %d impressa!\n", i);
        } else {
            printf("Erro ao imprimir. Codigo: %d\n", retorno);
        }
    }

    /* 5) corta o papel */
    cortarPapel();
}

void imprimirQRCode(void) {
    if (!conexaoAberta) {
        printf("Conexao fechada. Abra a conexao antes (opcao 2).\n");
        return;
    }

    char dados[200];
    capturarEntrada("Conteudo do QR Code: ", dados, sizeof(dados));

    int retorno = ImpressaoQRCode(dados, 6, 4);
    cortarPapel();

    if (retorno == 0) {
        printf("QR Code impresso!\n");
    } else {
        printf("Erro ao imprimir QR Code. Codigo: %d\n", retorno);
    }
}

void imprimirCodigoBarras(void) {
    if (!conexaoAberta) {
        printf("Conexao fechada. Abra a conexao antes (opcao 2).\n");
        return;
    }

    /* tipo 8 = CODE 128 (o texto comeca com {A), altura 100, largura 2, HRI 3 = numero acima e abaixo */
    int retorno = ImpressaoCodigoBarras(8, "{A012345678912", 100, 2, 3);
    cortarPapel();

    if (retorno == 0) {
        printf("Codigo de barras impresso!\n");
    } else {
        printf("Erro ao imprimir codigo de barras. Codigo: %d\n", retorno);
    }
}

void imprimirXMLSAT(void) {
    if (!conexaoAberta) {
        printf("Conexao fechada. Abra a conexao antes (opcao 2).\n");
        return;
    }

    /* le o arquivo XML (na raiz do projeto) para a memoria */
    char *dadosXML = lerArquivo("XMLSAT.xml");
    if (dadosXML == NULL) {
        printf("Nao foi possivel ler o XMLSAT.xml\n");
        return;
    }

    int retorno = ImprimeXMLSAT(dadosXML, 0);
    cortarPapel();
    free(dadosXML); /* devolve a memoria */

    if (retorno == 0) {
        printf("XML SAT impresso!\n");
    } else {
        printf("Erro ao imprimir XML SAT. Codigo: %d\n", retorno);
    }
}

void imprimirXMLCancelamentoSAT(void) {
    if (!conexaoAberta) {
        printf("Conexao fechada. Abra a conexao antes (opcao 2).\n");
        return;
    }

    /* le o arquivo XML (na raiz do projeto) para a memoria */
    char *dadosXML = lerArquivo("CANC_SAT.xml");
    if (dadosXML == NULL) {
        printf("Nao foi possivel ler o CANC_SAT.xml\n");
        return;
    }

    int retorno = ImprimeXMLCancelamentoSAT(dadosXML, ASS_QRCODE, 0);
    free(dadosXML); /* devolve a memoria */

    if (retorno == 0) {
        printf("XML SAT de cancelamento impresso!\n");
    } else {
        printf("Erro ao imprimir XML SAT de cancelamento. Codigo: %d\n", retorno);
    }
}

void abrirGavetaElgin(void) {
    if (!conexaoAberta) {
        printf("Conexao fechada. Abra a conexao antes (opcao 2).\n");
        return;
    }

    int retorno = AbreGavetaElgin(1, 50, 50);
    if (retorno == 0) {
        printf("Gaveta (Elgin) aberta!\n");
    } else {
        printf("Erro ao abrir a gaveta (Elgin). Codigo: %d\n", retorno);
    }
}

void abrirGaveta(void) {
    if (!conexaoAberta) {
        printf("Conexao fechada. Abra a conexao antes (opcao 2).\n");
        return;
    }

    int retorno = AbreGaveta(1, 5, 10);
    if (retorno == 0) {
        printf("Gaveta aberta!\n");
    } else {
        printf("Erro ao abrir a gaveta. Codigo: %d\n", retorno);
    }
}

void emitirSinalSonoro(void) {
    if (!conexaoAberta) {
        printf("Conexao fechada. Abra a conexao antes (opcao 2).\n");
        return;
    }

    /* a DLL aceita de 1 a 63 apitos */
    int qtd = lerInteiro("Quantos apitos (1 a 63)? ");
    if (qtd < 1 || qtd > 63) {
        printf("Quantidade invalida. Use um valor entre 1 e 63.\n");
        return;
    }

    int retorno = SinalSonoro(qtd, 50, 5);
    if (retorno == 0) {
        printf("Sinal sonoro emitido!\n");
    } else {
        printf("Erro ao emitir sinal sonoro. Codigo: %d\n", retorno);
    }
}

void fecharPrograma(void){
	printf("\nFechando o Programa...\n\n");
	if (conexaoAberta) {
	    fecharConexao();
	}
	FreeLibrary(dll);
	exit(0);
}

/* ===================== Programa principal ===================== */

int main(void) {

	/* sem a DLL nenhum comando funciona, entao encerra */
	if (!carregarDLL()) {
	    printf("Programa encerrado.\n");
	    return 1;
	}

	if (!fazerLogin()) {
	    printf("Acesso negado.\n");
	    FreeLibrary(dll);
	    return 1;
	}

    int opcao = -1;
    while (opcao != 0) {
        exibirMenu();
        opcao = lerInteiro("\nDigite a opcao desejada: ");
        printf("\n");

	        switch(opcao){

	        	case 0:
	        	  fecharConexao();
	        	break;

	        	case 1:
	        	  configurarConexao();
	        	break;

	            case 2:
	        	  abrirConexao();
				break;

	        	case 3:
	        	  fecharConexao();
				break;

	        	case 4:
	        	  imprimirTexto();
				break;

	        	case 5:
	              imprimirQRCode();
				break;

	        	case 6:
	        	  imprimirCodigoBarras();
				break;

	        	case 7:
	        	  imprimirXMLSAT();
				break;

	        	case 8:
	        	  imprimirXMLCancelamentoSAT();
				break;

	        	case 9:
	        	  abrirGavetaElgin();
				break;

	        	case 10:
	        	  abrirGaveta();
				break;

	        	case 11:
	        	  emitirSinalSonoro();
				break;

	        	case 12:
	        	  fecharPrograma();
				break;

	        	default:
	        	  printf("Opcao invalida.\n");
	        	break;
		}
    }

    FreeLibrary(dll);

    return 0;
}
