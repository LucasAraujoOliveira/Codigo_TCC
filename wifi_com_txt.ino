#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>


// Variáveis----------------------------------------------------------------

unsigned long piloto, periodo; // Armazena o piloto e o período de um clock (piloto/23), respectivamente
int endereco[28]; // Armazena cada bit do endereço transmitido (código + anti code)
int dados[28]; // Armazena a duração em us de cada bit
int i; // Utilizado nos laços (for)
String codigo = ""; // Armazena o código lido
unsigned long tempo;

int auxiliar = 0;

//--------------------------------------------------------------------------

#define sinal 12  //Pino de entrada para o sinal do receptor
#define ledVerde 5 //LED verde: inclusão
#define ledVermelho 4 //LED vermelho: exclusão
#define botIncluir 13 // Botão incluir
#define botExcluir 0 // Botão excluir
//#define rele 14 //Relé de acionamento do portão


//----------------------------------------------------------------------------

String leituraWeb();
void removeUserCode(String username);


const char* ssid = "LucasTCC";
const char* password = "lucas1234";

const int gatePin = 14; // GPIO 14 para acionar o portão
const String LoginADM = "admin";
const String SenhaADM = "admin";

ESP8266WebServer server(80);

String generateRandomCode(int length) {
  String result = "";
  for (int i = 0; i < length; i++) {
    result += String(random(0, 10));
  }
  return result;
}

void handleRoot() {
  String page = R"(
<html>
<head>
<style>
  body {
    display: flex;
    justify-content: center;
    align-items: center;
    height: 100vh;
    flex-direction: column;
    font-family: 'Arial', sans-serif;
    background-color: #f0f0f0;
    margin: 0;
    padding: 0;
  }

  .container {
    text-align: center;
    border: 1px solid #ddd;
    border-radius: 8px;
    padding: 20px;
    background: white;
    box-shadow: 0 2px 4px rgba(0, 0, 0, 0.2);
    width: 300px;
  }

  button {
    width: 220px;
    height: 60px;
    font-size: 20px;
    margin: 10px 0;
    background-color: #007bff;
    color: white;
    border: none;
    border-radius: 5px;
    cursor: pointer;
    transition: background-color 0.3s;
  }

  button:hover {
    background-color: #0056b3;
  }
</style>
</head>
<body>
  <div class="container">
    <h1>Pagina Inicial</h1>
    <p><a href='/admin/login'><button>Administrador</button></a></p>
    <p><a href='/user/login'><button>Usuario</button></a></p>
    <p><a href='/visitor/login'><button>Visitante</button></a></p>
  </div>
</body>
</html>
)";
  server.send(200, "text/html", page);
}




void handleAdminLogin() {
  String page = R"(
<html>
<head>
<style>
  body {
    display: flex;
    justify-content: center;
    align-items: center;
    height: 100vh;
    flex-direction: column;
    font-family: 'Arial', sans-serif;
    background-color: #f0f0f0;
    margin: 0;
    padding: 0;
  }

  .container {
    text-align: center;
    border: 1px solid #ddd;
    border-radius: 8px;
    padding: 20px;
    background: white;
    box-shadow: 0 2px 4px rgba(0, 0, 0, 0.2);
    width: 300px;
  }

  input[type=text], input[type=password], input[type=submit] {
    width: 90%;
    padding: 12px 20px;
    margin: 8px 0;
    display: inline-block;
    border: 1px solid #ccc;
    border-radius: 4px;
    box-sizing: border-box;
  }

  input[type=submit] {
    font-size: 20px;
    background-color: #007bff;
    color: white;
    border: none;
    cursor: pointer;
    transition: background-color 0.3s;
  }

  input[type=submit]:hover {
    background-color: #0056b3;
  }

  button {
    width: 90%;
    padding: 12px 20px;
    font-size: 20px;
    background-color: #007bff;
    color: white;
    border: none;
    border-radius: 4px;
    cursor: pointer;
    transition: background-color 0.3s;
    text-align: center;
    display: inline-block;
    margin: 8px 0;
  }

  button:hover {
    background-color: #0056b3;
  }
</style>
</head>
<body>
  <div class="container">
    <h1>Login Administrador</h1>
    <form action='/admin/auth' method='POST'>
      Login:<br><input type='text' name='login'><br>
      Senha:<br><input type='password' name='senha'><br><br>
      <input type='submit' value='Entrar'>
    </form>
    <a href='/'><button type='button'>Voltar</button></a>
  </div>
</body>
</html>
)";
  server.send(200, "text/html", page);
}



