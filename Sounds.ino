void  PlayStartup(){
        digitalWrite(ledPin, HIGH);
        tone(buzzerPin, 1000, 50);   //Time,Frequency
        delay(80);
        digitalWrite(ledPin, LOW);
        tone(buzzerPin, 2000, 50);
        delay(80);
        digitalWrite(ledPin, HIGH);
        tone(buzzerPin, 1000, 50);
        delay(80);
        digitalWrite(ledPin, LOW);
        tone(buzzerPin, 2000, 50);
        delay(80);
        digitalWrite(ledPin, HIGH);
        tone(buzzerPin, 4000, 50);
        delay(80);
        digitalWrite(ledPin, LOW);
        tone(buzzerPin, 2000, 50);
        delay(80);
        digitalWrite(ledPin, HIGH);
        tone(buzzerPin, 8000, 50);
        delay(80);
        digitalWrite(ledPin, LOW);
}

void PlayExit(){
         tone(buzzerPin, 400, 20);
         tone(buzzerPin, 400, 20);
         delay(50);
         tone(buzzerPin, 400, 20);
         tone(buzzerPin, 150, 20);
         delay(50);
         tone(buzzerPin, 200, 20);
         delay(50);
         tone(buzzerPin, 250, 20);
}

void PlayCruise(){
         tone(buzzerPin, 3500, 20);
         delay(50);
         tone(buzzerPin, 1500, 20);
}

void PlayConfirm(){
         tone(buzzerPin, 4000, 20);
         delay(50);
         tone(buzzerPin, 4000, 20);
         tone(buzzerPin, 1500, 20);
         delay(50);
         tone(buzzerPin, 2000, 20);
         delay(50);
         tone(buzzerPin, 2500, 20);
         delay(50);
         tone(buzzerPin, 3500, 20);
         tone(buzzerPin, 1500, 20);
         delay(50);
         tone(buzzerPin, 2000, 20);
         delay(50);
         tone(buzzerPin, 2500, 20);
         delay(50);
         tone(buzzerPin, 3500, 20);
       }
       
void PlayPush(){
       //  tone(buzzerPin, 3000, 15);
       }
       
void PlayConfiguration(){
         tone(buzzerPin, 1500, 20);
         delay(50);
         tone(buzzerPin, 2000, 20);
         delay(50);
         tone(buzzerPin, 2500, 20);
         delay(50);
         tone(buzzerPin, 3500, 20);
       }
       
void PlayError(){
         tone(buzzerPin, 200, 50);
         delay(50);
         tone(buzzerPin, 200, 50);

       }
       
void PlayArming(){
         tone(buzzerPin, 1000, 20);
         delay(20);
         tone(buzzerPin, 2000, 20);
         delay(40);
         tone(buzzerPin, 4000, 20);
         delay(80);
         tone(buzzerPin, 9000, 20);
         delay(20);
       }
       
void PlayArmed(){
         tone(buzzerPin, 262, 10); tone(buzzerPin, 262, 10); tone(buzzerPin, 262, 100);
         tone(buzzerPin, 262, 10); tone(buzzerPin, 262, 10); tone(buzzerPin, 0, 100);
         tone(buzzerPin, 262, 10); tone(buzzerPin, 262, 10); tone(buzzerPin, 0, 100);
         tone(buzzerPin, 262, 10); tone(buzzerPin, 262, 10); tone(buzzerPin, 262, 100);
       }


void PlaySomeFuturisticSound(){
      digitalWrite(ledPin, LOW);tone(buzzerPin, 2000, 25);delay(50);
      digitalWrite(ledPin, HIGH);tone(buzzerPin, 1800, 25);delay(50);
      digitalWrite(ledPin, LOW);tone(buzzerPin, 2000, 25);delay(50);
      digitalWrite(ledPin, HIGH);tone(buzzerPin, 1800, 25);delay(50);
      digitalWrite(ledPin, LOW);tone(buzzerPin, 2000, 25);delay(50);
      tone(buzzerPin, 2000, 400);delay(50);
      tone(buzzerPin, 2050, 400);delay(50);
      tone(buzzerPin, 2100, 400);delay(50);
      tone(buzzerPin, 2150, 200);delay(50);
      tone(buzzerPin, 2200, 100);delay(50);
      delay(100);
      tone(buzzerPin, 2000, 400);delay(50);
      tone(buzzerPin, 2050, 400);delay(50);
      tone(buzzerPin, 2100, 400);delay(50);
      tone(buzzerPin, 2150, 200);delay(50);
      tone(buzzerPin, 2200, 100);delay(50);  
       }

       
