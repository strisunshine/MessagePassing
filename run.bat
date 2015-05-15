@echo off
echo.
echo ***************************** Demonstrating requirements ********************************
echo.  
echo  CommandLine Argument for Server1: 8090  
echo.  
echo  CommandLine Argument for Server2: 6070
echo.
echo  CommandLine Argument for Client1: "client1toServer1.txt" "CIME.pdf" "client1toServer2.txt" "python.exe" 9080 8090 6070
echo.
echo  CommandLine Argument for Client2: "client2toServer1.txt" "MT4S03sol.doc" "science.txt" "kola.jpg" "client2toServer2.txt 7060 8090 6070
echo.
start Debug\Server1.exe 8090
start Debug\Server2.exe 6070
start Debug\Client1.exe "client1toServer1.txt" "CIME.pdf" "client1toServer2.txt" "python.exe" 9080 8090 6070
start Debug\Client2.exe "client2toServer1.txt" "MT4S03sol.doc" "science.txt" "kola.jpg" "client2toServer2.txt" 7060 8090 6070
pause