void handleAdminAuth() {
  if (server.arg("login") == LoginADM && server.arg("senha") == SenhaADM) {
    server.sendHeader("Location", "/management", true);
    server.send(302, "text/plain", "");
  } else {
    String page = R"(
<html>
<head>
<style>
  body {
    display: flex;
    justify-content: center;
    align-items: center;
    height: 100vh;
    flex-direction: column;
    font-family: 'Arial', sans-serif;
    background-color: #f0f0f0;
    margin: 0;
    padding: 0;
  }

  .container {
    text-align: center;
    border: 1px solid #ddd;
    border-radius: 8px;
    padding: 20px;
    background: white;
    box-shadow: 0 2px 4px rgba(0, 0, 0, 0.2);
    width: 300px;
  }

  p {
    margin: 20px 0;
    font-size: 20px;
  }

  button {
    width: 90%;
    padding: 12px 20px;
    font-size: 20px;
    background-color: #007bff;
    color: white;
    border: none;
    border-radius: 4px;
    cursor: pointer;
    transition: background-color 0.3s;
  }

  button:hover {
    background-color: #0056b3;
  }

  a, a button {
    text-decoration: none;
  }
</style>
</head>
<body>
  <div class="container">
    <h1>Login ou senha incorretos!</h1>
    <a href='/admin/login'><button type='button'>Voltar</button></a>
  </div>
</body>
</html>
)";
    server.send(200, "text/html", page);
  }
}


void handleManagementPage() {
  String page = R"(
<html>
<head>
<style>
  body {
    display: flex;
    justify-content: center;
    align-items: center;
    height: 100vh;
    flex-direction: column;
    font-family: 'Arial', sans-serif;
    background-color: #f0f0f0;
    margin: 0;
    padding: 0;
  }

  .container {
    text-align: center;
    border: 1px solid #ddd;
    border-radius: 8px;
    padding: 20px;
    background: white;
    box-shadow: 0 2px 4px rgba(0, 0, 0, 0.2);
    width: 300px;
  }

  a {
    text-decoration: none;
  }

  button {
    width: 90%;
    padding: 12px 20px;
    font-size: 20px;
    background-color: #007bff;
    color: white;
    border: none;
    border-radius: 4px;
    cursor: pointer;
    transition: background-color 0.3s;
    display: block;
    margin: 10px auto;
    text-decoration: none; /* Remover sublinhado em alguns browsers */
  }

  button:hover {
    background-color: #0056b3;
  }
</style>
</head>
<body>
  <div class="container">
    <h1>Gerenciamento de Usuarios</h1>
    <a href='/list'><button>Listar Usuarios</button></a>
    <a href='/add'><button>Adicionar Usuario</button></a>
    <a href='/remove'><button>Remover Usuario</button></a>
    <a href='/'><button type='button'>Voltar</button></a>
  </div>
</body>
</html>
)";
  server.send(200, "text/html", page);
}



void handleUserList() {
  File file = LittleFS.open("/usuarios.txt", "r");
  if (!file) {
    server.send(200, "text/html", "Erro ao abrir arquivo usuarios.txt");
    return;
  }

  String fileContent = "<ul style='list-style-type: none; padding: 0;'>";
  while (file.available()) {
    String userLine = file.readStringUntil('\n');
    if (userLine.length() > 0) {
      int asteriskIndex = userLine.indexOf('*');
      if (asteriskIndex != -1) {
        String username = userLine.substring(0, asteriskIndex);
        fileContent += "<li style='padding: 5px;'>" + username + "</li>";
      }
    }
  }
  file.close();

  fileContent += "</ul>";
  String page = R"(
<html>
<head>
<style>
  body {
    display: flex;
    justify-content: center;
    align-items: center;
    height: 100vh;
    flex-direction: column;
    font-family: 'Arial', sans-serif;
    background-color: #f0f0f0;
    margin: 0;
    padding: 0;
  }

  .container {
    text-align: center;
    border: 1px solid #ddd;
    border-radius: 8px;
    padding: 20px;
    background: white;
    box-shadow: 0 2px 4px rgba(0, 0, 0, 0.2);
    width: 300px;
  }

  a {
    text-decoration: none;
  }

  button {
    width: 90%;
    padding: 12px 20px;
    font-size: 20px;
    background-color: #007bff;
    color: white;
    border: none;
    border-radius: 4px;
    cursor: pointer;
    transition: background-color 0.3s;
    display: block;
    margin: 10px auto;
  }

  button:hover {
    background-color: #0056b3;
  }
</style>
</head>
<body>
  <div class="container">
    <h1>Lista de Usuarios</h1>
    )"+ fileContent + R"(
    <a href='/management'><button type='button'>Voltar</button></a>
  </div>
</body>
</html>
)";
  server.send(200, "text/html", page);
}



void handleAddUserPage() {
  String page = R"(
<html>
<head>
<style>
  body {
    display: flex;
    justify-content: center;
    align-items: center;
    height: 100vh;
    flex-direction: column;
    font-family: 'Arial', sans-serif;
    background-color: #f0f0f0;
    margin: 0;
    padding: 0;
  }

  .container {
    text-align: center;
    border: 1px solid #ddd;
    border-radius: 8px;
    padding: 20px;
    background: white;
    box-shadow: 0 2px 4px rgba(0, 0, 0, 0.2);
    width: 300px;
  }

  input[type=text], input[type=password], input[type=submit], button {
    width: 90%;
    padding: 12px 20px;
    margin: 8px 0;
    display: inline-block;
    border: 1px solid #ccc;
    border-radius: 4px;
    box-sizing: border-box;
  }

  input[type=submit] {
    font-size: 20px;
    background-color: #007bff;
    color: white;
    border: none;
    cursor: pointer;
    transition: background-color 0.3s;
  }

  input[type=submit]:hover, button:hover {
    background-color: #0056b3;
  }

  a {
    text-decoration: none;
  }

  button {
    font-size: 20px;
    background-color: #007bff;
    color: white;
    border: none;
    cursor: pointer;
    transition: background-color 0.3s;
    text-align: center;
  }
</style>
</head>
<body>
  <div class="container">
    <h1>Adicionar Usuario</h1>
    <form action='/adduser' method='POST'>
      Usuario:<br><input type='text' name='username'><br>
      Senha:<br><input type='password' name='password'><br><br>
      <input type='submit' value='Adicionar'>
    </form>
    <a href='/management'><button type='button'>Voltar</button></a>
  </div>
</body>
</html>
)";
  server.send(200, "text/html", page);
}



