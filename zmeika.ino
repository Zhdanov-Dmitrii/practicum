#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <EEPROM.h>

Adafruit_PCD8544 display = Adafruit_PCD8544(9, 10, 11, 12, 13);

#define buttonA 2
#define buttonB 3
#define buttonC 4
#define buttonD 5
#define buttonF 7

int width = display.width();
int height = display.height();
int abcd = 1; //0 - вверх, 1 = вправо, 2 - вниз, 3 - влево
int hobj = 6, wobj = 6; //размеры объекта
int n, m;  //размеры поля
char p[28][16];
char v = ' ', s = 'S', e = '@', f = 'X';
char Max = 50;//цель игры
char Sx[50], Sy[50];         //точки Змеи
int Size = 1;
int x = 0, y = 0;   //местоположение змеи
int xw = 0, yw = 0; //местоположение верхнего левого угла окна
bool victory = false, defeat = false;

char difficulty = 1, zoom = 2, sspeed = 0;
int Speed = 1000;

char names[9];
word records[3];


char Rand(int x, int y)
{
  double t = sqrt(x * x + y * y);
  double pv, pe, pf;
  pf = pow(1.11, t);
  pe = sqrt(70 - t) * 0.9;

  switch (difficulty)
  {
    case 0: pf += 5; break;
    case 1: pf += 10; break;
    case 2: pf += 15; break;
  }


  int p = rand() % 100;
  if (p < pe)
    return e;
  if (p < pf)
    return f;
  return v;
}

void showS()
{
  for (int i = 0; i < Size; i++)
  {
    int xp = Sx[i] - xw, yp = Sy[i] - yw;
    if (xp > 0 && xp < n && yp > 0 && yp < m)
      p[xp][yp] = s;
  }
}

//сдвиг вверх
void shiftUp()
{
  for (int i = n - 1; i > 0; i--)
    for (int j = 0; j < m; j++)
      p[i][j] = p[i - 1][j];

  xw--;
  for (int i = 0; i < m; i++)
    p[0][i] = Rand(xw, yw + i);

  showS();
}

//сдвиг вниз
void shiftDown()
{
  for (int i = 0; i < n - 1; i++)
    for (int j = 0; j < m; j++)
      p[i][j] = p[i + 1][j];
  xw++;
  for (int i = 0; i < m; i++)
    p[n - 1][i] = Rand(xw, yw + i);

  showS();
}

//сдвиг влево
void lshift()
{
  for (int i = 0; i < n; i++)
    for (int j = m - 1; j > 0; j--)
      p[i][j] = p[i][j - 1];

  yw--;
  for (int i = 0; i < n; i++)
    p[i][0] = Rand(xw + i, yw);

  showS();
}

//сдвиг вправо
void rshift()
{
  for (int i = 0; i < n; i++)
    for (int j = 0; j < m - 1; j++)
      p[i][j] = p[i][j + 1];

  yw++;
  for (int i = 0; i < n; i++)
    p[i][m - 1] = Rand(xw + i, yw);

  showS();
}

void drawS(int x, int y)
{
  x *= hobj;
  y *= wobj;

  display.fillRect(x, y, hobj, wobj, 1);
}

void drawE(int x, int y)
{
  x *= hobj;
  y *= wobj;

  display.drawCircle((x + x + hobj - 1) / 2, (y + y + wobj - 1) / 2, hobj / 2, 1);
}

void drawF(int x, int y)
{
  x = x * hobj;
  y = y * wobj;

  display.drawRect(x, y, hobj, wobj, 1);
}

void show()
{
  display.clearDisplay();
  for (int i = 0; i < n - 1; i++)
  {
    for (int j = 0; j < m - 1; j++)
    {
      if (p[i][j] == s)
        drawS(i, j);
      else if (p[i][j] == e)
        drawE(i, j);
      else if (p[i][j] == f)
        drawF(i, j);
    }
  }
  display.display();
}

bool sraw(char a[], char b[])
{
  for (byte i = 0; i < 3; i++)
    if (a[i] != b[i])
      return false;
  return true;
}

