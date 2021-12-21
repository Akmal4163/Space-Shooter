#include <iostream>
#include <vector>
#include <array>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>


struct bullet
{
	sf::Sprite bulletShape;
	sf::Texture* texture;
	
	bullet(sf::Texture *texture, sf::Vector2f position) 
	{
		this->texture = texture;
		this->bulletShape.setTexture(*texture);
		this->bulletShape.setScale(0.05f, 0.05f);
		this->bulletShape.setPosition(position);
	}
	~bullet() {}
};

struct player
{
	int HP;
	int HPMax;
	sf::Sprite playerShape;
	sf::Texture *texture;
	
	std::vector<bullet>bullets;

	player(sf::Texture *texture)
	{
		this->HPMax = 10;
		this->HP = HPMax;

		this->texture = texture;
		this->playerShape.setTexture(*texture);
		this->playerShape.setScale(0.1f, 0.1f);
	
	}
	~player(){}
};

struct enemy
{
	int HP;
	int HPMax;
	sf::Sprite enemyShape;
	sf::Texture* texture;
	
	enemy(sf::Texture *texture, sf::Vector2u windowSize) 
	{
		this->HPMax = rand() % 3 + 1;
		this->HP = this->HPMax;

		this->texture = texture;
		this->enemyShape.setTexture(*texture);
		this->enemyShape.setScale(0.1f, 0.1f);

		this->enemyShape.setPosition(windowSize.x - this->enemyShape.getGlobalBounds().width, rand()%windowSize.y - this->enemyShape.getGlobalBounds().height);
	}
	~enemy() {}
};