void handleAddUser() {
  String username = server.arg("username");
  String password = server.arg("password");
  String userCode = ""; // Para armazenar o código capturado

  if (username.length() > 0 && password.length() > 0) {
    // Acende o LED verde indicando que está no modo de inclusão de código
    digitalWrite(ledVerde, HIGH);
    unsigned long startMillis = millis();

    // Aguarda até 10 segundos por um código válido ou até que um código seja recebido
    while (millis() - startMillis < 10000 && userCode.isEmpty()) {
      userCode = leituraWeb(); // Tenta ler um novo código
      //delay(100); // Um pequeno delay para não sobrecarregar o loop
      if(userCode != "")
      {
        break;
      }
    }

    digitalWrite(ledVerde, LOW); // Desliga o LED verde após a espera ou recebimento de um código

    // Se nenhum código foi recebido, atribui um padrão de zeros
    if (userCode.isEmpty()) {
      userCode = "00000000000000000000"; // 20 zeros
    }

    // Armazena o usuário, senha e código no arquivo de usuários
    File userFile = LittleFS.open("/usuarios.txt", "a");
    if (!userFile) {
      server.send(200, "text/html", "Erro ao abrir arquivo usuarios.txt");
      return;
    }

    String userData = username + "*" + password + "\n";
    userFile.print(userData);
    userFile.close();

    // Armazena o código e o usuário no arquivo de códigos
    File codeFile = LittleFS.open("/codigos.txt", "a");
    if (!codeFile) {
      server.send(200, "text/html", "Erro ao abrir arquivo codigos.txt");
      return;
    }

    String codeData = userCode + "*" + username + "\n";
    codeFile.print(codeData);
    codeFile.close();

    server.sendHeader("Location", "/management", true);
    server.send(302, "text/plain", "");
  } else {
    server.send(200, "text/html", "<p>Informação de usuario ou senha faltando.</p><a href='/add'><button>Voltar</button></a>");
  }
}


void handleRemoveUserPage() {
  String page = R"(
<html>
<head>
<style>
  body {
    display: flex;
    justify-content: center;
    align-items: center;
    height: 100vh;
    flex-direction: column;
    font-family: 'Arial', sans-serif;
    background-color: #f0f0f0;
    margin: 0;
    padding: 0;
  }

  .container {
    text-align: center;
    border: 1px solid #ddd;
    border-radius: 8px;
    padding: 20px;
    background: white;
    box-shadow: 0 2px 4px rgba(0, 0, 0, 0.2);
    width: 300px;
  }

  input[type=text], input[type=submit], button {
    width: 90%;
    padding: 12px 20px;
    margin: 8px 0;
    display: inline-block;
    border: 1px solid #ccc;
    border-radius: 4px;
    box-sizing: border-box;
  }

  input[type=submit] {
    font-size: 20px;
    background-color: #007bff;
    color: white;
    border: none;
    cursor: pointer;
    transition: background-color 0.3s;
  }

  input[type=submit]:hover, button:hover {
    background-color: #0056b3;
  }

  a button {
    text-decoration: none;
    width: 90%; /* Mantendo o tamanho consistente com os inputs */
    padding: 12px 20px; /* Mesmo padding que os inputs */
    font-size: 20px;
    background-color: #007bff;
    color: white;
    border: none;
    border-radius: 4px;
    cursor: pointer;
    transition: background-color 0.3s;
  }

  a button:hover {
    background-color: #0056b3;
  }
</style>
</head>
<body>
  <div class="container">
    <h1>Remover Usuario</h1>
    <form action='/removeuser' method='POST'>
      Usuario:<br><input type='text' name='username'><br><br>
      <input type='submit' value='Remover'>
    </form>
    <a href='/management'><button type='button'>Voltar</button></a>
  </div>
</body>
</html>
)";
  server.send(200, "text/html", page);
}


void handleRemoveUser() {
  String username = server.arg("username");
  if (username.length() > 0) {
    // Remove user from usuarios.txt
    bool userFound = false;
    File userFile = LittleFS.open("/usuarios.txt", "r+");
    String userData = "";
    String line;
    while (userFile.available()) {
      line = userFile.readStringUntil('\n');
      if (!line.startsWith(username + "*")) {
        userData += line + "\n";
      } else {
        userFound = true;
      }
    }
    userFile.close();

    if (userFound) {
      userFile = LittleFS.open("/usuarios.txt", "w");
      userFile.print(userData);
      userFile.close();

      // Remove user from codigos.txt
      removeUserCode(username);

      server.sendHeader("Location", "/management", true);
      server.send(302, "text/plain", "");
    } else {
      server.send(200, "text/html", "<p>Usuário não encontrado.</p><a href='/remove'><button>Voltar</button></a>");
    }
  } else {
    server.send(200, "text/html", "<p>Nome de usuário necessário.</p><a href='/remove'><button>Voltar</button></a>");
  }
}

