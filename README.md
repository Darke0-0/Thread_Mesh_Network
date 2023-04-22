# Thread-Network
Connect DHT11 at Port 0 pin 11
Press Button 1 on each NRF board Including receiver board.
So Temperature
Reading will be transmitted to central board
With 8 sec delay. 
In cli_sender main.c file at line 196 change IP address 
according to your receiver board.
Use below link to understand CLI commands. 

https://github.com/openthread/openthread/blob/main/src/cli/README.md
## ESP to NRF connection
connect pins as follows.
NRF Tx => ESP Rx
NRF Rx => ESP Tx
NRF CTs => ESP RTs
NRF RTs => ESP CTs
Pin number is mentioned in the ESPGoogleSheet.ino file
