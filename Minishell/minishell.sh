clear
# Voy a la carpeta que contiene los archivos fuentes de los comandos
cd sources
# Compilo los archivos de los diferentes comandos
gcc mkdir.c -o mkdir -Wall
gcc rmdir.c -o rmdir -Wall
gcc mkfile.c -o mkfile -Wall
gcc lsdir.c -o lsdir -Wall
gcc lsfile.c -o lsfile -Wall
gcc chmod.c -o chmod -Wall
gcc help.c -o help -Wall
cd ..
# Regreso a la carpeta que contiene la minishell
gcc minishell.c -o minishell -Wall
./minishell