void removeUserCode(String username) {
  File codeFileIn = LittleFS.open("/codigos.txt", "r");
  String codeFileData = "";
  String line;
  while (codeFileIn.available()) {
    line = codeFileIn.readStringUntil('\n');
    // Exclui apenas as linhas que contêm o nome de usuário especificado
    if (!line.endsWith("*" + username)) {
      codeFileData += line + "\n";
    }
  }
  codeFileIn.close();

  File codeFileOut = LittleFS.open("/codigos.txt", "w");
  codeFileOut.print(codeFileData);
  codeFileOut.close();
}



void handleUserLogin() {
  String page = R"(
<html>
<head>
<style>
  body {
    display: flex;
    justify-content: center;
    align-items: center;
    height: 100vh;
    flex-direction: column;
    font-family: 'Arial', sans-serif;
    background-color: #f0f0f0;
    margin: 0;
    padding: 0;
  }

  .container {
    text-align: center;
    border: 1px solid #ddd;
    border-radius: 8px;
    padding: 20px;
    background: white;
    box-shadow: 0 2px 4px rgba(0, 0, 0, 0.2);
    width: 300px;
  }

  input[type=text], input[type=password] {
    width: 90%;
    padding: 12px 20px;
    margin: 8px 0;
    display: inline-block;
    border: 1px solid #ccc;
    border-radius: 4px;
    box-sizing: border-box;
  }

  button, input[type=submit] {
    width: 90%;
    padding: 12px 20px;
    font-size: 20px;
    background-color: #007bff;
    color: white;
    border: none;
    border-radius: 4px;
    cursor: pointer;
    transition: background-color 0.3s;
  }

  button:hover, input[type=submit]:hover {
    background-color: #0056b3;
  }

  a, a button {
    text-decoration: none;
  }
</style>
</head>
<body>
  <div class="container">
    <h1>Login Usuario</h1>
    <form action='/user/auth' method='POST'>
      Usuario:<br><input type='text' name='username'><br>
      Senha:<br><input type='password' name='password'><br><br>
      <input type='submit' value='Entrar'>
    </form>
    <a href='/'><button type='button'>Voltar</button></a>
  </div>
</body>
</html>
)";
  server.send(200, "text/html", page);
}


void handleUserAuth() {
  String username = server.arg("username");
  String password = server.arg("password");
  File file = LittleFS.open("/usuarios.txt", "r");
  if (!file) {
    server.send(200, "text/html", "Erro ao abrir arquivo usuarios.txt");
    return;
  }

  bool authSuccess = false;
  String credentials = username + "*" + password + "\n";
  String line;
  while (file.available()) {
    line = file.readStringUntil('\n') + "\n";
    if (line.equals(credentials)) {
      authSuccess = true;
      break;
    }
  }
  file.close();

  if (authSuccess) {
    server.sendHeader("Location", "/user/panel", true);
    server.send(302, "text/plain", "");
  } else {
    String page = R"(
<html>
<head>
<style>
  body {
    display: flex;
    justify-content: center;
    align-items: center;
    height: 100vh;
    flex-direction: column;
    font-family: 'Arial', sans-serif;
    background-color: #f0f0f0;
    margin: 0;
    padding: 0;
  }

  .container {
    text-align: center;
    border: 1px solid #ddd;
    border-radius: 8px;
    padding: 20px;
    background: white;
    box-shadow: 0 2px 4px rgba(0, 0, 0, 0.2);
    width: 300px;
  }

  p {
    margin: 20px 0;
    font-size: 20px;
  }

  button {
    width: 90%;
    padding: 12px 20px;
    font-size: 20px;
    background-color: #007bff;
    color: white;
    border: none;
    border-radius: 4px;
    cursor: pointer;
    transition: background-color 0.3s;
  }

  button:hover {
    background-color: #0056b3;
  }

  a, a button {
    text-decoration: none;
  }
</style>
</head>
<body>
  <div class="container">
    <h1>Usuario ou senha incorretos.</h1>
    <a href='/user/login'><button type='button'>Voltar</button></a>
  </div>
</body>
</html>
)";
    server.send(200, "text/html", page);
  }
}