int main(int argc, char* argv[])
{

	srand(time(NULL));

	sf::RenderWindow window(sf::VideoMode(800, 600), "Space Shooter", sf::Style::Default);
	window.setFramerateLimit(70);

	sf::Font fonts;
	fonts.loadFromFile("fonts/Dosis-Light.ttf");

	//load texture
	sf::Texture playerTexture;
	playerTexture.loadFromFile("gfx/ship.png");

	sf::Texture enemyTexture;
	enemyTexture.loadFromFile("gfx/enemy.png");

	sf::Texture bulletTexture;
	bulletTexture.loadFromFile("gfx/missileTex01.png");

	//ui init
	sf::Text textScore;
	textScore.setFont(fonts);
	textScore.setFillColor(sf::Color::Yellow);
	textScore.setCharacterSize(40);
	textScore.setPosition(0.0f, 0.0f);

	sf::Text textGameOver;
	textGameOver.setFont(fonts);
	textGameOver.setFillColor(sf::Color::Yellow);
	textGameOver.setCharacterSize(100);
	textGameOver.setPosition(100.0f,(float)(window.getSize().y/2));
	textGameOver.setString("GAME OVER");


	//on user create
	int score = 0;
	player player(&playerTexture);
	sf::Text HPtext;
	HPtext.setFont(fonts);
	HPtext.setFillColor(sf::Color::Yellow);
	HPtext.setCharacterSize(12);

	
	int shootTimer = 15;
	int enemySpawnTimer = 20;

	//enemy init
	std::vector<enemy> enemies;
	

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
		}

		if (player.HP >= 0)
		{
			//player movement
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
				player.playerShape.move(-5.0f, 0.0f);
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
				player.playerShape.move(5.0f, 0.0f);
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
				player.playerShape.move(0.0f, -5.0f);
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
				player.playerShape.move(0.0f, 5.0f);

			HPtext.setPosition(player.playerShape.getPosition().x, player.playerShape.getPosition().y - HPtext.getGlobalBounds().height);
			HPtext.setString(std::to_string(player.HP) + "/" + std::to_string(player.HPMax));

			//player collision detection
			if (player.playerShape.getPosition().x <= 0.0f) //left
				player.playerShape.setPosition(0.0f, player.playerShape.getPosition().y);

			if (player.playerShape.getPosition().x > window.getSize().x - player.playerShape.getGlobalBounds().width) //right
				player.playerShape.setPosition(window.getSize().x - player.playerShape.getGlobalBounds().width, player.playerShape.getPosition().y);

			if (player.playerShape.getPosition().y <= 0.0f) //top
				player.playerShape.setPosition(player.playerShape.getPosition().x, 0.0f);

			if (player.playerShape.getPosition().y > window.getSize().y - player.playerShape.getGlobalBounds().height) //bottom
				player.playerShape.setPosition(player.playerShape.getPosition().x, window.getSize().y - player.playerShape.getGlobalBounds().height);


			//update shoot timer
			if (shootTimer < 20)
				shootTimer++;

			//shooting
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && shootTimer >= 15)
			{
				player.bullets.push_back(bullet(&bulletTexture, player.playerShape.getPosition()));
				shootTimer = 0;
			}

			if (enemySpawnTimer < 20)
				enemySpawnTimer++;

			if (enemySpawnTimer >= 20)
			{
				enemies.push_back(enemy(&enemyTexture, window.getSize()));
				enemySpawnTimer = 0;
			}

			//bullets
			for (auto& bulletIterator : player.bullets)
			{
				bulletIterator.bulletShape.move(25.0f, 0.0f);

				//window bullet position
				if (bulletIterator.bulletShape.getPosition().x > window.getSize().x)
					player.bullets.erase(player.bullets.begin());

				for (auto& enemyIntersectIterator : enemies)
				{
					if (bulletIterator.bulletShape.getGlobalBounds().intersects(enemyIntersectIterator.enemyShape.getGlobalBounds()))
					{
						if (enemyIntersectIterator.HP <= 1)
						{
							score += enemyIntersectIterator.HPMax;
							enemies.erase(enemies.begin());
						}

						else
							enemyIntersectIterator.HP--; //damage
						break;
					}
				}
			}

			//enemies
			for (auto& enemiesIterator : enemies)
			{
				enemiesIterator.enemyShape.move(-7.0f, 0.0f);
				if (enemiesIterator.enemyShape.getPosition().x > window.getSize().x)
					enemies.erase(enemies.begin());

				if (enemiesIterator.enemyShape.getPosition().y < 0.0f)
					enemiesIterator.enemyShape.setPosition(enemiesIterator.enemyShape.getPosition().x, window.getSize().y - enemiesIterator.enemyShape.getGlobalBounds().height);

				if (enemiesIterator.enemyShape.getPosition().y > window.getSize().y - enemiesIterator.enemyShape.getGlobalBounds().height)
					enemiesIterator.enemyShape.setPosition(enemiesIterator.enemyShape.getPosition().x, window.getSize().y - enemiesIterator.enemyShape.getGlobalBounds().height);

				if (enemiesIterator.enemyShape.getGlobalBounds().intersects(player.playerShape.getGlobalBounds()))
				{
					enemies.erase(enemies.begin());
					player.HP--;
				}

			}

			//UI update
			textScore.setString("SCORE: " + std::to_string(score));
		}

		//=====================================//

		//clear screen
		window.clear();

		
		/////////////////////////// DRAW //////////////////////////////
		//draw player
		window.draw(player.playerShape);

		//draw bullets
		for (auto& i : player.bullets)
		{
			window.draw(i.bulletShape);
		}

		//draw enemies
		for (auto& k : enemies)
		{
			HPtext.setString(std::to_string(k.HP) + "/" + std::to_string(k.HPMax));
			HPtext.setPosition(k.enemyShape.getPosition().x, k.enemyShape.getPosition().y - HPtext.getGlobalBounds().height);
			window.draw(HPtext);
			window.draw(k.enemyShape);
		}

		//UI
		window.draw(textScore);
		window.draw(HPtext);
		if (player.HP < 0)
			window.draw(textGameOver);

		window.display();
	}

	return 0;
}