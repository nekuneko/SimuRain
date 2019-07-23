* Instructions (Work in progress...)

The system currently works following the flow:

* The program is loaded into the ROM of the card and gets activated when receives electric power and the serial port is opened.

* When running, the program writes every 2 second the data received from the different sensors. So running the Arduino IDE showing the serial port is recommended. This information is needed firstly to calibrate the instruments of the experiment.

* Software records day-time, Ph and temperature every two seconds.

* The output file is "datalog.txt", but it can be modified in the source code. It includes Windows CRLF, in UTF-8 format.

* Every time new data is recorded in the microSD card, the modification date of the file in the operating system is modified.

* New data is appended at the end of the file. If user needs data separated into different files, (s)he has to do it manually.

Troubleshooting:

* In case of problems when writing in the card, formatting the SD card with the "SD Card Formatter" software is recommended: https://www.sdcard.org/downloads/formatter_4/index.html

* In case the time is not synchronized (or too early/delayed) it can be re-adjusted: disconnect electric power, take the batteries off using a screwdriver and, after 10 seconds, connect it again and load the program anew. The system, when the serial port will be opened, will detect that there is no actual time and will use the compilation time of the program as current time.


* Instrucciones provisionales (Work in progress...)

El programa actualmente funciona de la siguiente manera:

* El programa se carga en la ROM de la tarjeta y se activa cuando recibe alimentación y se abre el puerto serial. En el puerto serial se van mostrando las lecturas y otra información, por lo que hace falta tener e Arduino IDE y mostrar el puerto serial. Esta información puede servir para la calibración del experimento.

* El software registra dia-hora, Ph y temperatura (no caudal), espera hasta que terminen de pasar 2 segundos y vuelve a hacerlo.

* El nombre del fichero de salida es "datalog.txt", su puede cambiar al inicio del programa. El fichero tiene final de línea Windows CRLF, codificado en UTF-8.

* Cuando se escribe en la tarjeta microSD, se guarda la fecha de modificación de la última vez que fue escrita.

* El contenido se añade al final del fichero. Si se desean separar los datos hay que hacerlo a mano desde el ordenador.

En caso de problemas:

* Si hay problemas de escritura se debe probar a formatear la tarjeta microSD con el software "SD Card Formatter": https://www.sdcard.org/downloads/formatter_4/index.html

* Si se pierde la hora (o el reloj atrasa/adelanta más de lo deseado) se puede ajustar la hora. Para hacerlo se desconecta todo de la alimentación eléctrica, se saca la pila con un destornillador, se esperan 10 segundos, se vuelve a conectar y se vuelve a subir el programa. El sistema, cuando se abra el puerto serial, detectará que no tiene horay cogerá la hora de la última compilación del programa como hora actual para desde ahí empezará a contar.

