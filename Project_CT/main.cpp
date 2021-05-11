#include <SFML/Graphics.hpp>
#include <algorithm>
#include <numeric>

#include "InputManager.h"
#include "ResourceManager.h"
#include "TrackManager.h"
#include "Trainer.h"
#include "MainMenu.h"
#include "Overlay.h"

#include "ThreadPool.h"

int main(){
    //window setup
    sf::ContextSettings settings;    
    settings.antialiasingLevel = 8;    
    sf::RenderWindow window(sf::VideoMode(1500, 900), "Racer Project_CT", sf::Style::Close, settings);                
    sf::View camera;
    window.setFramerateLimit(90);

    //main view
    camera.setSize(3000, 1800);
    camera.setCenter(1200, 700);
   
    //neural net positioning on UI
    sf::FloatRect dimensions = sf::FloatRect(
        sf::Vector2f(window.getViewport(window.getDefaultView()).width / 2 - 200, 15),
        sf::Vector2f(400, 200)
    );

    //managers 
    ResourceManager resourceManager;
    TrackManager trackManager(&resourceManager);
        
    //NN Trainers
    Trainer trainer(&resourceManager, trackManager.GetCurrentTrack(), dimensions);    

    //UI
    MainMenu menu(&resourceManager, &trainer);
    Overlay overlay(&resourceManager, &trainer, &menu);
    InputManager inputManager(&trainer, &overlay);        

    //misc
    sf::Vector2f mouseCoords;
    bool displayDev = false;
    std::deque<float> allFPS;

    //main clock
    sf::Clock clk;
    float dt;

    //initialise thread pool
    ThreadPool pool(12);
    pool.init();

    //main loop
    while (window.isOpen())
    {                
        //handle fps
        float fps = 1.f / clk.getElapsedTime().asSeconds();
        allFPS.push_back(fps);
        if (allFPS.size() > 30) allFPS.pop_front();

        //keep after fps fetch
        dt = clk.restart().asSeconds();

        if (dt > 0.05) dt = 0.05;

        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        mouseCoords = window.mapPixelToCoords(sf::Mouse::getPosition(window), camera);        

        //manage events
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::Resized)
            {
                sf::FloatRect visibleArea(0.f, 0.f, (float)event.size.width, (float)event.size.height);
                window.setView(sf::View(visibleArea));
            }                                 

            //update UI
            menu.Update(window, event);
            if(trainer.IsRunning())
                overlay.Update(window, event);

            inputManager.UpdateUIControls(event, mouseCoords);
        }
        //dev overlay
        displayDev = overlay.IsDevOn();

        //update 
        trainer.Update(dt, pool);
        std::string frames = std::to_string((int)(std::accumulate(allFPS.begin(), allFPS.end(), 0.0) / allFPS.size()));
        overlay.UpdateData(frames);

        //draw entities
        window.clear(sf::Color(139, 69, 19));
        window.setView(camera);        
        trackManager.DrawTrack(window, displayDev);
        trainer.DrawEntities(window, displayDev);

        //UI------- 
        window.setView(window.getDefaultView());        
        trainer.DrawUI(window);        
        menu.Draw(window);        
        if (trainer.IsRunning())
            overlay.Draw(window);

        window.display();
    }
    pool.shutdown();
    return 0;
}