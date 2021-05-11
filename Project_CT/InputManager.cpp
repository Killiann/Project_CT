#include "InputManager.h"

InputManager::InputManager(Trainer* t, Overlay* o) : trainer(t), overlay(o){}
void InputManager::UpdateUIControls(sf::Event event, sf::Vector2f mouseCoords) {
    if (event.type == sf::Event::MouseButtonPressed){
        //select cars
        if (event.mouseButton.button == sf::Mouse::Left)
        {
            for (unsigned int i = 0; i < trainer->GetCars().size(); ++i) {
                if (trainer->GetCars()[i].containsPoint(mouseCoords)) {
                    if (i != trainer->GetCurrentID()) {
                        trainer->GetCars()[i].Select();
                        trainer->GetCars()[trainer->GetCurrentID()].Deselect();
                        trainer->SetCurrentID(i);
                    }
                }
            }
        }
    }
    if (event.type == sf::Event::KeyPressed) {
        //cycle through cars
        if (trainer->IsRunning()) {
            if (event.key.code == sf::Keyboard::Key::Num1) {
                if (!isCycleDown) {
                    isCycleDown = true;
                    trainer->GetCars()[trainer->GetCurrentID()].Deselect();
                    if (trainer->GetCurrentID() == (trainer->GetCars().size() - 1))
                        trainer->SetCurrentID(0);
                    else trainer->SetCurrentID(trainer->GetCurrentID() + 1);
                    trainer->GetCars()[trainer->GetCurrentID()].Select();
                }
            }

            //reset scene
            if (event.key.code == sf::Keyboard::Key::Num0) {
                if (!isResetDown) {
                    trainer->ResetScene();
                    isResetDown = true;
                }
            }
            //reset scene
            if (event.key.code == sf::Keyboard::Key::Num2) {
                if (!isNextGenDown) {
                    trainer->NextGeneration(false);
                    isNextGenDown = true;
                }
            }            
            //toggle overlay
            if (event.key.code == sf::Keyboard::Key::C) {
                if (trainer->IsRunning() && !isOverlayDown) {
                    overlay->ShowHide();
                    isOverlayDown = true;
                }
            }
            //export
            if (event.key.code == sf::Keyboard::Key::X) {
                if (trainer->IsRunning() && !isExportDown) {
                    trainer->ExportData("data.csv");
                    isExportDown = true;
                }
            }
            //save scene
            if (event.key.code == sf::Keyboard::Key::S) {
                if (!isSaveDown) {
                    trainer->SaveScene("trainer.sim");
                    isSaveDown = true;
                }
            }            
            //open main menu
            if (event.key.code == sf::Keyboard::Key::Escape) {
                if (trainer->IsRunning() && !isMainMenuDown) {
                    overlay->OpenMainMenu();
                    isMainMenuDown = true;
                }
            }
        }                
    }
    if (event.type == sf::Event::KeyReleased) {
        if (event.key.code == sf::Keyboard::Key::Num1) {
            if (isCycleDown) isCycleDown = false;
        }
        if (event.key.code == sf::Keyboard::Key::Num0) {
            if (isResetDown) isResetDown = false;
        }
        if (event.key.code == sf::Keyboard::Key::Num2) {
            if (isNextGenDown) isNextGenDown = false;
        }
        if (event.key.code == sf::Keyboard::Key::S) {
            if (isSaveDown) isSaveDown = false;
        }
        if (event.key.code == sf::Keyboard::Key::C) {
            if (isOverlayDown) isOverlayDown = false;
        }
        if (event.key.code == sf::Keyboard::Key::Escape) {
            if (isMainMenuDown) isMainMenuDown = false;
        }
        if (event.key.code == sf::Keyboard::Key::X) {
            if (isExportDown) isExportDown = false;
        }
    }
}