void swap(char a[], char b[])
{
  for (byte i = 0; i < 3; i++)
  {
    char t = a[i];
    a[i] = b[i];
    b[i] = t;
  }
}

void showName(char Name[])
{
  if (victory)
    display.println("Victory");
  else
    display.println("Defeat");

  display.print("your size is");
  display.println(Size);
  display.println("your name is");
  display.print(Name[0]);
  display.print(Name[1]);
  display.println(Name[2]);
}

void gameOver()
{
  display.clearDisplay();
  display.setTextSize(1);  // установка размера шрифта
  display.setTextColor(BLACK); // установка цвета текста
  display.setCursor(0, 0); // установка позиции курсора

  char Name[3] = { 'a', 'a', 'a' };
  showName(Name);
  display.println("^");
  byte t1 = 0, t2 = 0, t3 = 0;

rname1:
  delay(500);
  while (digitalRead(buttonB) != LOW)
  {
    if (digitalRead(buttonC) == LOW)
      t1 = (t1 + 1) % 26;
    if (digitalRead(buttonA) == LOW)
      t1 = ((t1 - 1) + 26) % 26;
    Name[0] = 'a' + t1;
    display.clearDisplay();

    showName(Name);
    display.println("^");
    display.display();
    delay(300);
  }

rname2:
  delay(500);
  while (digitalRead(buttonB) != LOW)
  {
    if (digitalRead(buttonD) == LOW)
      goto rname1;
    if (digitalRead(buttonC) == LOW)
      t2 = (t2 + 1) % 26;
    if (digitalRead(buttonA) == LOW)
      t2 = ((t2 - 1) + 26) % 26;
    Name[1] = 'a' + t2;
    display.clearDisplay();

    showName(Name);
    display.println(" ^");
    display.display();
    delay(300);
  }

  delay(500);
  while (digitalRead(buttonB) != LOW)
  {
    if (digitalRead(buttonD) == LOW)
      goto rname2;
    if (digitalRead(buttonC) == LOW)
      t3 = (t3 + 1) % 26;
    if (digitalRead(buttonA) == LOW)
      t3 = ((t3 - 1) + 26) % 26;
    Name[2] = 'a' + t3;
    display.clearDisplay();

    showName(Name);
    display.println("  ^");
    display.display();
    delay(300);
  }

  bool b = false; //есть ли уже в таблице
  for (byte i = 0; i < 3; i++) //если имя есть в таблице увеличиваем его значение
    if (sraw(names + i * 3, Name))
    {
      b = true;
      records[i] += Size;
      break;
    }

  if (!b)
    if (Size > records[2])
    {
      records[2] = Size;
      swap(names + 6, Name);
    }

  for (byte i = 0; i < 3; i++)
    for (byte j = 2; j >= (i + 1); j--)
      if (records[j] > records[j - 1])
      {
        word t = records[j];
        records[j] = records[j - 1];
        records[j - 1] = t;

        swap(names + j * 3, names + (j - 1) * 3);
      }
  for (int i = 0; i < 9; i++)
    EEPROM.write(4 + i, names[i]);
  for (int i = 0; i < 3; i++)
    eeprom_write_word(13 + 2 * i, records[i]);
}