void handleUserPanel() {
  String page = R"(
<html>
<head>
<style>
  body {
    display: flex;
    justify-content: center;
    align-items: center;
    height: 100vh;
    flex-direction: column;
    font-family: 'Arial', sans-serif;
    background-color: #f0f0f0;
    margin: 0;
    padding: 0;
  }

  .container {
    text-align: center;
    border: 1px solid #ddd;
    border-radius: 8px;
    padding: 20px;
    background: white;
    box-shadow: 0 2px 4px rgba(0, 0, 0, 0.2);
    width: 300px;
  }

  button {
    width: 90%;
    padding: 12px 20px;
    font-size: 20px;
    background-color: #007bff;
    color: white;
    border: none;
    border-radius: 4px;
    cursor: pointer;
    transition: background-color 0.3s;
    display: block;
    margin: 10px auto;
  }

  button:hover {
    background-color: #0056b3;
  }

  a, a button {
    text-decoration: none;
  }
</style>
</head>
<body>
  <div class="container">
    <h1>Painel do Usuario</h1>
    <form action='/user/gate' method='post'>
      <button type='submit'>Acionar Portao</button>
    </form>
    <form action='/user/generate' method='post'>
      <button type='submit'>Gerar Codigo Visitante</button>
    </form>
    <a href='/'><button type='button'>Voltar</button></a>
  </div>
</body>
</html>
)";
  server.send(200, "text/html", page);
}


void handleGate() {
  digitalWrite(gatePin, HIGH);
  delay(1000);
  digitalWrite(gatePin, LOW);
  server.sendHeader("Location", "/user/panel", true);
  server.send(302, "text/plain", "");
}

void handleGenerateCode() {
  String code = generateRandomCode(5);
  File file = LittleFS.open("/visitantes.txt", "a");
  if (!file) {
    server.send(200, "text/html", "Erro ao abrir arquivo visitantes.txt");
    return;
  }
  
  file.println(code);
  file.close();

  String page = R"(
<html>
<head>
<style>
  body {
    display: flex;
    justify-content: center;
    align-items: center;
    height: 100vh;
    flex-direction: column;
    font-family: 'Arial', sans-serif;
    background-color: #f0f0f0;
    margin: 0;
    padding: 0;
  }

  .container {
    text-align: center;
    border: 1px solid #ddd;
    border-radius: 8px;
    padding: 20px;
    background: white;
    box-shadow: 0 2px 4px rgba(0, 0, 0, 0.2);
    width: 300px;
  }

  p {
    margin: 20px 0;
    font-size: 20px;
  }

  button {
    width: 90%;
    padding: 12px 20px;
    font-size: 20px;
    background-color: #007bff;
    color: white;
    border: none;
    border-radius: 4px;
    cursor: pointer;
    transition: background-color 0.3s;
    display: block;
    margin: 10px auto;
  }

  button:hover {
    background-color: #0056b3;
  }

  a, a button {
    text-decoration: none;
  }
</style>
</head>
<body>
  <div class="container">
    <h1>Painel do Usuario</h1>
    <p>Codigo Gerado: )"+ code + R"(</p>
    <a href='/user/panel'><button type='button'>Voltar</button></a>
  </div>
</body>
</html>
)";
  server.send(200, "text/html", page);
}

void handleVisitorLogin() {
  String page = R"(
<html>
<head>
<style>
  body {
    display: flex;
    justify-content: center;
    align-items: center;
    height: 100vh;
    flex-direction: column;
    font-family: 'Arial', sans-serif;
    background-color: #f0f0f0;
    margin: 0;
    padding: 0;
  }

  .container {
    text-align: center;
    border: 1px solid #ddd;
    border-radius: 8px;
    padding: 20px;
    background: white;
    box-shadow: 0 2px 4px rgba(0, 0, 0, 0.2);
    width: 300px;
  }

  input[type=text], input[type=submit], button {
    width: 90%;
    padding: 12px 20px;
    margin: 8px 0;
    display: inline-block;
    border: 1px solid #ccc;
    border-radius: 4px;
    box-sizing: border-box;
    font-size: 20px;
  }

  input[type=submit] {
    background-color: #007bff;
    color: white;
    border: none;
    cursor: pointer;
    transition: background-color 0.3s;
  }

  input[type=submit]:hover {
    background-color: #0056b3;
  }

  a button, button {
    text-decoration: none;
    background-color: #007bff;
    color: white;
    border: none;
    cursor: pointer;
    transition: background-color 0.3s;
  }

  a button:hover, button:hover {
    background-color: #0056b3;
  }
</style>
</head>
<body>
  <div class="container">
    <h1>Visitante</h1>
    <form action='/visitor/verify' method='POST'>
      Codigo:<br><input type='text' name='code'><br>
      <input type='submit' value='Acionar portao'>
    </form>
    <a href='/'><button type='button'>Voltar</button></a>
  </div>
</body>
</html>
)";
  server.send(200, "text/html", page);
}



