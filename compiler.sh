echo "Compilando todos os arquivos."
gcc comutador.c -o comutador
gcc destino.c -o destino
gcc origem.c -o origem
echo "Compilação completa!"
echo "Para executar em ordem: ./comutador -> ./destino -> ./origem"