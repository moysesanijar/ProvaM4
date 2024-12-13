Os estados do sistema são:

1. Leitura do LDR (sensor de luminosidade)
O valor lido pelo LDR determina se o sistema está em modo noturno (escuro) ou modo diurno (claro).
Escuro: Modo noturno ativo.
Claro: Modo convencional (diurno) ativo.

2. Modo Noturno (Escuro)
Quando está escuro (valor do LDR abaixo ou igual ao limiar definido):
Estado Noturno: Somente o LED amarelo pisca (acende por 1 segundo, apaga por 1 segundo, e assim sucessivamente).
Todos os demais LEDs (verde, vermelho) permanecem apagados.
O botão, mesmo pressionado, não altera o comportamento neste modo, pois o semáforo não segue o ciclo convencional.

3. Modo Convencional (Claro)
Quando está claro (valor do LDR acima do limiar):
O sistema entra no modo convencional, simulando um semáforo normal.
Ciclo do semaforo no Modo Convencional:
Verde: LED verde acende por 3 segundos.
Amarelo: Apos o verde, o LED verde apaga e o LED amarelo acende por 2 segundos.
Vermelho: Apos o amarelo, o LED amarelo apaga e o LED vermelho acende por 5 segundos.

Ao final do vermelho, o ciclo recomeça (verde → amarelo → vermelho) enquanto estiver claro.

4. Interações com o Botão no Modo Convencional Durante o Vermelho
Quando o semáforo está no estado vermelho (fechado) durante o modo claro, o botão pode alterar o comportamento:
Primeira Pressão do Botão durante o Vermelho:
Ao pressionar o botão uma vez durante o vermelho, o sistema inicia uma contagem de 1 segundo.
Após 1 segundo do primeiro pressionamento, o semáforo “abre”, mudando para o verde e permanecendo assim por 3 segundos, interrompendo o ciclo normal que manteria o vermelho por 5 segundos.
Três Pressões do Botão durante o Vermelho:
Se, enquanto o semáforo estiver vermelho no modo claro, o botão for pressionado 3 vezes (contagem acumulativa durante o mesmo período de vermelho), o sistema envia uma requisição HTTP (para “http://www.google.com.br”) como forma de alerta.

Resumindo são esses estados:
Modo Noturno (Escuro): LED amarelo piscando continuamente, sem interferência do botão.
Modo Convencional (Claro):
Verde (3s)
Amarelo (2s)
Vermelho (5s)

Durante o vermelho:
Pressão do botão 1x: após 1s, abre (verde)
Pressão do botão 3x: envia alerta HTTP

