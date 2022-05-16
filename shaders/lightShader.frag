#version 330

struct Fog {
    vec3 color;
    float start, end, density;
    int type;
    bool isActive;
};

struct PointLight {
    vec3 diffuse, ambient, specular, position;          
    float constant, linear, quadratic; 
    bool blinn;
};

struct SpotLight {
    vec3 diffuse, ambient, specular, direction, position;
    float cut;
    bool isActive;
};


struct DirectLight {
    vec3 diffuse, ambient, specular, direction;
};

#define LIGHTS_COUNT 4

uniform PointLight pointLights [LIGHTS_COUNT];
uniform DirectLight directLight;
uniform SpotLight spotLight;
uniform Fog fog;

uniform sampler2D textureSampler;
uniform sampler2D normalSampler;
uniform sampler2D roughSampler;
uniform vec3 viewPos;

uniform bool skipTangent;
uniform bool useDirLight;
uniform bool combinedTexture;
uniform bool lampIsOn;

in mat3 TBN;
in vec3 position_v;
in vec2 texCoord_v;
in vec3 normal_v;
in float t_v;

out vec4 fragmentColor;

int frame;

// functions
vec3 AddPointLight(PointLight plight, vec3 normal, vec3 viewDir);
vec3 AddSpotLight();
vec3 AddDirectLight(vec3 normal);

float calculateFogFactor(float coords);

vec4 sampleTexture()
{
    ivec2 pattern = ivec2(3, 1); // 3 frames in a row (i'm lazy, thx)
    int u = frame % pattern.x;
    int v = frame / pattern.x;
    return texture(textureSampler, vec2((texCoord_v.x + u) * 1/pattern.x, (texCoord_v.y + v) * 1/pattern.y));
}

void main()
{       
    frame = int(t_v / 0.7f);

    if(texture(textureSampler, texCoord_v).a < 0.1)
        discard;

    vec3 result = vec3(0.0);
    vec3 viewDir = normalize(viewPos - position_v);
    vec3 normal = texture2D(normalSampler, texCoord_v).xyz;
    normal = normal * 2.0 - 1.0;
    normal = normalize(TBN * normal);
    if(skipTangent)
        normal = normalize(normal_v);

    if(useDirLight)
        result += AddDirectLight(normal);

    int lightsToUse = lampIsOn ? LIGHTS_COUNT : LIGHTS_COUNT - 1;
    for (int i = 0; i < lightsToUse; i++)
       result += AddPointLight(pointLights[i], normal, viewDir);

    if(spotLight.isActive)
       result += AddSpotLight();

    fragmentColor = vec4(result, 1.0f);

    // fooooooooooog (not frog!)
    if(fog.isActive)
        fragmentColor = mix(fragmentColor, vec4(fog.color, 1.0), calculateFogFactor(abs(viewPos.z)));
}

vec3 AddDirectLight(vec3 normal) 
{
    float dotL = max(dot(normal, -directLight.direction), 0.0);

    vec3 ambient  = directLight.ambient * vec3(texture(textureSampler, texCoord_v));
    vec3 diffuse  = directLight.diffuse * vec3(texture(textureSampler, texCoord_v));
    vec3 specular = directLight.specular * vec3(texture(roughSampler, texCoord_v));

    // if texture is a couple of frames
    if(combinedTexture) 
    {
        ambient = directLight.ambient * vec3(sampleTexture());
        diffuse = directLight.diffuse  * vec3(sampleTexture());
    }

    ambient *= dotL;
    diffuse *= dotL;
    specular *= dotL;

    return (ambient + diffuse + specular) * 0.03;
}

vec3 AddSpotLight() 
{
    vec3 lightDir = normalize(spotLight.position - position_v);
    float theta = dot(lightDir, normalize(-spotLight.direction)); // spot vector
    
    if(theta > spotLight.cut)
    {
        vec3 color = spotLight.diffuse;
        float intensity = (1.0 - theta) / (1.0 - spotLight.cut);
        intensity = 1.0 - intensity;
        return color * intensity;
    }
    return vec3(0.0);
}

vec3 AddPointLight(PointLight plight, vec3 normal, vec3 viewDir)
{
    // calculate directions
    vec3 lightDir = normalize(plight.position - position_v);
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    // setup the light
    float diff = max(dot(lightDir, normal), 0.0);
    float spec = 0.0;
    if (plight.blinn)
        spec = pow(max(dot(normal, halfwayDir), 0.0), 8.0f);
    else
        spec = pow(max(dot(viewDir, reflectDir), 0.0), 8.0f);

    float dist = length(plight.position - position_v);
    // attenuation - loss of light intensity over distance
    float attenuation = 1.0 / (plight.constant + plight.linear * dist + plight.quadratic * (dist * dist));

    // final properties
    vec3 ambient  = plight.ambient  * vec3(texture(textureSampler, texCoord_v)) * vec3(0.01);
    vec3 diffuse  = plight.diffuse  * diff * vec3(texture(textureSampler, texCoord_v));
    vec3 specular = plight.specular * spec * vec3(texture(roughSampler, texCoord_v)) * vec3(0.01);
    
    // if texture is a couple of frames
    if(combinedTexture) 
    {
        ambient = plight.ambient * vec3(sampleTexture()) * vec3(0.01);
        diffuse = plight.diffuse  * diff * vec3(sampleTexture());
    }

    // combine light
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

float calculateFogFactor(float coords) 
{
    float res = 0.0;
    if (fog.type == 0) // linear
    {
        res = (fog.end - coords) / (fog.end - fog.start); 
    }
    else if (fog.type == 1) // exponential
    {
        res = exp(-fog.density * coords);
    }
    return (1.0 - clamp(res, 0.0, 1.0));
}
