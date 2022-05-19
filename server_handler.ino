void create_path_handler(void)
{
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
    { 
        request->send(LittleFS, "/index.html", "text/html"); 
    });
    
    server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request)
    { 
        request->send(LittleFS, "/style.css", "text/css"); 
    });
    
    server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request)
    { 
        request->send(LittleFS, "/script.js", "text/js"); 
    });
  
    server.on("/icon.png",  HTTP_GET, [](AsyncWebServerRequest *request)
    { 
        request->send(LittleFS, "/icon.png",  "image/png"); 
    });
    
    server.on("/view.png",  HTTP_GET, [](AsyncWebServerRequest *request)
    { 
        request->send(LittleFS, "/view.png",  "image/png"); 
    });
    
    server.on("/knob.png",  HTTP_GET, [](AsyncWebServerRequest *request)
    { 
        request->send(LittleFS, "/knob.png",  "image/png"); 
    });
    
    server.on("/fader.png", HTTP_GET, [](AsyncWebServerRequest *request)
    { 
        request->send(LittleFS, "/fader.png", "image/png"); 
    });

    server.on("/admin", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        String username = "";
        String password = "";

        if(request->hasParam("ssid") && request->hasParam("pass"))
        {
            username = request->getParam("ssid")->value();
            password = request->getParam("pass")->value();

            write_string(15, username);
            write_string(35, password);
    
            request->send(200, "text/plain", "SSID dan PASSWORD Berhasil Diperbarui \nSilahkan Pilih Mode STA Dan Restart Untuk Menghubungkan Ke Router");
        }
        else
        {
            request->send(200, "text/plain", "Keyword Salah");
        }
    });
}

void socket_event(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
    switch(type)
    {
        case WS_EVT_CONNECT    : socket_onopen();                break;
        case WS_EVT_DISCONNECT : socket_onclose();               break;
        case WS_EVT_DATA       : handle_message(arg, data, len); break;
    }
}

void socket_onopen(void)
{
    digitalWrite(2, LOW);
    
    String data_setup = "";
    String prefix[13] = { "A","B","D","E","F","G","H","I","J","K","L","M","O" };
    
    for(unsigned char index=0; index<13; index++)
    {
        int8_t value = EEPROM.read(index);
        String value_string = String(value);
        String len = String(value_string.length());

        data_setup += prefix[index] + len + value_string;
    }

    String volume_string = String(volume_value);
    String subwoofer_string = String(subwoofer_value);

    data_setup += "C" + String(volume_string.length()) + volume_string;
    data_setup += "N" + String(subwoofer_string.length()) + subwoofer_string;
    
    socket.textAll(data_setup);
}

void socket_onclose(void)
{
    digitalWrite(2, HIGH);
}

void handle_message(void *arg, uint8_t *data, size_t len)
{
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if(info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
    {
        data[len] = 0;
        String message = (char*)data;
        processor_handler(message, true);
    }
}

void write_string(unsigned char addr, String value)
{
    unsigned char len = value.length();

    for(unsigned char index=0; index<len; index++)
    {
        EEPROM.write(addr+index, value[index]);
    }

    EEPROM.write(addr+len, '\0');
    EEPROM.commit();
}

String read_string(unsigned char addr)
{
    char ascii[20];
    
    unsigned char index = 0;
    unsigned char value = EEPROM.read(addr);

    while(value != '\0' && index < 20)
    {
        value = EEPROM.read(addr+index);
        ascii[index] = value;
        index++;
    }

    return String(ascii);
}