void handleVisitorVerify() {
  String code = server.arg("code");
  File file = LittleFS.open("/visitantes.txt", "r");
  if (!file) {
    server.send(200, "text/html", "Erro ao abrir arquivo visitantes.txt");
    return;
  }

  bool codeExists = false;
  String fileContent = "";
  String currentCode;
  while (file.available()) {
    currentCode = file.readStringUntil('\n');
    currentCode.trim();
    if (currentCode.equals(code)) {
      codeExists = true;
    } else {
      if (currentCode.length() > 0) {
        fileContent += currentCode + "\n";
      }
    }
  }
  file.close();

  if (codeExists) {
    File file = LittleFS.open("/visitantes.txt", "w");
    file.print(fileContent);
    file.close();

    digitalWrite(gatePin, HIGH);
    delay(1000);
    digitalWrite(gatePin, LOW);
    String page = R"(
<html>
<head>
<style>
  body {
    display: flex;
    justify-content: center;
    align-items: center;
    height: 100vh;
    flex-direction: column;
    font-family: 'Arial', sans-serif;
    background-color: #f0f0f0;
    margin: 0;
    padding: 0;
  }

  .container {
    text-align: center;
    border: 1px solid #ddd;
    border-radius: 8px;
    padding: 20px;
    background: white;
    box-shadow: 0 2px 4px rgba(0, 0, 0, 0.2);
    width: 300px;
  }

  button {
    width: 90%;
    padding: 12px 20px;
    font-size: 20px;
    background-color: #007bff;
    color: white;
    border: none;
    border-radius: 4px;
    cursor: pointer;
    transition: background-color 0.3s;
  }

  button:hover {
    background-color: #0056b3;
  }

  a, a button {
    text-decoration: none;
  }
</style>
</head>
<body>
  <div class="container">
    <h1>Portao Acionado com Sucesso</h1>
    <a href='/'><button type='button'>Voltar</button></a>
  </div>
</body>
</html>
)";
    server.send(200, "text/html", page);
  } else {
    String page = R"(
<html>
<head>
<style>
  body {
    display: flex;
    justify-content: center;
    align-items: center;
    height: 100vh;
    flex-direction: column;
    font-family: 'Arial', sans-serif;
    background-color: #f0f0f0;
    margin: 0;
    padding: 0;
  }

  .container {
    text-align: center;
    border: 1px solid #ddd;
    border-radius: 8px;
    padding: 20px;
    background: white;
    box-shadow: 0 2px 4px rgba(0, 0, 0, 0.2);
    width: 300px;
  }

  button {
    width: 90%;
    padding: 12px 20px;
    font-size: 20px;
    background-color: #007bff;
    color: white;
    border: none;
    border-radius: 4px;
    cursor: pointer;
    transition: background-color 0.3s;
  }

  button:hover {
    background-color: #0056b3;
  }

  a, a button {
    text-decoration: none;
  }
</style>
</head>
<body>
  <div class="container">
    <h1>Codigo Invalido</h1>
    <a href='/visitor/login'><button type='button'>Voltar</button></a>
  </div>
</body>
</html>
)";
    server.send(200, "text/html", page);
  }
}




void setup() {

  //Inicializando as GPIOs
  pinMode(sinal, INPUT); //Sinal do receptor
  pinMode(ledVerde, OUTPUT); //LED verde: inclusão
  pinMode(ledVermelho, OUTPUT); //LED vermelho: exclusão
  pinMode(botIncluir, INPUT_PULLUP); //Botão incluir
  pinMode(botExcluir, INPUT_PULLUP); //Botão excluir
  //pinMode(rele, OUTPUT); //Relé de acionamento do portão
  



  
  Serial.begin(115200);
  pinMode(gatePin, OUTPUT);
  digitalWrite(gatePin, LOW);
  
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  if (!LittleFS.begin()) {
    Serial.println("An error has occurred while mounting LittleFS");
    return;
  }

  server.on("/", handleRoot);
  server.on("/admin/login", handleAdminLogin);
  server.on("/admin/auth", HTTP_POST, handleAdminAuth);
  server.on("/management", handleManagementPage);
  server.on("/list", handleUserList);
  server.on("/add", handleAddUserPage);
  server.on("/adduser", HTTP_POST, handleAddUser);
  server.on("/remove", handleRemoveUserPage);
  server.on("/removeuser", HTTP_POST, handleRemoveUser);
  server.on("/user/login", handleUserLogin);
  server.on("/user/auth", HTTP_POST, handleUserAuth);
  server.on("/user/panel", handleUserPanel);
  server.on("/user/gate", HTTP_POST, handleGate);
  server.on("/user/generate", HTTP_POST, handleGenerateCode);
  server.on("/visitor/login", handleVisitorLogin);
  server.on("/visitor/verify", HTTP_POST, handleVisitorVerify);

  server.begin();
  Serial.println("Servidor HTTP iniciado.");

  tempo = millis(); //Inicializando 'tempo' com o valor atual de millis()
}


//Protótipos das funções----------------------------------------------------

void excluirCodigo(String cod_excluir); // Exclui um código específico
void mostrarCodigos (); // Exibe todos os códigos que estão cadastrados
void incluirCodigo(String cod_Incluir, String usuario); // Inclui um código específico
bool verificarCodigo(String cod_verificar); // Verifica se o arquivo contém um código específico
String leitura(); //Função que realiza a leitura e decodificação do código transmitido

//--------------------------------------------------------------------------


