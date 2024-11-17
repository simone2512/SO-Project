Prima di eseguire il progetto controllare le impostazione del kernel di Linux per le dimensioni delle code di messaggi,
sono reimpostabili con il seguente comando:
sysctl kernel.msgmax=1000000
sysctl kernel.msgmnb=1000000