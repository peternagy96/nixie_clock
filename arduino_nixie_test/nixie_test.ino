/**/// Tube pinsconst int hour_10 = 6; const int hour_1 = A3; const int colon = A2;const int min_10 = A1; const int min_1 = A0; // nixie driver pins:const int A = 5;const int B = 4;const int C = 3;const int D = 2;int n = 0;void setup(){   Serial.begin(9600);   pinMode(hour_10, OUTPUT);  pinMode(hour_1, OUTPUT);  pinMode(min_10, OUTPUT);  pinMode(min_1, OUTPUT);     pinMode(A, OUTPUT);  pinMode(B, OUTPUT);  pinMode(C, OUTPUT);  pinMode(D, OUTPUT);  }void loop(){   digitalWrite(hour_10, HIGH);  for (int n=0; n<10; n++){    write_num_to_bin(n);    delay(400);  }  digitalWrite(hour_10, LOW);  delay(2);    digitalWrite(hour_1, HIGH);  for (int n=0; n<10; n++){    write_num_to_bin(n);    delay(400);  }  digitalWrite(hour_1, LOW);  delay(2);    digitalWrite(colon, HIGH);  delay(1000);  digitalWrite(colon, LOW);  delay(2);    digitalWrite(min_10, HIGH);  for (int n=0; n<10; n++){    write_num_to_bin(n);    delay(400);  }  digitalWrite(min_10, LOW);  delay(2);    digitalWrite(min_1, HIGH);  for (int n=0; n<10; n++){    write_num_to_bin(n);    delay(400);  }  digitalWrite(min_1, LOW);  delay(2);    /*digitalWrite(A, LOW);  digitalWrite(B, LOW);  digitalWrite(C, LOW);  digitalWrite(D, LOW);  delay(500);    digitalWrite(A, HIGH);  digitalWrite(B, LOW);  digitalWrite(C, LOW);  digitalWrite(D, LOW);  delay(500);    digitalWrite(A, LOW);  digitalWrite(B, HIGH);  digitalWrite(C, LOW);  digitalWrite(D, LOW);  delay(500);    digitalWrite(A, HIGH);  digitalWrite(B, HIGH);  digitalWrite(C, LOW);  digitalWrite(D, LOW);  delay(500);*/  }void write_num_to_bin(int n){  digitalWrite(A, LOW);  digitalWrite(B, LOW);  digitalWrite(C, LOW);  digitalWrite(D, LOW);  delay(2);    int A_activation = n % 2;  n /= 2;  int B_activation = n % 2;  n /= 2;  int C_activation = n % 2;  n /= 2;  int D_activation = n % 2;    Serial.print(D_activation);  Serial.print(C_activation);  Serial.print(B_activation);  Serial.println(A_activation);    digitalWrite(A, A_activation);  digitalWrite(B, B_activation);  digitalWrite(C, C_activation);  digitalWrite(D, D_activation);}