///////////////////////////////////////////////////////////////////////
// main.cpp
///////////////////////////////////////////////////////////////////////
#include <iostream>
#include <list>

#include <cstdlib>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
///////////////////////////////////////////////////////////////////////
class Snake
{
  public:
  Snake(); //konstruktor domyślny

  void init();  //inicjalizacja gry
  void start(); //start gry

  private:
  void handleEvents(); //obsługa eventów
  void move();         //obsługa poruszania się węża
  void quit();         //awaryjne wyjście z gry

  private:
  int mLogicSpeed;        //prędkość logiki
  const int mStartLength; //startowa długość węża
  const int mBlockSize;   //rozmiar węża

  const sf::IntRect mWindowBox; //do sprawdzenia czy wąż nie uderzył w ścianę
  sf::RenderWindow mWindow;     //okno aplikacji
  sf::Event mEvent;             //event listener

  sf::Texture mTextureBackground; //textura tła
  sf::Sprite mBackground;         //sprite tła

  sf::Vector2f mLastMove;               //ostatni ruch odczytany z klawiatury
  std::list<sf::RectangleShape> mSnake; //lista kwadratów węża
  sf::RectangleShape mApple;            //jabuszko :)
};
///////////////////////////////////////////////////////////////////////
Snake::Snake() :
  mLogicSpeed(300),
  mStartLength(10),
  mBlockSize(10),
  mWindowBox(0, 0, 500, 500),
  mLastMove(-mBlockSize, 0)
{
}
///////////////////////////////////////////////////////////////////////
void Snake::init()
{
  srand(time(0));

  //ustawianie okna
  mWindow.create(sf::VideoMode(mWindowBox.width, mWindowBox.height), "Snake");
  mWindow.setKeyRepeatEnabled(true);

  //ustawianie tła
  sf::IntRect backgroundSize = mWindowBox;
  mTextureBackground.loadFromFile("background.png", backgroundSize);
  mBackground.setTexture(mTextureBackground);

  //ustawianie jabłka
  mApple.setSize(sf::Vector2f(mBlockSize, mBlockSize));
  mApple.setFillColor(sf::Color::Red);
  mApple.setPosition(rand() % (mWindowBox.width  / mBlockSize) * mBlockSize + 5,
                     rand() % (mWindowBox.height / mBlockSize) * mBlockSize);

  //ustawianie węża
  sf::RectangleShape rect;
  rect.setSize(sf::Vector2f(mBlockSize, mBlockSize));

  for(int i = 0; i < mStartLength; i++)
  {
    rect.setPosition(mWindowBox.width  / 4 + i * mBlockSize,
                     mWindowBox.height / 2);
    mSnake.push_back(rect);
  }
}
///////////////////////////////////////////////////////////////////////
void Snake::start()
{
  sf::Clock clock;
  while(mWindow.isOpen())
  {
    handleEvents();

    //stała prędkość pętli
    if(clock.getElapsedTime().asMilliseconds() > mLogicSpeed)
    {
      clock.restart();
      move();
    }

    mWindow.clear();

    mWindow.draw(mBackground); //tło
    mWindow.draw(mApple);      //jabłko
    for(auto it = mSnake.begin(); it != mSnake.end(); ++it)
      mWindow.draw(*it);       //wąż

    mWindow.display();
  }

  quit();
}
///////////////////////////////////////////////////////////////////////
void Snake::handleEvents()
{
  while(mWindow.pollEvent(mEvent))
  {
    if(mEvent.type == sf::Event::Closed)
    {
      mWindow.close();
      quit();
    }

    if(mEvent.type == sf::Event::KeyPressed)
    {
      static sf::Keyboard::Key lastPressed;

      //zabezpieczenie węża przed "wejściem w siebie"
      if(mEvent.key.code == sf::Keyboard::W && lastPressed != sf::Keyboard::S)
      {
        mLastMove.x = 0;
        mLastMove.y = -mBlockSize;
        lastPressed = mEvent.key.code;
        break;
      }

      if(mEvent.key.code == sf::Keyboard::S && lastPressed != sf::Keyboard::W)
      {
        mLastMove.x = 0;
        mLastMove.y = mBlockSize;
        lastPressed = mEvent.key.code;
        break;
      }

      if(mEvent.key.code == sf::Keyboard::A && lastPressed != sf::Keyboard::D)
      {
        mLastMove.x = -mBlockSize;
        mLastMove.y = 0;
        lastPressed = mEvent.key.code;
        break;
      }

      if(mEvent.key.code == sf::Keyboard::D && lastPressed != sf::Keyboard::A)
      {
        mLastMove.x = mBlockSize;
        mLastMove.y = 0;
        lastPressed = mEvent.key.code;
        break;
      }
    }
  }
}
///////////////////////////////////////////////////////////////////////
void Snake::move()
{
  //jeśli okno główne nie zawiera głowy węża to znaczy, że koniec gry
  if(!mWindowBox.contains(mSnake.front().getPosition().x,
                          mSnake.front().getPosition().y))
    quit();
  auto front = mSnake.front();

  //jeśli zebrane zostało jabłko, przedłuż węża
  if(front.getPosition().x == mApple.getPosition().x
  && front.getPosition().y == mApple.getPosition().y)
  {
    mSnake.push_front(front);

    //znajdź nową pozycję dla węża
    mApple.setPosition(rand() % (mWindowBox.width  / mBlockSize) * mBlockSize + 5,
                       rand() % (mWindowBox.height / mBlockSize) * mBlockSize);

    if(mLogicSpeed > 10) mLogicSpeed -= 10;
  }
  else
    mSnake.pop_back(); //usuń końcówkę węża

  front.move(mLastMove); //ustaw najnowszy odebrany kierunek
  mSnake.push_front(front); //wstaw go na początek węża
}
///////////////////////////////////////////////////////////////////////
void Snake::quit()
{
  sf::Clock clock;

  //poczekaj chwilę
  while(clock.getElapsedTime().asMilliseconds() < mLogicSpeed * 2);

  //i wyjdź z programu
  exit(1);
}
///////////////////////////////////////////////////////////////////////
int main()
{
  Snake snake;
  snake.init();
  snake.start();
}
///////////////////////////////////////////////////////////////////////
