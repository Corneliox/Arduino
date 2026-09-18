int ledPin = 13;
const int buzzer = 9;

void setup() {
  // put your setup code here, to run once:
  pinMode(ledPin,OUTPUT);
  pinMode(buzzer,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
digitalWrite(ledPin,HIGH);
delay(300);
digitalWrite(ledPin,LOW);
delay(300);

digitalWrite(ledPin,HIGH);
delay(300);
digitalWrite(ledPin,LOW);
delay(300);

digitalWrite(ledPin,HIGH);
delay(300);
digitalWrite(ledPin,LOW);
delay(300);

digitalWrite(ledPin,HIGH);
delay(300);
digitalWrite(ledPin,LOW);
delay(3000);


digitalWrite(ledPin,HIGH);
delay(300);
digitalWrite(ledPin,LOW);
delay(300);

digitalWrite(ledPin,HIGH);
delay(300);
digitalWrite(ledPin,LOW);
delay(300);

digitalWrite(ledPin,HIGH);
delay(300);
digitalWrite(ledPin,LOW);
delay(3000);

digitalWrite(ledPin,HIGH);
delay(300);
digitalWrite(ledPin,LOW);
delay(300);

digitalWrite(ledPin,HIGH);
delay(300);
digitalWrite(ledPin,LOW);
delay(3000);

// 600.400.100.4.5.8...5.6.5.1..4
tone(buzzer,100);
delay(1000);
noTone(buzzer);
delay(1000);
}
