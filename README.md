# Compilador para a linguagem C-

O projeto em questão foi desenvolvido como um dos requisitos da disciplinas de Compiladores do curso de Ciência da Computação na UNIFESP.

### Módulos desenvolvidos para o compilador

- Scanner;
- Parser;
- Gerenciador da tabela de símbolos;
- Analisador semântico;
- Gerador de código intermediário.

### Execução

Para utilizar o compilador, basta clonar o repositório e executar os seguintes comandos:

```bash
git clone git@github.com:thiago-henrique-leite/compilador-cminus.git

cd compilador-cminus

# Para compilar o código
make

# Para utilizar o executável gerado
./compiler.x codes/mdc.c
```

Ao final, caso deseje remover os arquivos gerados após a compilação, execute:

```bash
make remove
```
