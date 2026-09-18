const int buzzer = 9;

void setup() {
  // put your setup code here, to run once:
pinMode(buzzer,OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
tone(buzzer,600);
delay(200);
noTone(buzzer);
delay(200);

tone(buzzer,400);
delay(200);
noTone(buzzer);
delay(200);

tone(buzzer,100);
delay(200);
noTone(buzzer);
delay(200);

tone(buzzer,400);
delay(200);
noTone(buzzer);
delay(200);

tone(buzzer,500);
delay(200);
noTone(buzzer);
delay(200);

tone(buzzer,800);
delay(200);
noTone(buzzer);
delay(1000);

tone(buzzer,500);
delay(200);
noTone(buzzer);
delay(200);

tone(buzzer,600);
delay(200);
noTone(buzzer);
delay(200);

tone(buzzer,500);
delay(200);
noTone(buzzer);
delay(200);

tone(buzzer,100);
delay(200);
noTone(buzzer);
delay(200);

tone(buzzer,400);
delay(200);
noTone(buzzer);
delay(3000);
}
