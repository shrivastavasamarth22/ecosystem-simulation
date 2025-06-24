#ifndef CAMERA_H
#define CAMERA_H

#include <SFML/Graphics.hpp>

class Camera {
public:
    Camera(unsigned int window_width, unsigned int window_height, int world_width, int world_height, int tile_size);

    void handleEvent(const sf::Event& event, sf::RenderWindow& window);
    void update(float delta_time);
    void reset();
    void pan(const sf::Vector2f& delta);

    const sf::View& getView() const;
    bool isDragging() const;

private:
    void initialize(unsigned int window_width, unsigned int window_height, int world_width, int world_height, int tile_size);
    void updateView();
    void constrain();
    void smoothZoom(float delta_time);

    sf::View m_view;
    float m_zoom_level;
    float m_target_zoom_level;
    sf::Vector2f m_center;
    bool m_is_dragging;
    sf::Vector2i m_last_mouse_pos;
    sf::FloatRect m_world_bounds;
    float m_min_zoom;
    float m_max_zoom;
    unsigned int m_window_width;
    unsigned int m_window_height;
};

#endif // CAMERA_H
