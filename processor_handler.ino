unsigned char main_selector;
unsigned char loudness;
unsigned char frekuensi;
unsigned char treble;

void send_command(unsigned char address, unsigned char value)
{
    Wire.beginTransmission(0x44);
    Wire.write(address);
    Wire.write(value);
    Wire.endTransmission();

    Serial.print(address, HEX); Serial.print(" -> "); Serial.println(value, HEX);
}

void setup_processor(void)
{
    String prefix[13] = { "A","B","D","E","F","G","H","I","J","K","L","M","O" };

    send_command(0x42, 0xEB);
    send_command(0x43, 0x5F); // volume
    send_command(0x47, 0x01);
    send_command(0x4C, 0x00);
    send_command(0x4D, 0x00);
    send_command(0x4E, 0x00);
    send_command(0x4F, 0x5F); // subwoofer
    send_command(0x50, 0x9C);

    for(unsigned char index=0; index<13; index++)
    {
        int8_t value = EEPROM.read(index);

        String value_string = String(value);
        String len = String(value_string.length());

        processor_handler( prefix[index] + len + value_string, false);
    }
}

void processor_handler(String message, bool eeprom_save)
{
    String data;
    char prefix = message.charAt(0);
    char data_length = message.charAt(1);

    switch(data_length)
    {
        case '1' : data = message.substring(2,3); break;
        case '2' : data = message.substring(2,4); break;
        case '3' : data = message.substring(2,5); break;
    }

    int8_t value = data.toInt();

    switch(prefix)
    {
        case 'A' :
            main_selector = (main_selector & 0x78) | value;
            send_command(0x40, main_selector);
            if(eeprom_save == true){ EEPROM.write(0,value); EEPROM.commit(); }
        break;

        case 'B' :
            main_selector = (main_selector & 0x07) | (value<<3);
            send_command(0x40, main_selector);
            if(eeprom_save == true){ EEPROM.write(1,value); EEPROM.commit(); }
        break;

        case 'C' :
            if(value < 0){ send_command(0x43,16-value); }
            else { send_command(0x43, value); }
            volume_value = value;
        break;

        case 'D' :
            if(value < 0){ send_command(0x4A,16-value); send_command(0x4B,0); }
            else{ send_command(0x4A,0); send_command(0x4B,value+16); }
            if(eeprom_save == true){ EEPROM.write(2,value); EEPROM.commit(); }
        break;

        case 'E' :
            if(value < 4){ send_command(0x49, 0xF7); }
            else{ send_command(0x49, (((value-4)/2)<<4)|0x07); }
            if(eeprom_save == true){ EEPROM.write(3,value); EEPROM.commit(); }
        break;

        case 'F' :
            loudness = (loudness & 0x70) | (value * -1) | 0x40;
            send_command(0x41, loudness);
            if(eeprom_save == true){ EEPROM.write(4,value); EEPROM.commit(); }
        break;

        case 'G' :
            loudness = (loudness & 0x4F) | (value << 4) | 0x40;
            send_command(0x41, loudness);
            if(eeprom_save == true){ EEPROM.write(5,value); EEPROM.commit(); }
        break;

        case 'H' :
            if(value < 0){ send_command(0x46, value * -1); }
            else{ send_command(0x46, value + 16); }
            if(eeprom_save == true){ EEPROM.write(6,value); EEPROM.commit(); }
        break;

        case 'I' :
            frekuensi = (frekuensi & 0xCF) | (value << 4) | 0xC0;
            send_command(0x48, frekuensi);
            if(eeprom_save == true){ EEPROM.write(7,value); EEPROM.commit(); }
        break;

        case 'J' :
            if(value < 0){ send_command(0x45, (value * -1) | 0x20); }
            else{ send_command(0x45, (value + 16) | 0x20); }
            if(eeprom_save == true){ EEPROM.write(8,value); EEPROM.commit(); }
        break;

        case 'K' :
            frekuensi = (frekuensi & 0xF3) | (value << 2) | 0xC0;
            send_command(0x48, frekuensi);
            if(eeprom_save == true){ EEPROM.write(9,value); EEPROM.commit(); }
        break;

        case 'L' :
            if(value < 0) { treble = (treble & 0xE0) | (value * -1) | 0x80; send_command(0x44, treble); }
            else { treble = (treble & 0xE0) | (value + 16) | 0x80; send_command(0x44, treble); }
            if(eeprom_save == true){ EEPROM.write(10,value); EEPROM.commit(); }
        break;

        case 'M' :
            treble = (treble & 0x9F) | (value << 5) | 0x80;
            send_command(0x44, treble);
            if(eeprom_save == true){ EEPROM.write(11,value); EEPROM.commit(); }
        break;

        case 'N' :
            if(value <= 0){ send_command(0x4F,16-value);}
            else { send_command(0x4F, value); }
            subwoofer_value = value;
        break;

        case 'O' :
            frekuensi = (frekuensi & 0xFC) | value | 0xC0;
            send_command(0x48, frekuensi);
            if(eeprom_save == true){ EEPROM.write(12,value); EEPROM.commit(); }
        break; 
    }
}

