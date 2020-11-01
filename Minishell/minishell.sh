clear
# Voy a la carpeta que contiene los archivos fuentes de los comandos
cd sources
# Compilo los archivos de los diferentes comandos
gcc mkdir.c -o mkdir
gcc rmdir.c -o rmdir
cd ..
# Regreso a la carpeta que contiene la minishell
gcc minishell.c -o minishell
./minishell