void loop() {
  server.handleClient();

  codigo = ""; //Limpa a viriável 'codigo'
  codigo = leitura();

  if((codigo != "") && ((millis() - tempo) > 1000)){ //Verifica se há algum código e se já se passou 1s desde a última leitura
    if(verificarCodigo(codigo)){ //Se o código for válido...
      Serial.println("Codigo valido");
      tempo = millis();
      while((millis() - tempo) < 1000){
        digitalWrite(gatePin, HIGH);
      }
      digitalWrite(gatePin, LOW);
    }
    else{
      Serial.println("Codigo invalido");
    }
    tempo = millis(); //Reseta a variável tempo para o tempo atual
  }

  //- - - - - - - - - - - - - - - - - Inclusão - - - - - - - - - - - - - - - - - - -
  
  if(digitalRead(botIncluir) == LOW){ //Se o botão de incluir for pressionado
    tempo = millis(); //Reseta o 'tempo' com o millis atual
    digitalWrite(ledVerde, HIGH); //Liga o LED verde
    
    while(auxiliar == 0){ //Entra em modo de aguardo durante 10 segundos, para que seja fornecido um código
      if(((millis() - tempo) > 10000)){ //Após 10 segundos, 'auxiliar' recebe 1 e o loop é cancelado
        auxiliar = 1;
      }
      
      codigo = leitura();
      if(codigo != ""){ //Se receber algum código...
        if(verificarCodigo(codigo) == false){ //Verifica se o código não existe, assim o mesmo é incluso
          incluirCodigo(codigo, "0000");
          mostrarCodigos();
          break; //Sai do modo de aguardo
        }
        else{ //Se o código já existe, sai do modo de aguardo
          Serial.println("Codigo ja existe");
          break;
        }
      }
    }
    tempo = millis(); //Reseta o 'tempo' com o millis atual
    auxiliar = 0; //Reseta a variável 'auxiliar'
    delay(100); //Aguarda 0,1 s para dar tempo do usuário despressionar o botão do controle e não abrir o portão logo após o cadastro
    digitalWrite(ledVerde, LOW); //Desliga o LED de cadastro

    }
  
  //- - - - - - - - - - - - - - - - - Exclusão - - - - - - - - - - - - - - - - - - -

  if(digitalRead(botExcluir) == LOW){ //Se o botão de excliir for pressionado
    //Serial.println("botao exclusao");
    tempo = millis(); //Reseta o 'tempo' com o millis atual
    digitalWrite(ledVermelho, HIGH); //Liga o LED vermelho
    while(auxiliar == 0){ //Entra em modo de aguardo durante 10 segundos, para que seja fornecido um código
      if(((millis() - tempo) > 10000)){//Após 10 segundos, 'auxiliar' recebe 1 e o loop é cancelado
        auxiliar = 1;
      }
      
      codigo = leitura();
      if(codigo != ""){ //Se receber algum código...
        if(verificarCodigo(codigo) == true){ //Verifiva se o código existe e realiza a exclusão do mesmo
          excluirCodigo(codigo);
          mostrarCodigos();
          break; //Sai do loop
        }
        else{ //Caso não exista o código, saí do loop
          Serial.println("Codigo nao existe");
          break;
        }
      }
    }
    tempo = millis(); //Reseta o 'tempo' com o millis atual
    auxiliar = 0; //Reseta a variável 'auxiliar'
    delay(100); //Aguarda 0,1 s para dar tempo do usuário despressionar o botão do controle
    digitalWrite(ledVermelho, LOW); //Desliga o LED de exclusão
  }
}


//Funções-------------------------------------------------------------------

void excluirCodigo(String cod_excluir)
{
  String arquivoEntrada = "/codigos.txt";
  
  // Código de identificação que você deseja remover
  String codigoARemover = cod_excluir;

  // Abre o arquivo de entrada para leitura
  File fileIn = LittleFS.open(arquivoEntrada, "r");

  // Verifica se o arquivo de entrada foi aberto com sucesso
  if (!fileIn) {
    Serial.println("Falha ao abrir o arquivo de entrada para leitura");
    return;
  }

  // Nome do arquivo de saída (sem o código)
  String arquivoSaida = "/seuarquivo_sem_codigo.txt";

  // Abre o arquivo de saída para escrita
  File fileOut = LittleFS.open(arquivoSaida, "w");

  // Verifica se o arquivo de saída foi aberto com sucesso
  if (!fileOut) {
    Serial.println("Falha ao abrir o arquivo de saída para escrita");
    fileIn.close(); // Feche o arquivo de entrada antes de sair
    return;
  }

  // Lê o conteúdo do arquivo de entrada linha por linha
  while (fileIn.available()) {
    String linha = fileIn.readStringUntil('\n');
    // Verifica se a linha contém o código a ser removido
    if (!linha.startsWith(codigoARemover)) { // startsWith verifica se a string linha começa com um determinado parâmetro, neste caso verifica se a linha contém o código
      // Se não contiver, escreva a linha no arquivo de saída
      fileOut.println(linha);
    }
  }

  // Fecha ambos os arquivos
  fileIn.close();
  fileOut.close();

  // Substitui o arquivo original pelo arquivo de saída
  LittleFS.remove(arquivoEntrada); // Excliu o primeiro arquivo, que contém todos os códigos
  LittleFS.rename(arquivoSaida, arquivoEntrada); // Renomeia o segundo arquivo, que não contém o código excluído, com o nome do primeiro arquivo

  Serial.println("Código removido com sucesso!");
}

//--------------------------------------------------------------------------

