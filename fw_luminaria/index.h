const char CONF_page[] PROGMEM = R"=====(
	<!DOCTYPE html>
	<html lang="pt-br">
		<head>
			<title>IOT_CONFIG</title>
			<meta charset="uft=8">
			<meta name="viewport" content="width=device-width, initial-scale=1.0">
		</head>
		<body>
			<header>
				<h1> Lumin&aacute;ria </h1>
			</header>
		<main>
			<p>Configura&ccedil;&atilde;o enviada com sucesso!</p>
			<form method="POST" action="/">
					<input class="sub1" type="submit" value="Voltar">
			</form>
		</main>
			<footer>
				<p>Criado por: Washington Lisboa</p>
				<a href="https://github.com/wlisboa/iot_luminaria">fonte do codigo</a>
			</footer>
		</body>
		<style>
			body {
				text-align: center;
			}
			header {
				padding-top: 5px;
				border-bottom: 2px solid #000;
				background-color: #38C2B3;
			}
			main {
				border-bottom: 2px solid #000;
			}
		</style>
	</html>
)=====";


const char CTRL_page[] PROGMEM = R"=====(
	<!DOCTYPE html>
	<html lang="pt-br">
		<head>
			<title>IOT_CONFIG</title>
			<meta charset="uft=8">
			<meta name="viewport" content="width=device-width, initial-scale=1.0">
		</head>
		<body onload="load()">
		<header>
			<h1> Lumin&aacute;ria </h1>
		</header>
		<main>
		<form class="form-conf" method="POST" action="/conf">
				<div>
					<h3>Configura&ccedil;&atilde;o do despertador</h3>
					<select id="desp" name="desp">
						<option value="luz">Luz</option>
						<option value="som">Som</option>
					</select>
					<label>Hora:</label>
					<input type="time" id="time" name="time">
				</div>
				<div>
					<h3>Configura&ccedil;&atilde;o da rede</h3>

					<label>Nome da rede:</label>
					<div class="data">
						<input type="text" id="rede" name="rede">
					</div>

					<label>Senha:</label>
					<div class="data">
						<input type="password" id="pass" name="pass">
					</div>

					<label>IP:</label>
					<div class="data">
						<input id="ip" name="ip" required pattern="^([0-9]{1,3}\.){3}[0-9]{1,3}$">
					</div>

					<label>Subnet Mask:</label>
					<div class="data">
						<input id="subn" name="subn" required pattern="^([0-9]{1,3}\.){3}[0-9]{1,3}$">
					</div>

					<label>Gateway:</label>
					<div class="data">
						<input id="gatw" name="gatw" required pattern="^([0-9]{1,3}\.){3}[0-9]{1,3}$">
					</div>
					<div class="d-sub1">
						<input class="sub1" type="submit" value="Enviar">
					</div>
				</div>
			</form>
		</main>
		<footer>
			<p>Criado por: Washington Lisboa</p>
			<a href="https://github.com/wlisboa/iot_luminaria">fonte do codigo</a>
		</footer>
		</body>
		<script>
			function load() {
				var xhttp = new XMLHttpRequest();
				xhttp.onreadystatechange = function()
				{
					if (xhttp.readyState == 4 && xhttp.status == 200)
					{
						var data = xhttp.responseText;
						var data_s = data.split(" ");
						document.getElementById("desp").value = data_s[0];
						document.getElementById("time").value = data_s[1];
						document.getElementById("rede").value = data_s[2];
						document.getElementById("pass").value = data_s[3];
						document.getElementById("ip").value = data_s[4];
						document.getElementById("subn").value = data_s[5];
						document.getElementById("gatw").value = data_s[6];
					}
				};
				xhttp.open("POST", "/load", true);
				xhttp.send();
			}
		</script>
		<style>
		body {
			text-align: center;
		}
		header {
			padding-top: 2px;
			background-color: #38C2B3;
			border-bottom: 2px solid #666;
		}
		main {
			border-bottom: 2px solid #666;
		}
		.data{
			padding-bottom: 5px;
		}
		.data input{
			font-size: 16px;
		}
		.form-conf .d-sub1{
			padding-top: 20px;
			padding-bottom: 20px;
		}
		.form-conf .d-sub1 .sub1{
			font-size: 25px;
			width: 150px;
		}
		</style>
	</html>
)=====";


