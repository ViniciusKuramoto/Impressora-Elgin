# 🖨️ Controle da Impressora Térmica Elgin i8 / i9 em C
### *(Coloque aqui o nome do seu projeto)*
---

## 📘 Sobre o Projeto

Este projeto foi desenvolvido por alunos com o objetivo de criar um programa em **linguagem C** capaz de controlar uma impressora térmica **Elgin i8 ou i9**, usando a biblioteca oficial da Elgin (`E1_Impressora01.dll`).

O programa pede **login**, mostra um **menu** em laço e permite configurar e abrir a conexão com a impressora, imprimir texto, QR Code, código de barras e cupons XML SAT, abrir a gaveta de dinheiro e emitir sinal sonoro. Todo o código fica no arquivo `main.c`.

O trabalho faz parte da disciplina de **Programação em C**.

---

## 🧰 Tecnologias Utilizadas

- **Linguagem C**
- **Windows** (usa `windows.h`: `LoadLibraryA` e `GetProcAddress`)
- **Compilador GCC (MinGW)** via Code::Blocks ou terminal
- **Biblioteca Elgin** `E1_Impressora01.dll` (versões x86 e x64)
- **Git / GitHub** para versionamento

---

## 📁 Estrutura do Projeto

```
main.c                  → Código-fonte do programa (único arquivo editado)
E1_Impressora01.dll     → Biblioteca da Elgin (x86 ou x64, veja abaixo)
XMLSAT.xml              → XML de teste para o cupom SAT (dados fictícios)
CANC_SAT.xml            → XML de teste para o cancelamento SAT (dados fictícios)
README.md               → Este arquivo
```

> ⚠️ O arquivo `.exe` gerado **não** deve ser enviado ao repositório.

---

## ⚙️ Instalação e Execução

### 🔽 1. Baixar o Projeto do GitHub
1. Acesse o repositório.
2. Clique no botão **Code**.
3. Selecione **Download ZIP**.
4. Aguarde o download.

### 📦 2. Extrair o Arquivo ZIP
1. Vá até o local onde o arquivo foi salvo.
2. Clique com o botão direito → **Extrair tudo**.
3. Será criada uma nova pasta com os arquivos.

### 🧩 3. Escolher a DLL certa (32 ou 64 bits)
A DLL precisa ter a **mesma arquitetura do compilador**. Para descobrir a do seu, rode no terminal:

```
gcc -dumpmachine
```

| Resultado | Seu compilador é | Use a DLL da pasta |
|---|---|---|
| `x86_64-w64-mingw32` | 64 bits | **x64** |
| `mingw32` ou `i686-w64-mingw32` | 32 bits (o MinGW do Code::Blocks costuma ser assim) | **x86** |

Copie a `E1_Impressora01.dll` certa, o `XMLSAT.xml` e o `CANC_SAT.xml` para a **raiz do projeto** (a mesma pasta do `main.c`).

### 🧭 4A. Compilar e rodar pelo Code::Blocks
1. Abra o **Code::Blocks**.
2. **File → New → Project → Console application → C**.
3. Substitua o conteúdo do `main.c` do projeto pelo código deste repositório.
4. Copie a DLL e os XMLs para a pasta do projeto (a mesma do arquivo `.cbp`).
5. Em **Project → Properties → Build targets**, confira se o campo **Execution working dir** é a pasta do projeto.
6. Ligue a impressora, conecte o cabo **USB** e aperte **F9** (Build and run).

### 💻 4B. Compilar e rodar pelo terminal
Entre na pasta do projeto e execute:

```
gcc main.c -o impressora.exe
impressora.exe
```

---

## 🔐 Login de Teste

| Campo | Valor |
|---|---|
| Usuário | `admin` |
| Senha | `1234` |

São permitidas no máximo **3 tentativas**. O programa avisa quantas tentativas restam e é encerrado se todas forem erradas.

---

## 📋 Opções do Menu

| Nº | Opção | O que faz |
|---|---|---|
| 1 | Configurar conexão | Define tipo, modelo, porta e parâmetro |
| 2 | Abrir conexão | Abre a comunicação com a impressora |
| 3 | Fechar conexão | Fecha a comunicação com a impressora |
| 4 | Imprimir texto | Imprime um texto na quantidade de cópias escolhida |
| 5 | Imprimir QR Code | Imprime o QR Code do conteúdo digitado |
| 6 | Imprimir código de barras | Imprime um código de barras CODE 128 |
| 7 | Imprimir XML SAT | Imprime o cupom a partir do `XMLSAT.xml` |
| 8 | Imprimir XML SAT Cancelamento | Imprime o cancelamento a partir do `CANC_SAT.xml` |
| 9 | Abrir gaveta Elgin | Abre a gaveta de dinheiro (comando `AbreGavetaElgin`) |
| 10 | Abrir gaveta | Abre a gaveta de dinheiro (comando `AbreGaveta`) |
| 11 | Emitir sinal sonoro | Faz a impressora apitar (de 1 a 63 vezes) |
| 12 | Fechar programa | Fecha a conexão e encerra o programa |
| 0 | Sair | Fecha a conexão e sai do menu |

> As opções de impressão e gaveta só funcionam com a **conexão aberta**. Se estiver fechada, o programa avisa e não faz nenhuma pergunta.

**Valores para os testes de conexão (opção 1):** tipo = `1` (USB), modelo = `i9` (ou `i8`), porta = `USB`, parâmetro = `0`.

---

## 🧪 Testes

- Login com usuário e senha corretos e incorretos (3 tentativas)
- Abertura e fechamento da conexão com a impressora
- Tentativa de imprimir com a conexão fechada (deve ser bloqueada)
- Impressão de texto com várias cópias
- Impressão de QR Code e código de barras
- Impressão dos XMLs SAT e de cancelamento
- Abertura da gaveta pelos dois comandos
- Sinal sonoro
- Digitação de letras onde se espera número (o programa não quebra)

---

## 🛠️ Se der erro

| O que apareceu | O que provavelmente é |
|---|---|
| `Nao foi possivel carregar E1_Impressora01.dll (erro 126)` | A DLL não está na pasta de onde o programa é executado |
| `... (erro 193)` | DLL com bits diferentes do compilador |
| `Comando '...' nao encontrado na DLL` | DLL errada ou de outra versão |
| `Erro ao abrir conexao`, código `-9999` ou negativo | Impressora desligada, cabo solto, ou modelo/porta errados |
| `Nao foi possivel ler o XML` | O arquivo XML não está na pasta de execução |

---

## 📚 Documentação Adicional

- Documentação oficial da Elgin: https://elgindevelopercommunity.github.io/group___m1.html
- Manual da impressora **Elgin i8 / i9**

---

## 👥 Autores
*(em ordem alfabética)*

- Nome do Integrante 1
- Nome do Integrante 2
- Nome do Integrante 3