void mostrarCodigos ()
{
  // Abre o arquivo para leitura
  File file = LittleFS.open("/codigos.txt", "r");

  // Verifica se o arquivo de entrada foi aberto com sucesso
  if (!file) {
    Serial.println("Falha ao abrir o arquivo para leitura");
    return;
  }

  // Lê e imprime o conteúdo do arquivo
  while (file.available()) {
    Serial.write(file.read());
  }
  
  // Fecha o arquivo
  file.close();
}

//--------------------------------------------------------------------------

void incluirCodigo(String cod_Incluir, String usuario){
  
  String arquivo = "/codigos.txt";
  
  // Código que você deseja adicionar
  String novaLinha = cod_Incluir + "*" + usuario;

  // Abre o arquivo no modo de appending
  File file = LittleFS.open(arquivo, "a");

  // Verifica se o arquivo foi aberto com sucesso
  if (!file) {
    Serial.println("Falha ao abrir o arquivo para anexação");
    return;
  }

  // Escreve a nova linha (com o código) no arquivo
  file.println(novaLinha);

  // Fecha o arquivo após a escrita
  file.close();

  Serial.println("Nova linha com o código adicionada com sucesso!");
}

//--------------------------------------------------------------------------

bool verificarCodigo(String cod_verificar){
  
  String arquivo = "/codigos.txt";
  bool codigoEncontrado = false;
  // Código para verificar verificar
  String codigoProcurado = cod_verificar;

  // Abre o arquivo no modo de leitura
  File file = LittleFS.open(arquivo, "r");

  // Verifica se o arquivo foi aberto com sucesso
  if (!file) {
    Serial.println("Falha ao abrir o arquivo para leitura");
    return codigoEncontrado;
  }

  // Lê o conteúdo do arquivo linha por linha
  
  while (file.available()) {
    String linha = file.readStringUntil('\n');
    // Verifica se a linha contém o código procurado
    if (linha.startsWith(codigoProcurado)) { // startsWith verifica se a string linha começa com um determinado parâmetro, neste caso verifica se a linha contém o código
      codigoEncontrado = true;
      break; 
    }
  }

  // Fecha o arquivo após a leitura
  file.close();

  return codigoEncontrado; // Retorna True se encontrar e False se não encontrar
}

//-------------------------------------------------------------------------

String leitura(){  

  String codigo_txt = "";

  piloto = pulseIn(sinal, LOW); // Verifica quanto tempo em us o sinal fica em LOW
  
  periodo = piloto/23;  // 1 período de clock

  if((piloto > 7000) && (piloto < 14000)) // Verifica se o piloto recebido é válido
  {
    for(i = 0; i < 28; i++) // Armazena em 'dados' a duração em us de cada bit
    {
      dados[i] = pulseIn(sinal, HIGH);
    }

    for(i = 0; i < 28; i++) // Verifica se os bits são HIGH ou LOW e atribui em endereco[]
    {
      if((dados[i] > periodo*0.4) && (dados[i] < periodo*1.4))
      {
        endereco[i] = 1;
      }
      else
      {
        if((dados[i] > periodo*1.4) && (dados[i] < periodo*2.7))
        {
          endereco[i] = 0;
        }
      }
    }
    
    if((endereco[24]==0)&&(endereco[25]==1)&&(endereco[26]==0)&&(endereco[27]==1))  // Verifica se os bits de anti code (0101) são válidos
    {
      codigo_txt = "";

      for(i = 0; i < 24; i++) // Escreve o endereço do código em uma string
      {
        if(endereco[i] == 1)
        {
          codigo_txt = codigo_txt+'1';
        }
        else
        {
          codigo_txt = codigo_txt+'0';
        }
      }
    }
  }

  return codigo_txt; // Retorna o código
}



String leituraWeb(){  

  String codigo_txt = "";

  piloto = pulseIn(sinal, LOW); // Verifica quanto tempo em us o sinal fica em LOW
  
  periodo = piloto/23;  // 1 período de clock

  if((piloto > 7000) && (piloto < 14000)) // Verifica se o piloto recebido é válido
  {
    for(i = 0; i < 28; i++) // Armazena em 'dados' a duração em us de cada bit
    {
      dados[i] = pulseIn(sinal, HIGH);
    }

    for(i = 0; i < 28; i++) // Verifica se os bits são HIGH ou LOW e atribui em endereco[]
    {
      if((dados[i] > periodo*0.4) && (dados[i] < periodo*1.4))
      {
        endereco[i] = 1;
      }
      else
      {
        if((dados[i] > periodo*1.4) && (dados[i] < periodo*2.7))
        {
          endereco[i] = 0;
        }
      }
    }
    
    if((endereco[24]==0)&&(endereco[25]==1)&&(endereco[26]==0)&&(endereco[27]==1))  // Verifica se os bits de anti code (0101) são válidos
    {
      codigo_txt = "";

      for(i = 0; i < 24; i++) // Escreve o endereço do código em uma string
      {
        if(endereco[i] == 1)
        {
          codigo_txt = codigo_txt+'1';
        }
        else
        {
          codigo_txt = codigo_txt+'0';
        }
      }
    }
  }

  return codigo_txt; // Retorna o código
}