const char INDEX_page[] PROGMEM = R"=====(
	<!DOCTYPE html>
	<html lang="pt-br">
				<head>
		<title>Luminaria</title>
		<meta charset="uft=8">
		<meta name="viewport" content="width=device-width, initial-scale=1.0">
	</head>
	<body>
		<header>
			<h1> Lumin&aacute;ria </h1>
		</header>
		<main>
			<div>
				<p>Demonstra&ccedil;&atilde;o IOT</p>
			</div>
			<div class="sens-area">
				<button type=button onclick="sensor()">Ler Hora</button>
				<p id="sensor">valor lido</p>
			</div>
			<div class="cmd-area">
				<button type="button" id="btn1" onclick="comando()">Liga</button>
			</div>
		<div class="cmd-area">
			<button type="button" id="buzz" onclick="buzzer()">Buzina</button>
		</div>
			<div class="cmd-area">
				<button type="button" id="1" onclick="dimmer(this)">25%</button>
				<button type="button" id="2" onclick="dimmer(this)">50%</button>
				<button type="button" id="3" onclick="dimmer(this)">80%</button>
			</div>
		</main>
		<footer>
			<p>Criado por: Washington Lisboa</p>
			<a href="https://github.com/wlisboa/iot_luminaria">fonte do codigo</a>
		</footer>
	</body>
		<script>
			function sensor() {
				var xhttp = new XMLHttpRequest();
				xhttp.onreadystatechange = function()
				{
					if (xhttp.readyState == 4 && xhttp.status == 200)
					{
						document.getElementById("sensor").innerHTML = xhttp.responseText;
					}
				};
				xhttp.open("POST","/sensor",true);
				xhttp.send();
			}
			function comando() {
				var xhttp = new XMLHttpRequest();
				xhttp.onreadystatechange = function()
				{
					if (xhttp.readyState == 4 && xhttp.status == 200)
					{
						document.getElementById("btn1").innerHTML = xhttp.responseText;
					}
				};
				xhttp.open("POST","/cmd1",true);
				xhttp.send();
			}
			function buzzer() {
				var xhttp = new XMLHttpRequest();
				xhttp.onreadystatechange = function(){
					if (xhttp.readyState == 4 && xhttp.status == 200){
					document.getElementById("btn1").innerHTML = xhttp.responseText;
				}
			};
				xhttp.open("POST","/buzzer",true);
				xhttp.send();
			}
			function dimmer(button){
				if (button.id=='1'){
					var xhttp = new XMLHttpRequest();
					xhttp.open("POST", "/25", true);
					xhttp.send();
				}
				if (button.id=='2'){
					var xhttp = new XMLHttpRequest();
					xhttp.open("POST", "/50", true);
					xhttp.send();
				}
				if (button.id=='3'){
					var xhttp = new XMLHttpRequest();
					xhttp.open("POST", "/80", true);
					xhttp.send();
				}
			}
		</script>
		<style>
			body {
				text-align: center;
			}
			button {
				height: 50px;
				width: 150px;
			}
			header {
				padding-top: 5px;
				border-bottom: 2px solid #000;
				background-color: #38C2B3;
			}
			main {
				border-bottom: 2px solid #000;
			}
			.sens-area{
				border-bottom: 2px solid #000;
			}
			.cmd-area{
				display: fex;
				align-items: center;
				justify-content: space-between;
				border-bottom: 2px solid #000;
				padding-top: 10px;
				padding-bottom: 10px;
			}
		</style>
	</html>
)=====";
