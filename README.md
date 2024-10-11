# Projeto de Redes de Computadores 1 - Simulação de redes com ns-3
Projeto experimental de redes de computadores realizado durante a disciplina de Redes de Computadores (2024.2), da Faculdade de Computação e Telecomunizações - Universidade Federal do Pará (UFPA)
A proposta do projeto é desenvolver uma topologia de rede simples utilizando o simulador ns-3 , com mínimo 10 nós, e estabelcer um tráfego no mesma. Este tráfego gerado será posteriormente analisado com ferramentas como Wireshark, tcpdump. Também é possível visualizar a simulação utilizando o NetAnim.

## Obtendo os arquivos
```
git clone git@github.com:dhominicx/projeto-redes-1-simulador.git
cd projeto-redes-1-simulador
cp redes.cc path/to/ns3/installation/scratch
```

## Download e Instalação do ns-3

### Download
https://www.nsnam.org/releases/latest
```
tar xjf ns-allinone-3.43.tar.bz2
cd ns-allinone-3.43/ns-3.43
```
## Build
```
./ns3 configure --enable-examples --enable-tests
./ns3 build
./test.py
```

## Instalação do NetAnim
```
cd ns-allinone-3.42/netanim-3.109
make clean
qmake NetAnim.pro
make
```

## Visualização da simulação com NetAnim
Mude para o diretório onde o executável do NetAnim está localizado e execute:
```
cd ns-allinone-3.42/netanim-3.109
./NetAnim
```
Depois basta selecionar o arquivo `.xml ` gerado durante a simulação e visualizar a animação com o botão play

## Análise do tráfego com Wireshark/tcpdump
todo
