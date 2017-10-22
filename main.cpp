#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <cmath>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

float getAngle(sf::Vector2f &orig, sf::Vector2i &des) {
    return std::atan2(des.y - orig.y, des.x - orig.x)*180/(M_PI);
}

enum EState {
    spawning, waiting, touching, bouncing
};

struct BallWithDirect : public sf::CircleShape{
    BallWithDirect(float rad): sf::CircleShape(rad){
        dir.x = 0;
        dir.y = 0;
    }
    sf::Vector2f dir;
};

struct BoxWithPoints : public sf::RectangleShape {
    BoxWithPoints(sf::Vector2f size, int p) : sf::RectangleShape(size){
        points = p;
    }
    int points;   
};

float distance(const sf::Vector2f &orig, const sf::Vector2f &des) {
    return std::sqrt(std::pow(std::abs(des.x-orig.x), 2) + std::pow(std::abs(des.y-orig.y), 2));
}

bool ballInBox(sf::RectangleShape& box, BallWithDirect& ball){
    float ballRadius = ball.getRadius();
    auto boxBounds = box.getGlobalBounds();
    auto ballPos = ball.getPosition();
    
    sf::Vector2f point = sf::Vector2f(ballPos.x + ballRadius, ballPos.y);
    if(point.x > boxBounds.left && point.x < (boxBounds.left + boxBounds.width)
       && point.y > boxBounds.top && point.y < (boxBounds.top + boxBounds.height)) {
        ball.dir.x *= -1;
        return true;
        }
    point = sf::Vector2f(ballPos.x - ballRadius, ballPos.y);
    if(point.x > boxBounds.left && point.x < (boxBounds.left + boxBounds.width)
       && point.y > boxBounds.top && point.y < (boxBounds.top + boxBounds.height)) {
        ball.dir.x *= -1;
        return true;
        }
    point = sf::Vector2f(ballPos.x, ballPos.y + ballRadius);
    if(point.x > boxBounds.left && point.x < (boxBounds.left + boxBounds.width)
       && point.y > boxBounds.top && point.y < (boxBounds.top + boxBounds.height)) {
        ball.dir.y *= -1;
        return true;
        }
    point = sf::Vector2f(ballPos.x, ballPos.y - ballRadius);
    if(point.x > boxBounds.left && point.x < (boxBounds.left + boxBounds.width)
       && point.y > boxBounds.top && point.y < (boxBounds.top + boxBounds.height)) {
        ball.dir.y *= -1;
        return true;
        }
    return false;
    
}

bool isColision_andBounce(sf::RectangleShape& box, BallWithDirect& ball){
    float ballRadius = ball.getRadius();
    auto boxBounds = box.getGlobalBounds();
    
    if(ballInBox(box, ball)){
        return true;
    }
    
    //box in ball
    sf::Vector2f point = sf::Vector2f(boxBounds.left, boxBounds.top);
    if(distance(point, ball.getPosition()) < ballRadius) {
        ball.dir.x *= -1;
        ball.dir.y *= -1;
        return true;
    }
    point = sf::Vector2f(boxBounds.left +boxBounds.width, boxBounds.top);
    if(distance(point, ball.getPosition()) < ballRadius) {
        ball.dir.x *= -1;
        ball.dir.y *= -1;
        return true;
    }
    point = sf::Vector2f(boxBounds.left, boxBounds.top + boxBounds.height);
    if(distance(point, ball.getPosition()) < ballRadius) {
        ball.dir.x *= -1;
        ball.dir.y *= -1;
        return true;
    }
    point = sf::Vector2f(boxBounds.left + boxBounds.width, boxBounds.top + boxBounds.height);
    if(distance(point, ball.getPosition()) < ballRadius)  {
        ball.dir.x *= -1;
        ball.dir.y *= -1;
        return true;
    }
    
    return false;
}

