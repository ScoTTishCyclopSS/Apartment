#pragma once
#include "pgr.h"

using namespace glm;

class Light {
public:
    vec3 diffuse;
    vec3 ambient;
    vec3 specular;

    Light(vec3 dif, vec3 amb, vec3 spec) {
        diffuse = dif;
        ambient = amb;
        specular = spec;
    }
};

class PointLight : public Light {
public:
    float constant;
    float linear;
    float quadratic;

    bool blinn;
    vec3 position;
    vec3 startPosition;
    vec3 interpolateTo;

    PointLight(vec3 dif, vec3 amb, vec3 spec,
        float con, float lin, float quad, 
        bool blin, vec3 pos, vec3 inter) : Light(dif, amb, spec) {
        constant = con;
        linear = lin;
        quadratic = quad;
        blinn = blin;
        position = pos;
        interpolateTo = inter;
        startPosition = pos;
    }
};

class DirectLight : public Light {
public:
    vec3 direction;

    DirectLight(vec3 d, vec3 a, vec3 s, vec3 dir) : Light(d, a, s) {
        direction = dir;
    }
};

class SpotLight : public Light {
public:
    vec3 direction;
    vec3 position;
    float cut;

    SpotLight(vec3 d, vec3 a, vec3 s, vec3 pos, vec3 dir, float c) : Light(d, a, s) {
        direction = dir;
        position = pos;
        cut = c;
    }
};