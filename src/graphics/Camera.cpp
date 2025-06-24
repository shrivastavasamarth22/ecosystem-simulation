#include "graphics/Camera.h"
#include <algorithm>

Camera::Camera(unsigned int window_width, unsigned int window_height, int world_width, int world_height, int tile_size) {
    initialize(window_width, window_height, world_width, world_height, tile_size);
}

void Camera::initialize(unsigned int window_width, unsigned int window_height, int world_width, int world_height, int tile_size) {
    m_window_width = window_width;
    m_window_height = window_height;

    float world_pixel_width = world_width * tile_size;
    float world_pixel_height = world_height * tile_size;
    m_world_bounds = sf::FloatRect(0, 0, world_pixel_width, world_pixel_height);

    m_center = sf::Vector2f(world_pixel_width / 2.0f, world_pixel_height / 2.0f);

    float window_aspect = static_cast<float>(m_window_width) / m_window_height;
    float world_aspect = world_pixel_width / world_pixel_height;

    if (world_aspect > window_aspect) {
        m_zoom_level = world_pixel_width / m_window_width;
    } else {
        m_zoom_level = world_pixel_height / m_window_height;
    }

    m_max_zoom = m_zoom_level;
    m_min_zoom = 0.2f;
    m_target_zoom_level = m_zoom_level;
    m_is_dragging = false;

    m_view = sf::View(m_center, sf::Vector2f(m_window_width, m_window_height));
    updateView();
}

void Camera::pan(const sf::Vector2f& delta) {
    m_center += delta;
    constrain();
    updateView();
}

void Camera::handleEvent(const sf::Event& event, sf::RenderWindow& window) {
    if (event.type == sf::Event::MouseWheelScrolled) {
        if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
            float zoom_factor = (event.mouseWheelScroll.delta > 0) ? 0.9f : 1.1f;
            m_target_zoom_level *= zoom_factor;
            m_target_zoom_level = std::max(m_min_zoom, std::min(m_max_zoom, m_target_zoom_level));
        }
    }

    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            m_is_dragging = true;
            m_last_mouse_pos = sf::Mouse::getPosition(window);
        }
    }

    if (event.type == sf::Event::MouseButtonReleased) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            m_is_dragging = false;
        }
    }

    if (event.type == sf::Event::MouseMoved && m_is_dragging) {
        sf::Vector2i current_mouse_pos = sf::Mouse::getPosition(window);
        sf::Vector2f last_world_pos = window.mapPixelToCoords(m_last_mouse_pos, m_view);
        sf::Vector2f current_world_pos = window.mapPixelToCoords(current_mouse_pos, m_view);
        
        pan(last_world_pos - current_world_pos);

        m_last_mouse_pos = current_mouse_pos;
    }

    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) {
        reset();
    }
}

bool Camera::isDragging() const {
    return m_is_dragging;
}

void Camera::update(float delta_time) {
    smoothZoom(delta_time);
    constrain();
    updateView();
}

void Camera::reset() {
    m_center = sf::Vector2f(m_world_bounds.width / 2.0f, m_world_bounds.height / 2.0f);
    float window_aspect = static_cast<float>(m_window_width) / m_window_height;
    float world_aspect = m_world_bounds.width / m_world_bounds.height;

    if (world_aspect > window_aspect) {
        m_target_zoom_level = m_world_bounds.width / m_window_width;
    } else {
        m_target_zoom_level = m_world_bounds.height / m_window_height;
    }
}

const sf::View& Camera::getView() const {
    return m_view;
}

void Camera::updateView() {
    sf::Vector2f view_size(m_window_width * m_zoom_level, m_window_height * m_zoom_level);
    m_view.setSize(view_size);
    m_view.setCenter(m_center);
}

void Camera::constrain() {
    m_zoom_level = std::max(m_min_zoom, std::min(m_max_zoom, m_zoom_level));
    m_target_zoom_level = std::max(m_min_zoom, std::min(m_max_zoom, m_target_zoom_level));

    sf::Vector2f view_size(m_window_width * m_zoom_level, m_window_height * m_zoom_level);
    float half_width = view_size.x / 2.0f;
    float half_height = view_size.y / 2.0f;

    float min_x = m_world_bounds.left + half_width;
    float max_x = m_world_bounds.left + m_world_bounds.width - half_width;
    float min_y = m_world_bounds.top + half_height;
    float max_y = m_world_bounds.top + m_world_bounds.height - half_height;

    if (view_size.x >= m_world_bounds.width) {
        m_center.x = m_world_bounds.left + m_world_bounds.width / 2.0f;
    } else {
        m_center.x = std::max(min_x, std::min(max_x, m_center.x));
    }

    if (view_size.y >= m_world_bounds.height) {
        m_center.y = m_world_bounds.top + m_world_bounds.height / 2.0f;
    } else {
        m_center.y = std::max(min_y, std::min(max_y, m_center.y));
    }
}

void Camera::smoothZoom(float delta_time) {
    if (std::abs(m_zoom_level - m_target_zoom_level) > 0.001f) {
        float zoom_speed = 5.0f;
        m_zoom_level += (m_target_zoom_level - m_zoom_level) * zoom_speed * delta_time;
    }
}
