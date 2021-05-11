#include "ResourceManager.h"

ResourceManager::ResourceManager() {
    //load textures
    std::cout << "Loading resources..images";    
    if (!carTexture.loadFromFile("Resources/cars.png")) {
        std::cout << "Could not get car (01) texture." << std::endl;
    }
    if (!driftTexture.loadFromFile("Resources/drift.png")) {
        std::cout << "C1ould not get drift texture." << std::endl;
    }
    if (!trackTexture.loadFromFile("Resources/tracksheet.png")) {
        std::cout << "Could not get tracksheet.png.\n";
    }
    if (!checkpointTexture.loadFromFile("Resources/checkpoints.png")) {
        std::cout << "Could not get checkpoints.png";
    }
    if (!ddArrowTexture.loadFromFile("Resources/ddArrow.png")) {
        std::cout << "Could not get checkpoints.png";
    }
    std::cout << "fonts..";
    
    //load fonts
    if (!consoleFont.loadFromFile("Resources/Roboto-Black.ttf")) {
        std::cout << "Could not get Console Font." << std::endl;
    }
    if (!pixelFont.loadFromFile("Resources/DotGothic.ttf")) {
        std::cout << "Could not get Pixel Font (DotGothic)" << std::endl;
    }   
    if (!robotoLight.loadFromFile("Resources/Roboto-Light.ttf")) {
        std::cout << "Could not get Roboto Light" << std::endl;
    }
    if (!robotoRegular.loadFromFile("Resources/Roboto-Medium.ttf")) {
        std::cout << "Could not get Roboto Medium" << std::endl;
    }

    //load cursors
    std::cout << "cursors..";
    if (!handCursor.loadFromSystem(sf::Cursor::Hand)) {
        std::cout << "Could not load hand cursor.";
    }
    if (!arrowCursor.loadFromSystem(sf::Cursor::Arrow)) {
        std::cout << "Could not load arrow cursor.";
    }
    if (!textCursor.loadFromSystem(sf::Cursor::Text)) {
        std::cout << "Could not load text cursor.";
    }

    std::cout << "done." << std::endl;

}