int main(){
    
    sf::Music music;
    if (!music.openFromFile("music.ogg")) {
        std::cout << "music failed" << std::endl;
    }
    music.setLoop(true);         // make it loop
    music.play();
    
    srand (time(NULL));

    sf::Vector2u size(400,700);
    sf::RenderWindow window(sf::VideoMode(size.x,size.y), "bong", sf::Style::Close);
    

    sf::Event event;
    sf::Clock deltaClock;
    
    std::cout << "-> waiting" << std::endl;
    EState state = waiting;
    
    float speed = size.x/50.0f;
    float deltatime = 0.0f;
    float shootingAngle = 0.0f;
    float timeBetweenShoots = 0.0f;
    float timeNeededToShoot = 0.06f;
    float boxSize = size.x / 8.0f;
    float ballRadius = boxSize/6.0f;
    
    std::vector<BallWithDirect> balls;
    std::vector<BoxWithPoints> boxes;
    
    sf::RectangleShape shooterGuide(sf::Vector2f(size.x/2.0f,size.x/100.0f));
    shooterGuide.setOrigin(0,size.x/100.0f);
    sf::Vector2f shooterPosition(size.x/2.0f,size.y - ballRadius);

    
    int ballsToShoot = 2;
    int ballsShooted = 0;
    
    float timer = 0.0f; 
    while(window.isOpen()){

            while(window.pollEvent(event)){
                    switch (event.type){
                            case sf::Event::Closed:
                                    window.close();
                                    break;
                            case  sf::Event::KeyPressed:
                                    if (event.key.code == sf::Keyboard::Escape) {
                                            window.close();
                                    }
                                    if (event.key.code == sf::Keyboard::Q) {
                                            window.close();
                                    }
                                    break;
                            default:
                                    break;
                    }
            }	
            
            deltatime = deltaClock.restart().asSeconds();          
            timer += deltatime;
            if(timer <= 1.0f/60.0f) continue;
            
            switch(state){
                case spawning:{
                    std::cout << "-> waiting" << std::endl;
                    state = waiting;
                    
                    for (auto box = boxes.end() - 1; box != boxes.begin() - 1; box--){
                        
                        box->move(0,boxSize);
                        
                        if(box->getPosition().y > 8*boxSize){
                            box = boxes.erase(box);
                            std::cout << "erase box but you should have died" << std::endl;
                        }   
                    }

                    
                    for(int i = 0; i < 8; ++i){
                        if(std::rand()%2){
                            int points = 5;
                            BoxWithPoints box(sf::Vector2f(boxSize, boxSize), points);
                            box.setPosition(sf::Vector2f(boxSize*i, 0));
                            box.points = 10;
                            boxes.push_back(box);
                        }
                    }
                }
                    break;
                case waiting:{
                    if(sf::Mouse::isButtonPressed(sf::Mouse::Left)){
                        std::cout << "-> touching" << std::endl;
                        state = touching;
                    }
                }
                    break;
                case touching:{
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    shootingAngle = getAngle(shooterPosition, mousePos);
                    
                    if(!sf::Mouse::isButtonPressed(sf::Mouse::Left) && (std::abs(shootingAngle) > 20 && std::abs(shootingAngle) < 160)){
                        std::cout << "-> bouncing" << std::endl;
                        state = bouncing;
                        ballsShooted = 0;
                        timeBetweenShoots = timeNeededToShoot;
                    }      
                    
                    shooterGuide.setPosition(shooterPosition);
                    shooterGuide.setRotation(shootingAngle);
                    std::cout << "=D " << shootingAngle << std::endl;
                }
                    break;
                case bouncing:{
                    timeBetweenShoots += 1.0f/60.0f;
                    if(ballsShooted <= ballsToShoot && timeBetweenShoots >= timeNeededToShoot){
                        BallWithDirect newBall(ballRadius);
                        newBall.setPosition(shooterPosition);
                        sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
                        newBall.dir.x = std::cos(shootingAngle/180*(M_PI));
                        newBall.dir.y = std::sin(shootingAngle/180*(M_PI));
                        balls.push_back(newBall);
                        std::cout << "newball dir: "<< newBall.dir.x << "," << newBall.dir.y << std::endl;
                        ++ballsShooted;
                        timeBetweenShoots = 0;
                    }
                    
                    for (auto ball = balls.end() - 1; ball != balls.begin() - 1; ball--){
                        
                        auto oldPosition = ball->getPosition();
                        ball->move(speed * ball->dir.x, speed * ball->dir.y);
                        if(ball->getPosition().x > size.x){
                            ball->dir.x *= -1;
                            std::cout << "x" << std::endl;
                        }
                        if(ball->getPosition().x < 0){
                            ball->dir.x *= -1;
                            std::cout << "x<0" << std::endl;
                        }
                        if(ball->getPosition().y < 0){
                            ball->dir.y *= -1;
                            std::cout << "ytop" << std::endl;
                        }
                        ball->setPosition(oldPosition);
                        ball->move(speed * ball->dir.x, speed * ball->dir.y);
                        
                        for (auto box = boxes.end() - 1; box != boxes.begin() - 1; box--){
                            if(isColision_andBounce(*box, *ball)){
                                box->points -= 1;
                                std::cout << "*" << std::endl;
                            }
                            if(box->points <= 0){
                                box = boxes.erase(box);
                                std::cout << "killed erase box" << std::endl;
                            }   
                        }
                        
                        if(ball->getPosition().y > size.y+ballRadius){
                            ball = balls.erase(ball);
                            std::cout << "erase" << std::endl;
                        }   
                        
                    }
                    
                    if(balls.size() == 0){
                            std::cout << "-> spawning" << std::endl;
                        state = spawning;
                    }
                    
                }
                    break;
                    
            }
            
            window.clear();

            window.draw(shooterGuide);
            for(auto ball : balls){                        
                window.draw(ball);
            }

            for(auto box : boxes){                        
                box.setFillColor(sf::Color(0.0f+10.0f*box.points,0.0f+10.0f*box.points,0.0f+10.0f*box.points));
                window.draw(box);
            }
            
            window.display();
    }
}
