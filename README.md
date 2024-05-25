# [Criptografia AES]
<p><b>Alunos:</b> Arthur B. Pinotti, Kaue Reblin, Luiz G. Klitzke.</p>
<p>Modo <b>ECB</b>, chave e blocos de <b>128</b> bits e preenchimento <b>PKCS#7</b>.</p>

<p>Implementação do algoritmo AES em ECB como trabalho final da matéria de Segurança da Informação - FURB

## Execução
Basta compilar a solution presente no repositório, ou executar o arquivo presente nos releases.

## Argumentos de entrada
Ao rodar a aplicação, serão requisitados alguns argumentos através do prompt de comando:

1. O caminho para o arquivo a ser criptografado.
2. O caminho desejado para o arquivo de saída.
3. A chave de criptografia (string com a representação decimal dos bytes, separada por vírgula)
4. Indicador para gerar log (Y para gerar).

<p>Um exemplo de entrada, que pode ser colado diretamente no cmd para informar todos os parâmetros:</p>

```
TextoPlanoExemplo.txt
ResultadoExemplo.AES
65, 66 , 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80
y
```
## Aviso de erros
Ao informar algum argumento inválido pelo terminal, o mesmo ira apresentar uma mensagem detalhando o motivo, e um aviso aguardando qualquer input para fechar a aplicação.
Obs: Caso o input seja colado de 1 vez, o terminal será fechado, uma vez que o segundo argumento será tido como um input.