void setting()
{
  //Настройка сложности
setting1:
  while (digitalRead(buttonB) != LOW)
  {
    display.clearDisplay();
    display.setTextSize(1);  // установка размера шрифта
    display.setTextColor(BLACK); // установка цвета текста
    display.println("Difficulty");

    display.setTextSize(1);
    switch (difficulty)
    {
      case 0: display.println("Easy"); break;
      case 1: display.println("Normal"); break;
      case 2: display.println("Hard"); break;
    }
    if (digitalRead(buttonA) == LOW)
      difficulty = (((difficulty - 1) % 3) + 3) % 3;
    if (digitalRead(buttonC) == LOW)
      difficulty = (difficulty + 1) % 3;
    display.display();
    delay(300);
  }

  //Настройка зума
  delay(500);
setting2:
  while (digitalRead(buttonB) != LOW)
  {
    display.clearDisplay();
    display.println("Zoom");

    display.setTextSize(1);
    switch (zoom)
    {
      case 0: display.println("3x"); hobj = 3; wobj = 3; break;
      case 1: display.println("4x"); hobj = 4; wobj = 4; break;
      case 2: display.println("6x"); hobj = 6; wobj = 6; break;
    }
    if (digitalRead(buttonA) == LOW)
      zoom = (((zoom - 1) % 3) + 3) % 3;
    if (digitalRead(buttonC) == LOW)
      zoom = (zoom + 1) % 3;
    if (digitalRead(buttonD) == LOW)
      goto setting1;
    display.display();
    delay(300);
  }


  //Насстройка скорости
setting3:
  delay(500);
  while (digitalRead(buttonB) != LOW)
  {
    display.clearDisplay();
    display.println("Speed");

    switch (sspeed)
    {
      case 0: display.println("1 second"); Speed = 1000; break;
      case 1: display.println("0.75 second"); Speed = 750; break;
      case 2: display.println("0.5 second"); Speed = 500; break;
      case 3: display.println("0.25 second"); Speed = 250; break;
    }
    if (digitalRead(buttonA) == LOW)
      sspeed = (((sspeed - 1) % 4) + 4) % 4;
    if (digitalRead(buttonC) == LOW)
      sspeed = (sspeed + 1) % 4;
    if (digitalRead(buttonD) == LOW)
      goto setting2;
    display.display();
    delay(300);
  }

  //Настройка длины змеи
  delay(500);
  while (digitalRead(buttonB) != LOW)
  {
    display.clearDisplay();
    display.println("Max size");

    display.println((byte)Max);

    if (digitalRead(buttonA) == LOW)
      Max += 10;
    if (digitalRead(buttonC) == LOW)
      Max -= 10;
    if (digitalRead(buttonD) == LOW)
      goto setting3;

    if (Max > 50)
      Max = 10;
    if (Max <= 0)
      Max = 50;

    display.display();
    delay(300);
  }
  EEPROM.write(0, difficulty);
  EEPROM.write(1, zoom);
  EEPROM.write(2, sspeed);
  EEPROM.write(3, Max);
}

void table()
{
  display.clearDisplay();
  display.setTextSize(2);  // установка размера шрифта
  display.setTextColor(BLACK); // установка цвета текста
  display.setCursor(0, 0); // установка позиции курсора

  display.println("records");
  display.setTextSize(1);  // установка размера шрифта

  for (byte i = 0; i < 3; i++)
  {
    for (byte j = 0; j < 3; j++)
      display.print(names[3 * i + j]);
    display.print(" ");
    display.println(records[i]);
  }
  display.println("Press f to clear");
  display.display();
  delay(1500);
  
  while (digitalRead(buttonD) != LOW && digitalRead(buttonB) != LOW && digitalRead(buttonC) != LOW && digitalRead(buttonA) != LOW )
  {
    if (digitalRead(buttonF) != LOW)
    {
      //обнуление рекордов
      for (byte i = 0; i < 9; i++)
        names[i] = 'a';
      for (byte i = 0; i < 3; i++)
        records[i] = 0;

      //Запись в файл
      for (int i = 0; i < 9; i++)
        EEPROM.write(4 + i, names[i]);
      for (int i = 0; i < 3; i++)
        eeprom_write_word(13 + 2 * i, records[i]);
        
      break;
    }
    delay(200);
  }
}

