####Projetos de OpenCV do LabProdam

Repositório que armazena códigos de processamento de imagem e visão computacional. 

_./_ -- Projetos do Lab; 

_proc/_ -- Object-files, scripts de código comum a dois ou mais projetos;

_imgs/_ -- Imagens usadas pelos projetos;

_Tracking/_ -- Projeto do Gervásio de trackear a trajetória de objetos (?).

Se a cena for muito caótica ou a iluminação for fraca/inconstante o desempenho dos programas não vai ser muito bom.

-----------------------
#####Bibliotecas necessárias
- gcc; g++;
- libopencv-dev.

-----------------------
**OBS::** Se você não estiver conseguindo dar make mesmo com todas as bibliotecas instaladas e atualizadas, uma das (ou ambas) coisas abaixo pode ajudar:

1. No cógido que está dando erro, procure se há alguma linha com "#define vOpenCV x", e troque o valor de x para 1-x;
2. No Makefile, tente remover das LIBS (linha 5) a string "-isystem/usr/include".
