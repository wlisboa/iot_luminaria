# iot_luminaria

	Esse projeto consiste em um iot que pode ser controlado por requisições http.

## Funcionamento

	A configuração do dispositivo é feita atravé de um página de configuração que pode ser acessada da seguinte
	forma: Quando o dispositivo não encontra um rede para se conectar ele cria um ponto de acesso chamado
	"luminária", esse ponto não possui senha. Com um celular ou um computador com Wifi o usuário deverá
	se conectar a essa rede e acessar, através de um browser, o endereço "http://192.168.4.1/conf". Aparecerá
	um página que possibilita a configuração dos parämetros do dispositivo.

## Hardware

- Espressif - ESP8266

## Dependências

- [NTPClient.git](https://github.com/arduino-libraries/NTPClient.git)

