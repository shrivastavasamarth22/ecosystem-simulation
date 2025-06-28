#ifndef CAMERA_H
#define CAMERA_H

#include <SFML/Graphics.hpp>
#include <cstddef> // For size_t

class EntityManager; // Forward declaration

enum class CameraMode {
    NORMAL,
    ENTITY_FOLLOW
};

class Camera {
public:
    Camera(unsigned int window_width, unsigned int window_height, int world_width, int world_height, int tile_size);

    void handleEvent(const sf::Event& event, sf::RenderWindow& window, const EntityManager* entityManager = nullptr);
    void update(float delta_time);
    void updateFollowMode(const EntityManager* entityManager);
    void reset();
    void pan(const sf::Vector2f& delta);

    const sf::View& getView() const;
    bool isDragging() const;
    
    // Entity selection methods
    size_t getSelectedEntity() const;
    bool hasSelectedEntity() const;
    void clearSelection();
    
    // Camera mode methods
    CameraMode getMode() const;
    void setFollowTarget(size_t entity_id);

private:
    void initialize(unsigned int window_width, unsigned int window_height, int world_width, int world_height, int tile_size);
    void updateView();
    void constrain();
    void smoothZoom(float delta_time);
    void smoothPan(float delta_time);
    size_t findEntityAtPosition(const sf::Vector2f& world_pos, const EntityManager* entityManager) const;
    sf::Vector2f calculateFollowPosition(const EntityManager* entityManager) const;

    sf::View m_view;
    float m_zoom_level;
    float m_target_zoom_level;
    sf::Vector2f m_center;
    sf::Vector2f m_target_center;
    bool m_is_dragging;
    sf::Vector2i m_last_mouse_pos;
    sf::FloatRect m_world_bounds;
    float m_min_zoom;
    float m_max_zoom;
    unsigned int m_window_width;
    unsigned int m_window_height;
    int m_tile_size;
    
    // Camera mode and movement
    CameraMode m_mode;
    float m_follow_zoom_level;
    
    // Entity selection state
    size_t m_selected_entity;
    bool m_has_selection;
    static const size_t INVALID_ENTITY = static_cast<size_t>(-1);
};

#endif // CAMERA_H
