#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include <iostream>

// Function to calculate angle between two points
float calculateAngle(float x1, float y1, float x2, float y2)
{
    return atan2(y2 - y1, x2 - x1) * 180 / M_PI;
}

// Function to set origin of sprite to its center
void setOriginToCenter(sf::Sprite& sprite)
{
    sf::FloatRect bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
}

int main()
{
    // Create the main window
    sf::RenderWindow window(sf::VideoMode(1000, 800), "SFML Game Test");

    // Load background texture
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("../grass.png")) {
        // Error loading texture
        return -1;
    }

    // Create background sprite
    sf::Sprite background(backgroundTexture);

    // Scale the background sprite to fit the window size
    background.setScale(
            static_cast<float>(window.getSize().x) / background.getLocalBounds().width,
            static_cast<float>(window.getSize().y) / background.getLocalBounds().height
    );

    // Load sprite sheets for movement, idle, and reloading animations
    std::vector<sf::Texture> movementTextures;
    std::vector<sf::Texture> idleTextures;
    std::vector<sf::Texture> reloadingTextures;
    for (int i = 1; i <= 4; ++i) {
        sf::Texture movementTexture;
        sf::Texture idleTexture;
        sf::Texture reloadingTexture;
        if (!movementTexture.loadFromFile("../move/survivor-move_rifle_" + std::to_string(i) + ".png")) {
            // Error loading texture
            return -1;
        }
        if (!idleTexture.loadFromFile("../idle/survivor-idle_rifle_" + std::to_string(i) + ".png")) {
            // Error loading texture
            return -1;
        }
        if (!reloadingTexture.loadFromFile("../reload/survivor-reload_rifle_" + std::to_string(i) + ".png")) {
            // Error loading texture
            return -1;
        }
        movementTextures.push_back(movementTexture);
        idleTextures.push_back(idleTexture);
        reloadingTextures.push_back(reloadingTexture);
    }

    // Create a player sprite
    sf::Sprite player;
    player.setScale(0.5f, 0.5f); // Set scale to 50% of original size

    // Load the first movement texture to get the size
    player.setTexture(movementTextures[0]);

    // Center the player sprite based on the size of the image
    setOriginToCenter(player);

    // Set initial position of the player
    player.setPosition(window.getSize().x / 2, window.getSize().y / 2);

    int currentFrame = 0; // Current frame of animation
    bool isMoving = false; // Flag to indicate if player is moving
    bool isReloading = false; // Flag to indicate if player is reloading
    sf::Clock reloadTimer; // Timer for reload animation

    // Set the movement speed of the player
    float movementSpeed = 0.05f;

    // Vector to hold bullets
    std::vector<sf::CircleShape> bullets;

    // Vector to hold direction of bullets
    std::vector<sf::Vector2f> bulletDirections;

    // Aiming variables
    bool isAiming = false;
    sf::View normalView = window.getDefaultView();
    sf::View zoomedView = normalView;
    zoomedView.zoom(2.0f); // Zoom factor for aiming

    // Main loop
    while (window.isOpen())
    {
        // Process events
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R)
            {
                isReloading = true;
                reloadTimer.restart(); // Restart the reload timer
            }
            if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::R)
                isReloading = false;

            // Shoot bullet when left mouse button is pressed
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
            {
                // Get mouse position relative to the window
                sf::Vector2i mousePosition = sf::Mouse::getPosition(window);

                // Calculate angle between player and mouse
                float angle = calculateAngle(player.getPosition().x, player.getPosition().y, mousePosition.x, mousePosition.y);

                // Store direction vector
                sf::Vector2f direction(cos(angle * M_PI / 180), sin(angle * M_PI / 180));
                bulletDirections.push_back(direction);

                // Create bullet in the direction of the mouse
                sf::CircleShape bullet(4); // Circle bullet with radius 2
                bullet.setFillColor(sf::Color::Yellow);
                bullet.setOrigin(4, 4); // Set origin to center
                bullet.setPosition(player.getPosition()); // Set position to player's position
                bullets.push_back(bullet);
            }

            // Toggle aiming when right mouse button is pressed
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right)
            {
                isAiming = true;
                window.setView(zoomedView);
            }

            // Return to normal view when right mouse button is released
            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Right)
            {
                isAiming = false;
                window.setView(normalView);
            }
        }

        // Get mouse position relative to the window
        sf::Vector2i mousePosition = sf::Mouse::getPosition(window);

        // Calculate angle between player and mouse
        float angle = calculateAngle(player.getPosition().x, player.getPosition().y, mousePosition.x, mousePosition.y);

        // Rotate player sprite to face the mouse
        player.setRotation(angle);

        // Handle player movement and animate sprite
        if (!isReloading) {
            // Handle vertical movement
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && player.getPosition().y > 0) {
                player.setTexture(movementTextures[currentFrame]);
                player.move(0, -movementSpeed);
                currentFrame = (currentFrame + 1) % movementTextures.size();
                isMoving = true;
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && player.getPosition().y < window.getSize().y) {
                player.setTexture(movementTextures[currentFrame]);
                player.move(0, movementSpeed);
                currentFrame = (currentFrame + 1) % movementTextures.size();
                isMoving = true;
            }

            // Handle horizontal movement (strafing)
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && player.getPosition().x > 0) {
                player.setTexture(movementTextures[currentFrame]);
                player.move(-movementSpeed, 0);
                currentFrame = (currentFrame + 1) % movementTextures.size();
                isMoving = true;
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) && player.getPosition().x < window.getSize().x) {
                player.setTexture(movementTextures[currentFrame]);
                player.move(movementSpeed, 0);
                currentFrame = (currentFrame + 1) % movementTextures.size();
                isMoving = true;
            }

            if (!isMoving) {
                // Player is idle
                player.setTexture(idleTextures[currentFrame]);
            }
        }
        else {
            // Player is reloading
            if (reloadTimer.getElapsedTime().asSeconds() >= 0.1f) { // Change the duration to 0.1 second
                isReloading = false;
            }
            else {
                player.setTexture(reloadingTextures[currentFrame]);
                currentFrame = (currentFrame + 1) % reloadingTextures.size();
            }
        }

        // Update bullets
        for (size_t i = 0; i < bullets.size(); ++i)
        {
            float bulletSpeed = 1.0f;
            bullets[i].move(bulletDirections[i] * bulletSpeed);
        }

        // Clear the window
        window.clear();

        // Draw the background
        window.draw(background);

        // Draw bullets
        for (const auto& bullet : bullets)
        {
            window.draw(bullet);
        }

        // Draw the aiming line (dotted line)
        sf::VertexArray line(sf::LinesStrip);
        line.append(sf::Vertex(player.getPosition(), sf::Color::Red));
        line.append(sf::Vertex(sf::Vector2f(mousePosition.x, mousePosition.y), sf::Color::Red));
        for (int i = 0; i < line.getVertexCount(); i += 2) {
            line[i].position += sf::Vector2f(cos(angle * M_PI / 180) * 10 * i, sin(angle * M_PI / 180) * 10 * i);
            line[i + 1].position += sf::Vector2f(cos(angle * M_PI / 180) * 10 * (i + 1), sin(angle * M_PI / 180) * 10 * (i + 1));
        }
        window.draw(line);

        // Draw the player sprite
        window.draw(player);

        // Display the content
        window.display();
    }

    return 0;
}
