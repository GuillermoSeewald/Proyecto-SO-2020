clear
# Voy a la carpeta que contiene los archivos fuentes de los comandos
cd sources
# Compilo los archivos de los diferentes comandos
gcc mkdir.c -o mkdir
gcc rmdir.c -o rmdir
gcc mkfile.c -o mkfile
gcc lsdir.c -o lsdir
gcc lsfile.c -o lsfile
gcc chmod.c -o chmod
gcc help.c -o help
cd ..
# Regreso a la carpeta que contiene la minishell
gcc minishell.c -o minishell
./minishell
