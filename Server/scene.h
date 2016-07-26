#ifndef SCENE_H
#define SCENE_H

#pragma once

struct SFrame {

};

struct Scene {
    std::vector<SFrame> frames;

    Scene();
    ~Scene();

    void loadScene(const std::string p);
};

#endif