void startMenu()
{
  display.clearDisplay();
  display.setTextSize(1);  // установка размера шрифта
  display.setTextColor(BLACK); // установка цвета текста
  display.setCursor(0, 0); // установка позиции курсора

  byte pos = 0; //0 - start, 1 - setting, 2 - table
  delay(500);


  while (digitalRead(buttonD) != LOW && digitalRead(buttonB) != LOW)
  {
    display.clearDisplay();
    if (digitalRead(buttonA) == LOW)
      pos = ((pos - 1) + 3) % 3;

    if (digitalRead(buttonC) == LOW)
      pos = (pos + 1) % 3;

    if (pos == 0)
      display.setTextSize(2);  // установка размера шрифта
    display.println("Start");
    display.setTextSize(1);  // установка размера шрифта

    if (pos == 1)
      display.setTextSize(2);  // установка размера шрифта
    display.println("Setting");
    display.setTextSize(1);  // установка размера шрифтa

    if (pos == 2)
      display.setTextSize(2);  // установка размера шрифта
    display.println("Table");
    display.setTextSize(1);  // установка размера шрифта

    display.display();
    delay(300);
  }

  switch (pos)
  {
    case 0: break;
    case 1: setting(); break;
    case 2: table();
  }
  n = width / wobj;
  m = height / hobj;

  for (int i = 0; i < n; i++)
    for (int j = 0; j < m; j++)
      p[i][j] = Rand(i, j);

  Size = 1;
  x = n / 2; y = m / 2;
  Sx[0] = x; Sy[0] = y;
  xw = 0; yw = 0;
  show();
  delay(1500);
}

void setup() {
  pinMode(7, OUTPUT);
  digitalWrite(7, LOW);

  Serial.begin(9600);

  display.begin();
  display.clearDisplay();
  display.display();

  display.setContrast(70); // установка контраста


  difficulty = EEPROM.read(0);
  zoom = EEPROM.read(1);
  sspeed = EEPROM.read(2);
  Max = EEPROM.read(3);

  for (int i = 0; i < 9; i++)
    names[i] = EEPROM.read(4 + i);
  for (int i = 0; i < 3; i++)
    records[i] = eeprom_read_word(13 + 2 * i);

  switch (sspeed)
  {
    case 0: Speed = 1000; break;
    case 1: Speed = 750; break;
    case 2: Speed = 500; break;
    case 3: Speed = 250; break;
  }

  switch (zoom)
  {
    case 0: display.println("3x"); hobj = 3; wobj = 3; break;
    case 1: display.println("4x"); hobj = 4; wobj = 4; break;
    case 2: display.println("6x"); hobj = 6; wobj = 6; break;
  }

  startMenu();
}


void loop() {
  if (victory || defeat)
  {
    gameOver();
    delay(500);

    victory = false;
    defeat = false;

    startMenu();

    return;
  }
  
  //Определение направеления
  if (digitalRead(buttonA) == LOW)
    abcd = 0;
  if (digitalRead(buttonB) == LOW)
    abcd = 1;
  if (digitalRead(buttonC) == LOW)
    abcd = 2;
  if (digitalRead(buttonD) == LOW)
    abcd = 3;


  //Изменение координаты
  switch (abcd)
  {
    case 3: x--; break;
    case 2: y++; break;
    case 1: x++; break;
    case 0: y--; break;
  }

  int xp = x - xw, yp = y - yw; //координаты головы на поле
  if (xp < 2)
  {
    shiftUp();
    xp++;
  }
  if (xp >= n - 2)
  {
    shiftDown();
    xp--;
  }
  if (yp < 2)
  {
    lshift();
    yp++;
  }
  if (yp >= m - 2)
  {
    rshift();
    yp--;
  }

  char pt = p[xp][yp];
  if (pt == s || pt == f)
  {
    defeat = true;
    return;
  }

  for (int i = Size; i > 0; i--)
  {
    Sx[i] = Sx[i - 1];
    Sy[i] = Sy[i - 1];
  }
  Sx[0] = x;
  Sy[0] = y;

  p[xp][yp] = s;
  if (pt != e)
  {
    int txp = Sx[Size] - xw, typ = Sy[Size] - yw;

    if (txp >= 0 && txp < n && typ >= 0 && typ < m)
      p[txp][typ] = v;
  }
  else Size++;

  if (Size == Max)
  {
    victory = true;
    return;
  }

  show();
  delay(Speed);

}
