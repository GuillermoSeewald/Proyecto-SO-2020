while(1) {
    // Chequeo si hay alguien reponiendo la heladera
    // Se busca en la cola un mensaje (message) de tipo TYPE_MUTEX_HELADERA_CERRADA
    // Si no hay ningún mensaje de ese tipo, significa que hay alguien reponiendo la heladera
    if (msgrcv(queueId, &msgRcv, SIZE_MSG, TYPE_HELADERA_CERRADA, IPC_NOWAIT)!=-1) {

        // Ahora que abri la heladera, veo si hay botellas de leche
        if (msgrcv(queueId, &msgRcv, SIZE_MSG, TYPE_HELADERA_LLENA, IPC_NOWAIT)!=-1) {
            printf("Soy el compañero %d, tomando una botella de leche\n", num);
            // Cierro la heladera
            msgsnd(queueId, &msgTypeHeladeraCerrada, SIZE_MSG, IPC_NOWAIT);
            sleep(1);

        } else {
            // Cierro la heladera
            msgsnd(queueId, &msgTypeHeladeraCerrada, SIZE_MSG, IPC_NOWAIT);

            // Como no había más botellas, veo si ya fue alguien al supermercado
            printf("Soy el compañero %d, no hay mas leche, ¿ya fue alguien a comprar?\n", num);

            // Control de si ya fue alguien, buscando un mensaje de tipo TYPE_MUTEX_COMPRANDO
            if (msgrcv(queueId, &msgRcv, SIZE_MSG, TYPE_NADIE_COMPRANDO, IPC_NOWAIT)!=-1) {
                // Si entro en este caso, significa que no fue nadie aún al supermercado
                // El compañero va al supermercado
                printf("Soy el compañero %d, aún no fue nadie, yendo al supermercado\n", num);
                printf("Soy el compañero %d, comprando botellas\n", num);
                sleep(2);

                // Busco un mensaje de tipo TYPE_MUTEX_HELADERA_CERRADA para ver tener
                // exclusividad al abrir la heladera
                msgrcv(queueId, &msgRcv, SIZE_MSG, TYPE_HELADERA_CERRADA, 0);


                for (i=0; i<CANTIDAD_BOTELLAS; i++) {
                    // Una vez que se tiene exclusividad en la heladera, guarda todas
                    // las botellas compradas, enviando mensajes de tipo TYPE_HELADERA_LLENA
                    msgsnd(queueId, &msgTypeHeladeraLlena, SIZE_MSG, IPC_NOWAIT);
                    sleep(1);
                    printf("Soy el compañero %d, reponiendo botella nº %d\n", num, (i+1));
                }

                // Al terminar de reponer, cierro la heladera
                printf("Soy el compañero %d, ya repuse la heladera\n", num);
                msgsnd(queueId, &msgTypeHeladeraCerrada, SIZE_MSG, IPC_NOWAIT);
                sleep(2);

                // Indico que ya se volvió del supermercado con un mensaje
                // de tipo TYPE_MUTEX_COMPRANDO
                msgsnd(queueId, &msgTypeNadieComprando, SIZE_MSG, IPC_NOWAIT);
            } else {
                // Si ya fue alguien al supermercado, hago otras cosas
                printf("Soy el compañero %d, ya hay alguien que fue a comprar\n", num);
                sleep(2);
            }
        }
    } else {
        // Si hay alguien usando la heladera, hago otras cosas
        printf("Soy el compañero %d, la heladera está ocupada, me voy a hacer otras cosas\n", num);
        sleep(3);
    }
}
